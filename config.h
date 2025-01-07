#ifndef CONFIG_H
#define CONFIG_H

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>  // For rand()
#include <unistd.h>  // For sleep()



#define MAX_FOTELI 3                  // Maksymalna liczba foteli w salonie
#define MAX_KLIENTOW 100              // Maksymalna liczba klientów
#define MAX_FRYZJEROW 5               // Maksymalna liczba fryzjerów
#define LICZBA_MIEJSC_W_POCZEKALNI 5  // Liczba miejsc w poczekalni

// Deklaracje zmiennych globalnych
extern sem_t fotele, klient, service_done;  // Semafory 
extern pthread_mutex_t kasa_mutex, mutex_poczekalnia; // Mutexy
extern pthread_cond_t cond_fryzjer, cond_klient; // Zmienne warunkowe
extern int klienci_w_poczekalni;   // Liczba klientów w poczekalni
extern int zarobki;                // Zarobki fryzjera
extern int praca_trwa;             // Zmienna pracy salonu
extern int start_hour;             // Godzina rozpoczęcia pracy
extern int godzina, minuta;        // Godzina i minuta symulacji

// Kasa
extern int kasa[];    // Liczba banknotów w kasie

// Funkcje
void sprawdz_godzine_startu();
void *symuluj_czas(void *arg);
void aktualizuj_kase(int *kasa_10, int *kasa_20, int *kasa_50, int zaplata_10, int zaplata_20, int zaplata_50);
void zaplac(int cena);

#endif // CONFIG_H
