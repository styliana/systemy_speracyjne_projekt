#include "config.h"

// Zmienne globalne
sem_t fotele, klient, service_done;  // Semafory
pthread_mutex_t mutex, kasa_mutex, mutex_poczekalnia; // Mutexy
pthread_cond_t cond_fryzjer, cond_klient; // Zmienne warunkowe
int wolne_fotele = 0, praca_trwa = 0, klienci_w_poczekalni = 0; // Zmienne do synchronizacji
int kasa[] = {10, 20, 50}; // Initialize the array with available denominations
int zarobki = 0; // Zarobki fryzjera
