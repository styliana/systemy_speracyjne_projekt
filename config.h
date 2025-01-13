#ifndef CONFIG_H
#define CONFIG_H

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_FOTELI 3                  // Maksymalna liczba foteli w salonie
#define MAX_KLIENTOW 10               // Maksymalna liczba klientów
#define MAX_FRYZJEROW 5               // Maksymalna liczba fryzjerów
#define MAX_POCZEKALNIA 5  // Liczba miejsc w poczekalni
#define KOMUNIKAT_POCZEKALNIA 1                 // Typ komunikatu w poczekalni
#define NOMINALY 3
#define CZAS_USLUGI 5


// Deklaracje zmiennych globalnych
extern int salon;  // Kolejki komunikatów
extern sem_t fotele, poczekalnia;  // Semafory 
extern pthread_mutex_t kasa_mutex; // Mutexy
extern int salon_otwarty;             // Zmienna pracy salonu
extern int start_hour;             // Godzina rozpoczęcia pracy
extern int godzina, minuta;        // Godzina i minuta symulacji

// Komunikaty
struct komunikat
{
    unsigned long mtype;
    unsigned long podpis;
    unsigned int msg[NOMINALY];
};

// Funkcje
void sprawdz_godzine_startu();
int suma_banknoty(int* banknoty);
void dodaj_do_kasy(int* banknoty, int* kasa);
void odbierz_komunikat(struct komunikat* kom, long odbiorca);
void wyslij_komunikat(struct komunikat* kom);

#endif // CONFIG_H
