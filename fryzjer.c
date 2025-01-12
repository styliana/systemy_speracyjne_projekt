#include "config.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

extern int kasa[];

static void sygnal_1(int sig) {
}

void *fryzjer_praca(void *arg) {
    int id;
    unsigned long ja, klient_id;
    struct komunikat_poczekalnia kom_poczekalnia;
    struct komunikat_salon kom_salon;

    id = (long)arg;  // ID fryzjera
    ja = pthread_self();

    while (1) {
        // Czeka na klienta
        odbierz_komunikat_poczekalnia(&kom_poczekalnia, KOMUNIKAT_TYP);

        // Czeka z klientem na fotel
        pthread_mutex_lock(&poczekalnia_mutex); // Klient jeszcze jest w poczekalni
        klient_id = kom_poczekalnia.klient_id;
        sem_wait(&fotele); // Zajmujemy fotel
        pthread_mutex_unlock(&poczekalnia_mutex); // Można już wejść do poczekalni

        // Losowa cena strzyżenia w pełnych dziesiątkach (od 30 do 100 zł)
        int cena_uslugi = (rand() % 8 + 3) * 10;

        // Pobranie pieniędzy od klienta przed strzyżeniem
        printf("\033[0;34m[FRYZJER %d]: Proszę klienta %ld o zapłatę %d zł za strzyżenie.\033[0m\n", id, klient_id, cena_uslugi);
        // Podaje cenę klientowi
        kom_salon.mtype = klient_id;
        kom_salon.podpis = ja;
        kom_salon.msg[0] = cena_uslugi;
        kom_salon.msg[1] = 0;
        kom_salon.msg[2] = 0;
        wyslij_komunikat_salon(&kom_salon);

        // Odbiera zapłatę i wkłada do wspólnej kasy
        odbierz_komunikat_salon(&kom_salon, ja);
        pthread_mutex_lock(&kasa_mutex);
        kasa[0] += kom_salon.msg[0];
        kasa[1] += kom_salon.msg[1];
        kasa[2] += kom_salon.msg[2];
        pthread_mutex_unlock(&kasa_mutex);
        int zaplacono = suma_banknoty(kom_salon.msg);
        int reszta =  zaplacono - cena_uslugi;
        printf("\033[0;34m[FRYZJER %d]: Otrzymałem od klienta %ld o zapłatę %d zł za strzyżenie, nadwyzka: %d.\033[0m\n", id, klient_id, zaplacono, reszta);

        // Obsługuje klienta
        printf("\033[0;34m[FRYZJER %d]: Obsługuję klienta.\033[0m\n", id);
        sleep(CZAS_USLUGI);  // Czas obsługi

        // Zakończenie obsługi klienta
        printf("\033[0;34m[FRYZJER %d]: Zakończyłem strzyżenie klienta, zwalniam fotel.\033[0m\n", id);
        sem_post(&fotele);

        // Przygotuj komunikat
        kom_salon.mtype = klient_id;
        kom_salon.podpis = ja;
        kom_salon.msg[0] = 0;
        kom_salon.msg[1] = 0;
        kom_salon.msg[2] = 0;
        if (reszta) {
            pthread_mutex_lock(&kasa_mutex);
            // Jeśli nadwyżka została zapłacona, wydaj resztę
            while (reszta > 0) {  // Dopóki reszta nie zostanie zapłacona
                // Sprawdzenie, czy jest wystarczająca ilość banknotów w kasie do wydania reszty
                if (kasa[2] > 0 && reszta >= 50) {
                    kasa[2]--;  // Zmniejsz liczbę 50zł w kasie
                    kom_salon.msg[2]++; // Przekaż klientowi
                    reszta -= 50;  // Zmniejsz resztę
                } else if (kasa[1] > 0 && reszta >= 20) {
                    kasa[1]--;  // Zmniejsz liczbę 20zł w kasie
                    kom_salon.msg[1]++; // Przekaż klientowi
                    reszta -= 20;  // Zmniejsz resztę
                } else if (kasa[0] > 0 && reszta >= 10) {
                    kasa[0]--;  // Zmniejsz liczbę 10zł w kasie
                    kom_salon.msg[0]++; // Przekaż klientowi
                    reszta -= 10;  // Zmniejsz resztę
                } else if (kasa[0] == 0) {
                // Jeśli kasa nie ma odpowiednich banknotów, czekaj na nowe wpłaty
                    printf("\033[0;34m[FRYZJER %d]: Brak wystarczających środków na resztę, czekam.\033[0m\n", id);
                    pthread_mutex_unlock(&kasa_mutex);
                    sleep(2);  // Czekaj na nowe wpłaty
                    pthread_mutex_lock(&kasa_mutex);
                }
            }
            pthread_mutex_unlock(&kasa_mutex);
        }

        // Przekazanie reszty
        printf("\033[0;34m[FRYZJER %d]: Przekazuję klientowi %ld resztę %d zł.\033[0m\n", id, klient_id, suma_banknoty(kom_salon.msg));
        wyslij_komunikat_salon(&kom_salon);
    }

    fprintf(stderr, "Wypadłem z pętli fryzjera %ld", pthread_self());
    return NULL;
}