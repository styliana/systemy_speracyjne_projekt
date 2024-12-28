#ifndef CONFIG_H
#define CONFIG_H

#include <semaphore.h>
#include <pthread.h>

#define MAX_FOTELI 2                  // Maksymalna liczba foteli w salonie
#define MAX_KLIENTOW 100              // Maksymalna liczba klientów
#define MAX_FRYZJEROW 5               // Maksymalna liczba fryzjerów
#define LICZBA_MIEJSC_W_POCZEKALNI 2  // Liczba miejsc w poczekalni

// Deklaracje zmiennych globalnych
extern sem_t fotele, klient, service_done;  // Semafory
extern pthread_mutex_t kasa_mutex, mutex_poczekalnia; // Mutexy
extern pthread_cond_t cond_fryzjer, cond_klient; // Zmienne warunkowe
extern int klienci_w_poczekalni;   // Liczba klientów w poczekalni
extern int zarobki;                // Zarobki fryzjera
extern int praca_trwa;             // Zmienna pracy salonu
extern int start_hour;             // Godzina rozpoczęcia pracy

#endif // CONFIG_H