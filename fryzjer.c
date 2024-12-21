#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "fryzjer.h"
#include "klient.h"

extern sem_t fotel[];
extern sem_t poczekalnia;
extern pthread_mutex_t kasa_mutex;

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

        // Zwolnienie fotela
        sem_post(&fotel[fotel_id]);
    }
}
