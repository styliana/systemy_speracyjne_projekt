#include "config.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

extern int kasa[];

static void sygnal_1(int sig) {
}

void *fryzjer_praca(void *arg) {
    int id;
    unsigned long ja, klient_id;
    struct komunikat kom;

    id = (long)arg;  // ID fryzjera
    ja = pthread_self();

    while (1) {
        // Czeka na klienta
        odbierz_komunikat(&kom, KOMUNIKAT_POCZEKALNIA);

        // Czeka z klientem na fotel
        klient_id = kom.podpis;

        sem_wait(&fotele); // Zajmujemy fotel

        // Losowa cena strzyżenia w pełnych dziesiątkach (od 30 do 100 zł)
        int cena_uslugi = (rand() % 8 + 3) * 10;

        // Pobranie pieniędzy od klienta przed strzyżeniem
        printf("\033[0;34m[FRYZJER %d]: Proszę klienta %ld o zapłatę %d zł za strzyżenie.\033[0m\n", id, klient_id, cena_uslugi);
        // Podaje cenę klientowi
        kom.mtype = klient_id;
        kom.podpis = ja;
        kom.msg[0] = cena_uslugi;
        wyslij_komunikat(&kom);

        // Odbiera zapłatę i wkłada do wspólnej kasy
        odbierz_komunikat(&kom, ja);
        dodaj_do_kasy(kom.msg, kasa);
        int zaplacono = suma_banknoty(kom.msg);
        int reszta =  zaplacono - cena_uslugi;
        printf("\033[0;34m[FRYZJER %d]: Otrzymałem od klienta %ld o zapłatę %d zł za strzyżenie, nadwyzka: %d.\033[0m\n", id, klient_id, zaplacono, reszta);

        // Obsługuje klienta
        printf("\033[0;34m[FRYZJER %d]: Obsługuję klienta.\033[0m\n", id);
        sleep(CZAS_USLUGI);  // Czas obsługi

        // Zakończenie obsługi klienta
        printf("\033[0;34m[FRYZJER %d]: Zakończyłem strzyżenie klienta, zwalniam fotel.\033[0m\n", id);
        sem_post(&fotele);
        

        // Przygotuj komunikat z resztą dla klienta
        kom.mtype = klient_id;
        kom.podpis = ja;
        kom.msg[0] = 0;
        kom.msg[1] = 0;
        kom.msg[2] = 0;
        if (reszta) {
            pthread_mutex_lock(&kasa_mutex);
            
            //kasa[0] = 0; // TEST BRAKU BANKNOTÓW ZALICZONY
            
            // Jeśli nadwyżka została zapłacona, wydaj resztę
            while (reszta > 0) {  // Dopóki reszta nie zostanie zapłacona
                // Sprawdzenie, czy jest wystarczająca ilość banknotów w kasie do wydania reszty
                if (kasa[2] > 0 && reszta >= 50) {
                    kasa[2]--;  // Zmniejsz liczbę 50zł w kasie
                    kom.msg[2]++; // Przekaż klientowi
                    reszta -= 50;  // Zmniejsz resztę
                } else if (kasa[1] > 0 && reszta >= 20) {
                    kasa[1]--;  // Zmniejsz liczbę 20zł w kasie
                    kom.msg[1]++; // Przekaż klientowi
                    reszta -= 20;  // Zmniejsz resztę
                } else if (kasa[0] > 0 && reszta >= 10) {
                    kasa[0]--;  // Zmniejsz liczbę 10zł w kasie
                    kom.msg[0]++; // Przekaż klientowi
                    reszta -= 10;  // Zmniejsz resztę
                } else if (kasa[0] == 0) {
                // Jeśli kasa nie ma odpowiednich banknotów, czekaj na nowe wpłaty
                    printf("\033[0;34m[FRYZJER %d]: Brak wystarczających środków na resztę, czekam.\033[0m\n", id);
                    pthread_mutex_unlock(&kasa_mutex);
                    sleep(3);  // Czekaj na nowe wpłaty
                    pthread_mutex_lock(&kasa_mutex);
                }
            }
            pthread_mutex_unlock(&kasa_mutex);
        }

        // Przekazanie reszty
        printf("\033[0;34m[FRYZJER %d]: Przekazuję klientowi %ld resztę %d zł.\033[0m\n", id, klient_id, suma_banknoty(kom.msg));
        wyslij_komunikat(&kom);
    }

    fprintf(stderr, "Wypadłem z pętli fryzjera %ld", pthread_self());
    return NULL;
}