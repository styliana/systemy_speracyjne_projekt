#ifndef KLIENT_H
#define KLIENT_H

#include <semaphore.h>

typedef struct {
    int id;                 // ID klienta
    sem_t* fotel;           // Semafor fotela
} Klient;

// Deklaracja funkcji
void* klient_praca(void* arg);

#endif // KLIENT_H
