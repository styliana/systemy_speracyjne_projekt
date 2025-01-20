#include "kierownik.h"

int godzina = 8;     // Początkowa godzina
int minuta = 0;      // Początkowa minuta

pid_t klienci[MAX_KLIENTOW];
pid_t fryzjerzy[MAX_FRYZJEROW];
pthread_t symulacja_czasu; // Wątek symulacji czasu
int s1 = 0, s2 = 0; // Flagi wysłania sygnałów

int fotele, poczekalnia, kasa; // Semafory
int kolejka; // Kolejka komunikatów
int pamiec_id; // ID pamięci dzielonej
int *pamiec; // Pamięć dzielona

int main() {
    if (signal(SIGINT, szybki_koniec) == SIG_ERR)
    {
        perror("Blad obslugi sygnalu");
        exit(EXIT_FAILURE);
    }
    
    sprawdz_limit_procesow();
    srand(time(NULL));

    key_t klucz;

    // Inicjalizacja kolejki komunikatów
    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_KOLEJKA);
    kolejka = utworz_kolejke_komunikatow(klucz);

    // Inicjalizacja semaforów
    klucz = ftok(KLUCZ_PATH,KLUCZ_CHAR_SEM_KASA);
    kasa = utworz_semafor(klucz);
    ustaw_semafor(kasa, 1);
    
    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_SEM_FOTELE);
    fotele = utworz_semafor(klucz);
    ustaw_semafor(fotele, MAX_FOTELI);

    klucz = ftok(KLUCZ_PATH,KLUCZ_CHAR_SEM_POCZEKALNIA);
    poczekalnia = utworz_semafor(klucz);
    ustaw_semafor(poczekalnia, MAX_POCZEKALNIA);

    printf("\033[0;36m[INFO]: init poczekalnia: %d.\033[0m\n", sem_wartosc(poczekalnia));
    printf("\033[0;36m[INFO]: init fotele: %d.\033[0m\n", sem_wartosc(fotele));

    // Inicjalizacja pamięci dzielonej
    klucz = ftok(KLUCZ_PATH,KLUCZ_CHAR_PAMIEC);
    pamiec_id = utworz_pamiec_dzielona(klucz);
    pamiec = dolacz_pamiec_dzielona(pamiec_id);

    pamiec[0] = 10;
    pamiec[1] = 10;
    pamiec[2] = 0;

    // Sprawdzamy godzinę rozpoczęcia symulacji
    sprawdz_godzine_startu(&godzina, &minuta);

    // Tworzymy wątek do symulacji czasu
    pthread_create(&symulacja_czasu, NULL, symuluj_czas, (void *)NULL);

    // Tworzymy fryzjerów
    tworz_fryzjerow();

    // Tworzymy klientów
    tworz_klientow();

    // Możemy wysyłać sygnały lub zakończyć program
    char menu;
    while (menu != '3') {
        printf("1 - zakończ fryzjerów\n");
        printf("2 - zakończ klientów\n");
        printf("3 - koniec\n");

        while (getchar() != '\n'); // Czeszczenie bufora
        while(scanf("%c", &menu) != 1);
        switch (menu)
        {
        case '1':
            wyslij_s1();
            tworz_fryzjerow();
            break;
        case '2':
            wyslij_s2();
            tworz_klientow();
            break;
        case '3':
            koniec(0);
            break;
        default:
            printf("Niepoprawna opcja\n");
            break;
        }
    }

    exit(EXIT_FAILURE);
}

void tworz_fryzjerow() {
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        fryzjerzy[i] = fork();
        if (fryzjerzy[i] == 0) {
            execl("./fryzjer", "fryzjer", NULL);
        }
        printf("\033[0;36m[INFO]: Nowy fryzjer %d\033[0m\n", fryzjerzy[i]);
    }
}

void tworz_klientow() {
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        klienci[i] = fork();
        if (klienci[i] == 0) {
            execl("./klient", "klient", NULL);
        }
        printf("\033[0;36m[INFO]: Nowy klient %d\033[0m\n", klienci[i]);

        // Losowy czas oczekiwania przed przyjściem kolejnego klienta (od 4 do 10 sekund)
        int czas_oczekiwania = rand() % 3 + 10;
        sleep(czas_oczekiwania);
    }
}

void koniec(int s) {
    // Czekamy na zakończenie symulacji czasu
    zakoncz_symulacje_czasu();

    // Czekamy na zakończenie wszystkich klientów i fryzjerów
    wyslij_s1();
    wyslij_s2();

    // Zwalniamy zasoby
    zwolnij_zasoby();

    exit(EXIT_SUCCESS);
}

void szybki_koniec(int s) {
    zwolnij_zasoby();
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        kill(fryzjerzy[i], SIGKILL);
    }
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        kill(klienci[i], SIGKILL);
    }
    czekaj_na_procesy(MAX_FRYZJEROW);
    czekaj_na_procesy(MAX_KLIENTOW);

    zakoncz_symulacje_czasu();

    exit(EXIT_SUCCESS);
}

void wyslij_s1() {
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        kill(fryzjerzy[i], 1);
    }
    czekaj_na_procesy(MAX_FRYZJEROW);
}

void wyslij_s2() {
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        kill(klienci[i], 2);
    }
    czekaj_na_procesy(MAX_KLIENTOW);
}

void zakoncz_symulacje_czasu() {
    pthread_cancel(symulacja_czasu);
    pthread_join(symulacja_czasu, NULL);
}

void czekaj_na_procesy(int n) {
    int wPID, status;
	for(int i = 0; i < n; i++) {
		wPID = wait(&status);
		if(wPID == -1) {
			perror("Blad podczas wykonywania funkcji wait");
			exit(EXIT_FAILURE);
		} else {
            printf("\033[0;36m[INFO]: Koniec procesu: %d, status: %d\n", wPID, status);
		}
	}
}

void zwolnij_zasoby() {
    usun_kolejke_komunikatow(kolejka);
    usun_semafor(kasa);
    usun_semafor(fotele);
    usun_semafor(poczekalnia);
    odlacz_pamiec_dzielona(pamiec);
    usun_pamiec_dzielona(pamiec_id);

    printf("\033[0;36m[INFO]: Zasoby zwolnione\033[0m\n");
}

void sprawdz_limit_procesow() {
    struct rlimit rl;
    
    if (getrlimit(RLIMIT_NPROC, &rl) != 0) {
        perror("getrlimit");
        exit(EXIT_FAILURE);
    }

    if (rl.rlim_cur < MAX_FRYZJEROW + MAX_KLIENTOW) {
        fprintf(stderr, "Nie mogę stworzyć tak wielu procesów! Limit: %ld", rl.rlim_cur);
        exit(EXIT_FAILURE);    
    }
}

void print_stan_kasy() {
    sem_p(kasa, 1);
    printf("\033[0;36m[INFO]: Stan kasy: %d zł.\033[0m\n", suma_banknoty(pamiec));
    sem_v(kasa, 1);
}

void *symuluj_czas(void *arg) {
    while (1) {
        sleep(1);

        minuta++;
        if (minuta == 60) {
            minuta = 0;
            godzina++;
            // Otwarcie
            if (godzina == 8) {
                printf("\033[0;36m[INFO]: Otwarcie salonu.\033[0m\n");
                print_stan_kasy();

                // Otwieramy salon
                sem_v(poczekalnia, MAX_POCZEKALNIA);
            }
            else if (godzina == 24) {
                godzina = 0;
            }
        }
        // Zamknięcie
        if (godzina == 16 && minuta == 30) {
            printf("\033[0;36m[INFO]: Zamknięcie salonu za 30 min, zaraz zamykamy.\033[0m\n");

            // Zamykamy salon
            sem_p(poczekalnia, MAX_POCZEKALNIA);

            printf("\033[0;36m[INFO]: Zamknięto poczekalnię.\033[0m\n");

            printf("\033[0;36m[INFO]: poczekalnia: %d.\033[0m\n", sem_wartosc(poczekalnia));
            printf("\033[0;36m[INFO]: fotele: %d.\033[0m\n", sem_wartosc(fotele));

            // Wyświetlenie końcowego stanu kasy po dniu
            print_stan_kasy();
        }

        // Co 5 minut wyświetlamy godzinę
        if (minuta % 5 == 0) {
            printf("\033[0;36m[INFO]: Aktualna godzina: %02d:%02d\033[0m\n", godzina, minuta);
            printf("\033[0;36m[INFO]: poczekalnia: %d.\033[0m\n", sem_wartosc(poczekalnia));
            printf("\033[0;36m[INFO]: fotele: %d.\033[0m\n", sem_wartosc(fotele));
        }
        
        if (minuta % 20 == 0) {
        // Co 20 minut wyświetlamy stan kasy
            print_stan_kasy();  // Wyświetlenie stanu kasy
        }
    }

    return NULL;
}

void sprawdz_godzine_startu(int* godzina, int* minuta) {
    int godzina_startu, minuta_startu;
    char czas_startu[6];  // Bufor na format hh:mm

    while (1) {
        printf("Podaj godzinę rozpoczęcia symulacji w formacie hh:mm (od 8:00 do 16:00): ");
        
        if (scanf("%5s", czas_startu) != 1) {
            while (getchar() != '\n');
            printf("\033[0;31m[ERROR]: Wprowadzono nieprawidłową godzinę. Spróbuj ponownie.\033[0m\n");
            continue;
        }

        if (sscanf(czas_startu, "%d:%d", &godzina_startu, &minuta_startu) != 2) {
            printf("\033[0;31m[ERROR]: Wprowadzono nieprawidłowy format godziny. Spróbuj ponownie.\033[0m\n");
            continue;
        }

        if (godzina_startu < 8 || godzina_startu > 16) {
            // TODO:
            printf("\033[0;31m[ERROR]: Godzina %d:%02d jest niedozwolona (8:00 - 16:00).\033[0m\n", godzina_startu, minuta_startu);
            continue;
        }

        if (minuta_startu < 0 || minuta_startu >= 60) {
            printf("\033[0;31m[ERROR]: Minuty %02d są niepoprawne. Spróbuj ponownie.\033[0m\n", minuta_startu);
            continue;
        }

        *godzina = godzina_startu;
        *minuta = minuta_startu;
        break;
    }
}

