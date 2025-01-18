#include "klient.h"

long ja;
int poczekalnia; // Semafor
int kolejka; // Kolejka komunikatów
int zajmuje_poczekalnie = 0;

int main() {
    ja = getpid();
    if (signal(SIGINT, sygnal_2) == SIG_ERR)
    {
        perror("Blad obslugi sygnalu");
        exit(EXIT_FAILURE);
    }

    long fryzjer_id;
    key_t klucz;

    struct komunikat kom;
    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_KOLEJKA);
    kolejka = utworz_kolejke_komunikatow(klucz);

    klucz = ftok(KLUCZ_PATH,KLUCZ_CHAR_SEM_POCZEKALNIA);
    poczekalnia = utworz_semafor(klucz);

    while (1) {
        // Klient idzie do poczekalni
        printf("\033[0;32m[KLIENT %ld]: Przyszedłem do salonu.\033[0m\n", ja);

        // Jeśli jest miejsce, klient siada w poczekalni
        int wolne = sem_p_nowait(poczekalnia, 1);

        if (wolne == 0) {
            zajmuje_poczekalnie = 1;
            printf("\033[0;32m[KLIENT %ld]: Siadam w poczekalni.\033[0m\n", ja);
            printf("\033[0;32m[KLIENT %ld]: Jest jeszcze %d miejsc.\033[0m\n", ja, sem_wartosc(poczekalnia));

            kom.mtype = KOMUNIKAT_POCZEKALNIA;
            kom.podpis = ja;
            wyslij_komunikat(kolejka, &kom);

            // Czeka na komunikat od fryzjera, żeby usiąść na fotelu
            odbierz_komunikat(kolejka, &kom, ja);
            
            // Zwalnia miejsce w poczekalni i przechodzi do płacenia
            sem_v(poczekalnia, 1);
            printf("\033[0;32m[KLIENT %ld]: Wychodzę z poczekalni, widziałem tam %d miejsc.\033[0m\n", ja, sem_wartosc(poczekalnia));
            zajmuje_poczekalnie = 0;

            fryzjer_id = kom.podpis; // zapisuje id fryzjera
            printf("\033[0;32m[KLIENT %ld]: Zawołał mnie fryzjer %ld.\033[0m\n", ja, fryzjer_id);

            // Losowanie nadpłaty
            int cena = kom.msg[0];
            int nadwyzka = (rand() % 4) * 10;  // Wartości 0, 10, 20, 30
            int kwota_do_zaplaty = cena + nadwyzka;  // Kwota, którą klient płaci (z nadwyżką)
            int klient_dal_50 = 0, klient_dal_20 = 0, klient_dal_10 = 0;  // Liczniki dla każdego nominału
            int suma_zaplacona = 0;  // Zmienna do przechowywania sumy zapłaconej przez klienta

            // Przygotuj komunikat do fryzjera
            kom.mtype = fryzjer_id;
            kom.podpis = ja;
            kom.msg[0] = 0;
            kom.msg[1] = 0;
            kom.msg[2] = 0;

            while (kwota_do_zaplaty > 0) {  // Dopóki klient nie zapłaci pełnej kwoty
                int nominal = rand() % 3;  // Losowanie nominalu (0 - 10 zł, 1 - 20 zł, 2 - 50 zł)

                if (nominal == 0 && kwota_do_zaplaty >= 10) {
                    kwota_do_zaplaty -= 10;  // Zmniejsz resztę do zapłaty
                    klient_dal_10++;  // Zwiększ licznik 10zł
                    suma_zaplacona += 10;  // Dodaj 10zł do zapłaconej kwoty
                    kom.msg[0]++; // Przekaż fryzjerowi
                } else if (nominal == 1 && kwota_do_zaplaty >= 20) {
                    kwota_do_zaplaty -= 20;  // Zmniejsz resztę do zapłaty
                    klient_dal_20++;  // Zwiększ licznik 20zł
                    suma_zaplacona += 20;  // Dodaj 20zł do zapłaconej kwoty
                    kom.msg[1]++; // Przekaż fryzjerowi
                } else if (nominal == 2 && kwota_do_zaplaty >= 50) {
                    kwota_do_zaplaty -= 50;  // Zmniejsz resztę do zapłaty
                    klient_dal_50++;  // Zwiększ licznik 50zł
                    suma_zaplacona += 50;  // Dodaj 50zł do zapłaconej kwoty
                    kom.msg[2]++; // Przekaż fryzjerowi
                }
            }

            // Zapłać fryzjerowi
            wyslij_komunikat(kolejka, &kom);

            // Komunikaty o zapłacie
            printf("\033[0;32m[KLIENT %ld]: Zapłacono %d zł (w tym nadwyżka %d zł) w nominałach:\033[0m\n", ja, suma_zaplacona, nadwyzka);
            if (klient_dal_50 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 50zł\033[0m\n", ja, klient_dal_50);
            if (klient_dal_20 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 20zł\033[0m\n", ja, klient_dal_20);
            if (klient_dal_10 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 10zł\033[0m\n", ja, klient_dal_10);

            // Czekaj na zakończenie usługi i wydanie reszty
            odbierz_komunikat(kolejka, &kom, ja);

            // Sprawdzenie reszty
            int reszta = suma_banknoty(kom.msg);
            if (reszta == nadwyzka) {
                printf("\033[0;32m[KLIENT %ld]: Otrzymałem odpowiednią resztę %d zł.\033[0m\n", ja, reszta);
            } else {
                printf("\033[0;32m[KLIENT %ld]: Otrzymałem resztę %d zł, ale powinienem otrzymać %d zł.\033[0m\n", ja, reszta, nadwyzka);
            }

        } else {
            // Jeśli poczekalnia pełna, klient wychodzi
            printf("\033[0;32m[KLIENT %ld]: Nie mogę wejść do poczekalni, wracam do pracy.\033[0m\n", ja);
        }

        // Praca losowy czas
        sleep(rand() % 100 + 10);
    }

    fprintf(stderr, "Wypadłem z pętli klienta %ld", ja);
    exit(EXIT_FAILURE);
}

void sygnal_2(int sig) {
    printf("\033[0;32m[KLIENT %ld]: Otrzymałem sygnał 2, żegnam.\033[0m\n", ja);
    if (zajmuje_poczekalnie) {
        printf("\033[0;32m[KLIENT %ld]: Zwalniam swoje miejsce w poczekalni.\033[0m\n", ja);
        sem_v(poczekalnia, 1);
    }
    exit(EXIT_SUCCESS);
}