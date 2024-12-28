#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

void *klient_praca(void *arg) {
    int id = *(int *)arg;  // ID klienta
    free(arg);  // Zwalniamy pamięć po ID klienta

    if (praca_trwa) {
        printf("\033[0;32m[KLIENT %d]: Przyszedłem do salonu.\033[0m\n", id);

        // Sprawdzamy, czy jest dostępne miejsce w poczekalni
        pthread_mutex_lock(&mutex_poczekalnia);
        if (klienci_w_poczekalni < LICZBA_MIEJSC_W_POCZEKALNI) {
            // Jeśli jest miejsce, klient siada w poczekalni
            klienci_w_poczekalni++;
            printf("\033[0;32m[KLIENT %d]: Siadam w poczekalni.\033[0m\n", id);
            printf("\033[0;32m[KLIENT %d]: Dodałem się do poczekalni. Liczba oczekujących: %d.\033[0m\n", id, klienci_w_poczekalni);

            pthread_cond_signal(&cond_fryzjer);  // Powiadamiamy fryzjera, że klient jest dostępny
        } else {
            // Jeśli poczekalnia pełna, klient wychodzi
            printf("\033[0;31m[KLIENT %d]: Poczekalnia pełna, wracam do pracy.\033[0m\n", id);
            pthread_mutex_unlock(&mutex_poczekalnia);
            return NULL;  // Klient wychodzi i kończy swoją pracę
        }
        pthread_mutex_unlock(&mutex_poczekalnia);

        // Czekaj na zakończenie usługi
        sem_wait(&service_done);

        printf("\033[0;32m[KLIENT %d]: Zakończyłem usługę.\033[0m\n", id);
        sem_post(&fotele);  // Zwolnij fotel (fryzjer może go wykorzystać)
    }

    return NULL;
}
