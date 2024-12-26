#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

void *fryzjer_praca(void *arg) {
    int id = (long)arg;

    while (1) {
        pthread_mutex_lock(&mutex_poczekalnia);

        // Czekaj na dostępnych klientów w poczekalni
        while (klienci_w_poczekalni == 0) {
            pthread_cond_wait(&cond_fryzjer, &mutex_poczekalnia);  // Czekaj na klienta
        }

        // Obsługuje klienta
        if (klienci_w_poczekalni > 0) {
            klienci_w_poczekalni--;  // Obsługujemy klienta
            wolne_fotele++;  // Zwolnij fotel
            printf("Fryzjer %d: Obsługuję klienta.\n", id);

            // Symulacja pracy fryzjera (czas obsługi klienta)
            sleep(2);  // Czas pracy fryzjera

            // Kasa fryzjera
            pthread_mutex_lock(&kasa_mutex);
            kasa[0] += 20;  // Dodajemy 20 PLN do kasy
            zarobki += 20;  // Fryzjer zarabia 20 PLN
            pthread_mutex_unlock(&kasa_mutex);

            // Obsługa zakończona, powiadom klienta
            printf("Fryzjer %d: Obsługa zakończona.\n", id);
            pthread_cond_signal(&cond_klient);  // Powiadom klienta, że usługa się zakończyła
        }

        pthread_mutex_unlock(&mutex_poczekalnia);  // Zwalniamy mutex
    }

    return NULL;
}
