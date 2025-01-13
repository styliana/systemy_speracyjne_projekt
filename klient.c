#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

// static volatile sig_atomic_t sygnal_do_wyjscia = 0;
// static int w_salonie = 0;

static void sygnal_2(int sig) {
    // sygnal_do_wyjscia = 1;
}

void do_salonu(long id);
void praca();

void *klient_praca(void *arg) {
    long id;

    id = (long)arg;  // ID klienta

    // DO ZRObienia: ?
    // struct sigaction sa;
    // sa.sa_handler = sygnal_2;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sigaction(SIGUSR2, &sa, NULL);  // Obsługa SIGUSR2

    // v2
    if (signal(SIGUSR2, sygnal_2) == SIG_ERR)
    {
        perror("Blad obslugi sygnalu");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // if (sygnal_do_wyjscia) {
        //     sygnal_do_wyjscia = 0;
        //     if (w_salonie) {
        //         printf("\033[0;32m[KLIENT %ld]: Otrzymałem sygnał do opuszczenia salonu, wracam do pracy.\033[0m\n", id);
        //         praca();
        //     }
        // } else {
            do_salonu(id);
            praca();
        // }
    }

    fprintf(stderr, "Wypadłem z pętli klienta %ld", pthread_self());
    return NULL;
}

void do_salonu(long id) {
    unsigned long ja, fryzjer_id;
    struct komunikat kom;

    ja = pthread_self();

    // Klient idzie do poczekalni
    if (!salon_otwarty) {
        printf("\033[0;32m[KLIENT %ld]: Przyszedłem do salonu, ale jest zamknięte, więc wracam do pracy.\033[0m\n", id);
    } else {
        printf("\033[0;32m[KLIENT %ld]: Przyszedłem do salonu, jestem %ld.\033[0m\n", id, ja);

        // Jeśli jest miejsce, klient siada w poczekalni
        int wolne = sem_trywait(&poczekalnia);


        if (wolne == 0) {
            printf("\033[0;32m[KLIENT %ld]: Siadam w poczekalni.\033[0m\n", id);

            int stan_poczekalni;
            sem_getvalue(&poczekalnia, &stan_poczekalni);
            printf("\033[0;32m[KLIENT %ld]: Widzę %d jeszcze wolnych miejsc w poczekalni.\033[0m\n", id, stan_poczekalni);
            
            kom.mtype = KOMUNIKAT_POCZEKALNIA;
            kom.podpis = ja;
            wyslij_komunikat(&kom);
            // w_salonie = 1;

            // Czeka na komunikat od fryzjera, żeby usiąść na fotelu
            odbierz_komunikat(&kom, ja);
            
            // Zwalnia miejsce w poczekalni i przechodzi do płacenia
            sem_post(&poczekalnia);

            fryzjer_id = kom.podpis; // zapisuje id fryzjera
            printf("\033[0;32m[KLIENT %ld]: Zawołał mnie fryzjer %ld.\033[0m\n", id, fryzjer_id);

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
            wyslij_komunikat(&kom);

            // Komunikaty o zapłacie
            printf("\033[0;32m[KLIENT %ld]: Zapłacono %d zł (w tym nadwyżka %d zł) w nominałach:\033[0m\n", id, suma_zaplacona, nadwyzka);
            if (klient_dal_50 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 50zł\033[0m\n", id, klient_dal_50);
            if (klient_dal_20 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 20zł\033[0m\n", id, klient_dal_20);
            if (klient_dal_10 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 10zł\033[0m\n", id, klient_dal_10);

            // Czekaj na zakończenie usługi i wydanie reszty
            odbierz_komunikat(&kom, ja);

            // Sprawdzenie reszty
            int reszta = suma_banknoty(kom.msg);
            if (reszta == nadwyzka) {
                printf("\033[0;32m[KLIENT %ld]: Otrzymałem odpowiednią resztę %d zł.\033[0m\n", id, reszta);
            } else {
                printf("\033[0;32m[KLIENT %ld]: Otrzymałem resztę %d zł, ale powinienem otrzymać %d zł.\033[0m\n", id, reszta, nadwyzka);
            }

        } else {
            // Jeśli poczekalnia pełna, klient wychodzi
            printf("\033[0;32m[KLIENT %ld]: Poczekalnia pełna, wracam do pracy.\033[0m\n", id);
        }
    }
    // w_salonie = 0;
}

void praca() {
    sleep(rand() % 100 + 50);
}