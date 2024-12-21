#ifndef KLIENT_H
#define KLIENT_H

typedef struct {
    int id;
    int kwota;
    int fotel;
} Klient;

void klient_generuj(Klient *klient, int id);

#endif
