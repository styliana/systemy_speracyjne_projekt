#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "klient.h"

extern sem_t fotel[];
extern sem_t poczekalnia;

void* klient_praca(void* arg) {
    Klient* klient = (Klient*) arg;

    printf("Klient %d: Przychodzę do salonu.\n", klient->id);

    // Czekaj na wolne miejsce w poczekalni
    if (sem_trywait(&poczekalnia) == 0) {
        printf("Klient %d: Siadam w poczekalni.\n", klient->id);
        // Przechodzimy do fryzjera
        sem_post(klient->fotel);
        // Symulacja płatności
        sleep(1);
        printf("Klient %d: Zakończyłem strzyżenie i opuszczam salon.\n", klient->id);
    } else {
        printf("Klient %d: Brak miejsca w poczekalni, opuszczam salon.\n", klient->id);
    }

    return NULL;
}
