#ifndef KLIENT_H
#define KLIENT_H

<<<<<<< HEAD
typedef struct {
    int id;
    int kwota;
    int fotel;
} Klient;

void klient_generuj(Klient *klient, int id);
=======
void *klient_routine(void *arg); // Funkcja wykonywana przez każdy wątek klienta
>>>>>>> 9bae1b699490254930a05064bc503b7665153041

#endif
