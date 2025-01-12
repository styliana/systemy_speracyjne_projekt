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
#define LICZBA_MIEJSC_W_POCZEKALNI 5  // Liczba miejsc w poczekalni
#define KOMUNIKAT_TYP 1                 // Typ komunikatu w poczekalni
#define NOMINALY 3
#define CZAS_USLUGI 5


// Deklaracje zmiennych globalnych
extern int poczekalnia, salon;  // Kolejki komunikatów
extern sem_t fotele;  // Semafory 
extern pthread_mutex_t kasa_mutex, poczekalnia_mutex; // Mutexy
extern int salon_otwarty;             // Zmienna pracy salonu
extern int start_hour;             // Godzina rozpoczęcia pracy
extern int godzina, minuta;        // Godzina i minuta symulacji

// Komunikaty
struct komunikat_poczekalnia
{
    unsigned long mtype;
    unsigned long klient_id;
};

struct komunikat_salon
{
    unsigned long mtype;
    unsigned long podpis;
    unsigned int msg[NOMINALY];
};

// Funkcje
void sprawdz_godzine_startu();
int suma_banknoty(int* banknoty);
void odbierz_komunikat_salon(struct komunikat_salon* kom, long odbiorca);
void wyslij_komunikat_salon(struct komunikat_salon* kom);
void odbierz_komunikat_poczekalnia(struct komunikat_poczekalnia* kom, long odbiorca);
void wyslij_komunikat_poczekalnia(struct komunikat_poczekalnia* kom);

#endif // CONFIG_H
