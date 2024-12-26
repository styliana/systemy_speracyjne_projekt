#include "config.h"
#include "fryzjer.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *klient_praca(void *arg) {
    int id = (long)arg;

    printf("Klient %d: Przychodzę do salonu.\n", id);

    // Zabezpiecz dostęp do poczekalni
    pthread_mutex_lock(&mutex_poczekalnia);

    // Jeśli są wolne miejsca w poczekalni
    if (klienci_w_poczekalni < LICZBA_MIEJSC_W_POCZEKALNI) {
        klienci_w_poczekalni++;  // Klient siada w poczekalni
        wolne_fotele--;  // Zajmujemy fotel
        printf("Klient %d: Siadam w poczekalni.\n", id);

        // Zbudź fryzjera, jeśli jest dostępny
        pthread_cond_signal(&cond_fryzjer);  // Sygnał dla fryzjera

        // Czekaj na obsługę
        pthread_cond_wait(&cond_klient, &mutex_poczekalnia);
        printf("Klient %d: Opuszczam salon zadowolony.\n", id);
    } else {
        printf("Klient %d: Brak miejsca w poczekalni. Wychodzę.\n", id);
    }

    // Po zakończeniu usługi zwalniamy fotel
    pthread_mutex_unlock(&mutex_poczekalnia);

    return NULL;
}
