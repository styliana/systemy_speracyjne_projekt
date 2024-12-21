#ifndef KLIENT_H
#define KLIENT_H

#include <semaphore.h>

typedef struct {
    int id;                 // ID klienta
    sem_t* fotel;           // Semafor fotela
} Klient;

#endif // KLIENT_H
