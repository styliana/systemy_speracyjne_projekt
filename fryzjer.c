#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "fryzjer.h"

extern sem_t fotel[];
extern sem_t poczekalnia;
extern pthread_mutex_t kasa_mutex;
extern int przychod;  // Deklaracja zmiennej przychodu

void* fryzjer_praca(void* arg) {
    Fryzjer* fryzjer = (Fryzjer*) arg;
    int fotel_id = fryzjer->id - 1; // Indeks fotela

    while (1) {
        // Czekaj na klienta w poczekalni
        sem_wait(fryzjer->poczekalnia);
        printf("Fryzjer %d: Zajmuję fotel.\n", fryzjer->id);
        
        // Zajmowanie fotela
        sem_wait(&fotel[fotel_id]);

        // Symulacja strzyżenia
        sleep(2);  // Praca fryzjera
        printf("Fryzjer %d: Kończę strzyżenie.\n", fryzjer->id);

        // Aktualizacja przychodu
        pthread_mutex_lock(&kasa_mutex);
        przychod += CENA_USLUGI;  // Dodanie kosztu usługi
        pthread_mutex_unlock(&kasa_mutex);

        // Zwolnienie fotela
        sem_post(&fotel[fotel_id]);
    }
}
