#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

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

    // DO ZROBIENIA -- zmiany dot sygnalow od kierownika
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
    struct komunikat_poczekalnia kom_poczekalnia;
    struct komunikat_salon kom_salon;

    ja = pthread_self();
    kom_poczekalnia.mtype = KOMUNIKAT_TYP;
    kom_poczekalnia.klient_id = ja;

    // Klient idzie do poczekalni
    if (!salon_otwarty) {
        printf("\033[0;32m[KLIENT %ld]: Przyszedłem do salonu, ale jest zamknięte, więc wracam do pracy.\033[0m\n", id);
    } else {
        printf("\033[0;32m[KLIENT %ld]: Przyszedłem do salonu, jestem %ld.\033[0m\n", id, ja);

        // Sprawdzamy, czy jest dostępne miejsce w poczekalni
        struct msqid_ds buf;
        pthread_mutex_lock(&poczekalnia_mutex);
        if (msgctl(poczekalnia, IPC_STAT, &buf) == -1) {
            perror("Nie udało sie sprawdzić poczekalni");
            exit(EXIT_FAILURE);
        }

        // Jeśli jest miejsce, klient siada w poczekalni
        if (buf.msg_qnum < LICZBA_MIEJSC_W_POCZEKALNI) {
            printf("\033[0;32m[KLIENT %ld]: Siadam w poczekalni.\033[0m\n", id);
            wyslij_komunikat_poczekalnia(&kom_poczekalnia);
            // w_salonie = 1;
            pthread_mutex_unlock(&poczekalnia_mutex);

            // Czeka na komunikat od fryzjera, żeby usiąść na fotelu
            odbierz_komunikat_salon(&kom_salon, ja);

            fryzjer_id = kom_salon.podpis;
            printf("\033[0;32m[KLIENT %ld]: fryzjer %ld.\033[0m\n", id, fryzjer_id);

            int cena = kom_salon.msg[0];
            printf("\033[0;32m[KLIENT %ld]: cena %d.\033[0m\n", id, cena);
            int nadwyzka = (rand() % 4) * 10;  // Wartości 0, 10, 20, 30
            printf("\033[0;32m[KLIENT %ld]: nadwyzka %d.\033[0m\n", id, nadwyzka);
            int kwota_do_zaplaty = cena + nadwyzka;  // Kwota, którą klient płaci (z nadwyżką)
            int klient_dal_50 = 0, klient_dal_20 = 0, klient_dal_10 = 0;  // Liczniki dla każdego nominału
            int suma_zaplacona = 0;  // Zmienna do przechowywania sumy zapłaconej przez klienta

            // Przygotuj komunikat do fryzjera
            kom_salon.mtype = fryzjer_id;
            kom_salon.podpis = ja;
            kom_salon.msg[0] = 0;
            kom_salon.msg[1] = 0;
            kom_salon.msg[2] = 0;

            while (kwota_do_zaplaty > 0) {  // Dopóki klient nie zapłaci pełnej kwoty
                int nominal = rand() % 3;  // Losowanie nominalu (0 - 10 zł, 1 - 20 zł, 2 - 50 zł)

                if (nominal == 0 && kwota_do_zaplaty >= 10) {
                    kwota_do_zaplaty -= 10;  // Zmniejsz resztę do zapłaty
                    klient_dal_10++;  // Zwiększ licznik 10zł
                    suma_zaplacona += 10;  // Dodaj 10zł do zapłaconej kwoty
                    kom_salon.msg[0]++; // Przekaż fryzjerowi
                } else if (nominal == 1 && kwota_do_zaplaty >= 20) {
                    kwota_do_zaplaty -= 20;  // Zmniejsz resztę do zapłaty
                    klient_dal_20++;  // Zwiększ licznik 20zł
                    suma_zaplacona += 20;  // Dodaj 20zł do zapłaconej kwoty
                    kom_salon.msg[1]++; // Przekaż fryzjerowi
                } else if (nominal == 2 && kwota_do_zaplaty >= 50) {
                    kwota_do_zaplaty -= 50;  // Zmniejsz resztę do zapłaty
                    klient_dal_50++;  // Zwiększ licznik 50zł
                    suma_zaplacona += 50;  // Dodaj 50zł do zapłaconej kwoty
                    kom_salon.msg[2]++; // Przekaż fryzjerowi
                }
            }

            // Zapłać fryzjerowi
            wyslij_komunikat_salon(&kom_salon);

            // Komunikaty o zapłacie
            printf("\033[0;32m[KLIENT %ld]: Zapłacono %d zł (w tym nadwyżka %d zł) w nominałach:\033[0m\n", id, suma_zaplacona, nadwyzka);
            if (klient_dal_50 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 50zł\033[0m\n", id, klient_dal_50);
            if (klient_dal_20 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 20zł\033[0m\n", id, klient_dal_20);
            if (klient_dal_10 > 0) printf("\033[0;32m[KLIENT %ld]: %d x 10zł\033[0m\n", id, klient_dal_10);

            // Czekaj na zakończenie usługi i wydanie reszty
            odbierz_komunikat_salon(&kom_salon, ja);

            // Sprawdzenie reszty
            int reszta = suma_banknoty(kom_salon.msg);
            if (reszta == nadwyzka) {
                printf("\033[0;32m[KLIENT %ld]: Otrzymałem resztę %d zł.\033[0m\n", id, reszta);
            } else {
                printf("\033[0;32m[KLIENT %ld]: Otrzymałem resztę %d zł, ale powinienem otrzymać %d zł.\033[0m\n", id, reszta, nadwyzka);
            }

        } else {
            // Jeśli poczekalnia pełna, klient wychodzi
            pthread_mutex_unlock(&poczekalnia_mutex);
            printf("\033[0;32m[KLIENT %ld]: Poczekalnia pełna, wracam do pracy.\033[0m\n", id);
        }
    }
    // w_salonie = 0;
}

void praca() {
    sleep(rand() % 100 + 50);
}