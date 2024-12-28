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

        sem_wait(&fotele);  // Czekaj na dostępność fotela

        printf("\033[0;32m[KLIENT %d]: Siadam w poczekalni.\033[0m\n", id);

        pthread_mutex_lock(&mutex_poczekalnia);
        if (klienci_w_poczekalni < LICZBA_MIEJSC_W_POCZEKALNI) {
            klienci_w_poczekalni++;
            printf("\033[0;32m[KLIENT %d]: Dodałem się do poczekalni. Liczba oczekujących: %d.\033[0m\n", id, klienci_w_poczekalni);

            pthread_cond_signal(&cond_fryzjer);  // Powiadamiamy fryzjera, że klient jest dostępny
        } else {
            printf("\033[0;31m[KLIENT %d]: Poczekalnia pełna, czekam na wolne miejsce.\033[0m\n", id);
        }
        pthread_mutex_unlock(&mutex_poczekalnia);

        sem_wait(&service_done);  // Czekaj na zakończenie usługi

        printf("\033[0;32m[KLIENT %d]: Zakończyłem usługę.\033[0m\n", id);
        sem_post(&fotele);  // Zwolnij fotel (fryzjer może go wykorzystać)
    }

    return NULL;
}