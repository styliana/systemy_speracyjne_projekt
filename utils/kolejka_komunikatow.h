#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "../config.h"

// Komunikaty
struct komunikat
{
    unsigned long mtype;
    unsigned long podpis;
    unsigned int msg[NOMINALY];
};

int utworz_kolejke_komunikatow(key_t klucz);
void odbierz_komunikat(int kolejka, struct komunikat* kom, long odbiorca);
void wyslij_komunikat(int kolejka, struct komunikat* kom);
void usun_kolejke_komunikatow(int kolejka);
