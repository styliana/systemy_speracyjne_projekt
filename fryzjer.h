#ifndef FRYZJER_H
#define FRYZJER_H

#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int id;                 // ID fryzjera
    sem_t *fotel;           // Semafor do zajmowania fotela
    sem_t *poczekalnia;     // Semafor poczekalni
    sem_t *kasa;            // Semafor kasowy
    pthread_mutex_t *mutex_kasa;  // Mutex do kasy
} Fryzjer;

void *fryzjer_praca(void *arg);

#endif
