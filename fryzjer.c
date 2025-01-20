#include "fryzjer.h"

long ja;
long klient_id;
int fotele, kasa; // Semafory
int kolejka; // Kolejka komunikatów
int pamiec_id; // Pamięć dzielona
int *pamiec;
volatile sig_atomic_t zajmuje_fotel = 0, zajmuje_kase = 0;

// Flagi postępu działania programu, bezpiecznie dostępne przy obsłudze sygnału
volatile sig_atomic_t odebralem_komunikat_poczekalnia = 0;
volatile sig_atomic_t wyslalem_cene = 0;
volatile sig_atomic_t odebralem_zaplate = 0;
volatile sig_atomic_t wyslalem_reszte = 0;
volatile sig_atomic_t otrzymano_sygnal_do_wyjscia = 0;

int  main() {
    srand(time(NULL));
    ja = getpid();
    if (signal(SIGHUP, sygnal_1) == SIG_ERR)
    {
        perror("Blad obslugi sygnalu");
        exit(EXIT_FAILURE);
    }

    struct komunikat kom;
    key_t klucz;

    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_KOLEJKA);
    kolejka = utworz_kolejke_komunikatow(klucz);

    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_SEM_FOTELE);
    fotele = utworz_semafor(klucz);

    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_SEM_KASA);
    kasa = utworz_semafor(klucz);

    klucz = ftok(KLUCZ_PATH,KLUCZ_CHAR_PAMIEC);
    pamiec_id = utworz_pamiec_dzielona(klucz);
    pamiec = dolacz_pamiec_dzielona(pamiec_id);

    while (1) {
        if (otrzymano_sygnal_do_wyjscia) {
            break;
        }

        // Czeka na klienta
        if (odebralem_komunikat_poczekalnia != 1) {
            // printf("\033[0;34m[FRYZJER %ld]: odbieram komunikat poczekalnia\033[0m\n", ja);
            odbierz_komunikat(kolejka, &kom, KOMUNIKAT_POCZEKALNIA);
            odebralem_komunikat_poczekalnia = 1;
            // printf("\033[0;34m[FRYZJER %ld]: odebralem komunikat poczekalnia\033[0m\n", ja);
        }

        // Czeka z klientem na fotel
        klient_id = kom.podpis;

        if (!zajmuje_fotel) {
            sem_p(fotele, 1); // Zajmujemy fotel
            zajmuje_fotel = 1;
        }
        printf("\033[0;34m[FRYZJER %ld]: Udało się zająć fotel.\033[0m\n", ja);


        // Losowa cena strzyżenia w pełnych dziesiątkach (od 30 do 100 zł)
        int cena_uslugi = (rand() % 8 + 3) * 10;

        // Pobranie pieniędzy od klienta przed strzyżeniem
        printf("\033[0;34m[FRYZJER %ld]: Proszę klienta %ld o zapłatę %d zł za strzyżenie.\033[0m\n", ja, klient_id, cena_uslugi);
        // Podaje cenę klientowi
        kom.mtype = klient_id;
        kom.podpis = ja;
        kom.msg[0] = cena_uslugi;
        if (wyslalem_cene != 1) {
            // printf("\033[0;34m[FRYZJER %ld]: wysylam komunikat cena\033[0m\n", ja);
            wyslij_komunikat(kolejka, &kom);
            // printf("\033[0;34m[FRYZJER %ld]: wyslalem komunikat cena\033[0m\n", ja);
            wyslalem_cene = 1;
        }

        // Odbiera zapłatę i wkłada do wspólnej kasy
        if (odebralem_zaplate != 1) {
            // printf("\033[0;34m[FRYZJER %ld]: odbieram komunikat zaplata\033[0m\n", ja);
            odbierz_komunikat(kolejka, &kom, ja);
            // printf("\033[0;34m[FRYZJER %ld]: odebralem komunikat zaplata\033[0m\n", ja);
            odebralem_zaplate = 1;
        }
        
        sem_p(kasa, 1);
        zajmuje_kase = 1;
        pamiec[0] += kom.msg[0];
        pamiec[1] += kom.msg[1];
        pamiec[2] += kom.msg[2];
        sem_v(kasa, 1);
        zajmuje_kase = 0;

        int zaplacono = suma_banknoty(kom.msg);
        int reszta = zaplacono - cena_uslugi;
        printf("\033[0;34m[FRYZJER %ld]: Otrzymałem od klienta %ld o zapłatę %d zł za strzyżenie, nadwyzka: %d.\033[0m\n", ja, klient_id, zaplacono, reszta);

        // Obsługuje klienta
        printf("\033[0;34m[FRYZJER %ld]: Obsługuję klienta.\033[0m\n", ja);
        sleep(CZAS_USLUGI + rand()%3);  // Czas obsługi

        // Zakończenie obsługi klienta
        printf("\033[0;34m[FRYZJER %ld]: Zakończyłem strzyżenie klienta, zwalniam fotel.\033[0m\n", ja);
        if (zajmuje_fotel) {
            sem_v(fotele, 1);
            zajmuje_fotel = 0;
        }
        printf("\033[0;34m[FRYZJER %ld]: Udało się zwolnić fotel.\033[0m\n", ja);

        // Przygotuj komunikat z resztą dla klienta
        kom.mtype = klient_id;
        kom.podpis = ja;
        kom.msg[0] = 0;
        kom.msg[1] = 0;
        kom.msg[2] = 0;
        if (reszta) {
            sem_p(kasa, 1);
            zajmuje_kase = 1;
            
            // pamiec[0] = 0; // TEST BRAKU BANKNOTÓW ZALICZONY
            
            // Jeśli nadwyżka została zapłacona, wydaj resztę
            while (reszta > 0) {  // Dopóki reszta nie zostanie zapłacona
                // Sprawdzenie, czy jest wystarczająca ilość banknotów w kasie do wydania reszty
                if (pamiec[2] > 0 && reszta >= 50) {
                    pamiec[2]--;  // Zmniejsz liczbę 50zł w kasie
                    kom.msg[2]++; // Przekaż klientowi
                    reszta -= 50;  // Zmniejsz resztę
                } else if (pamiec[1] > 0 && reszta >= 20) {
                    pamiec[1]--;  // Zmniejsz liczbę 20zł w kasie
                    kom.msg[1]++; // Przekaż klientowi
                    reszta -= 20;  // Zmniejsz resztę
                } else if (pamiec[0] > 0 && reszta >= 10) {
                    pamiec[0]--;  // Zmniejsz liczbę 10zł w kasie
                    kom.msg[0]++; // Przekaż klientowi
                    reszta -= 10;  // Zmniejsz resztę
                } else if (pamiec[0] == 0) {
                // Jeśli kasa nie ma odpowiednich banknotów, czekaj na nowe wpłaty
                    printf("\033[0;34m[FRYZJER %ld]: Brak wystarczających środków na resztę, czekam.\033[0m\n", ja);
                    sem_v(kasa, 1);
                    zajmuje_kase = 0;
                    sleep(3);  // Czekaj na nowe wpłaty
                    sem_p(kasa, 1);
                    zajmuje_kase = 1;
                }
            }
            sem_v(kasa, 1);
            zajmuje_kase = 0;
        }

        // Przekazanie reszty
        printf("\033[0;34m[FRYZJER %ld]: Przekazuję klientowi %ld resztę %d zł.\033[0m\n", ja, klient_id, suma_banknoty(kom.msg));
        if (wyslalem_reszte != 1) {
            // printf("\033[0;34m[FRYZJER %ld]: wysylam komunikat reszta\033[0m\n", ja);
            wyslij_komunikat(kolejka, &kom);
            // printf("\033[0;34m[FRYZJER %ld]: wyslalem komunikat reszta\033[0m\n", ja);
            wyslalem_reszte = 1;
        }

        // Reset flag
        odebralem_komunikat_poczekalnia = 0;
        wyslalem_cene = 0;
        odebralem_zaplate = 0;
        wyslalem_reszte = 0;
    }

    zwolnij_zasoby();
    printf("\033[0;34m[FRYZJER %ld]: Żegnam.\033[0m\n", ja);
}

void sygnal_1(int sig) {
    printf("\033[0;34m[FRYZJER %ld]: Otrzymałem sygnał 1.\033[0m\n", ja);

    // Ustaw flagi
    if (odebralem_komunikat_poczekalnia) {
        otrzymano_sygnal_do_wyjscia = 1;
        
        if (wyslalem_cene != 1) {
            wyslalem_cene = -1;

        } else if (odebralem_zaplate != 1) {
            odebralem_zaplate = -1;

        } else if (wyslalem_reszte != 1) {
            wyslalem_reszte = -1;
        }
    } else {
        zwolnij_zasoby();
        printf("\033[0;34m[FRYZJER %ld]: Żegnam.\033[0m\n", ja);
        exit(EXIT_SUCCESS);
    }
    
    printf("\033[0;34m[FRYZJER %ld]: Sygnał ustawił flagi.\033[0m\n", ja);
}

void zwolnij_zasoby() {
    // Zwolnij semafory
    if (zajmuje_fotel) {
        printf("\033[0;34m[FRYZJER %ld]: Zwalniam fotel.\033[0m\n", ja);
        sem_v(fotele, 1);
    }
    if (zajmuje_kase) {
        printf("\033[0;34m[FRYZJER %ld]: Zwalniam kasę.\033[0m\n", ja);
        sem_v(kasa, 1);
    }
    // Odłącz pamięć
    odlacz_pamiec_dzielona(pamiec);
}