#ifndef CONFIG_H
#define CONFIG_H

#include <semaphore.h>
#include <pthread.h>

#define MAX_FOTELI 3             // Maksymalna liczba foteli w poczekalni
#define MAX_CZEKANIE 10          // Maksymalna liczba klientów w poczekalni
#define MAX_KLIENTOW 20          // Maksymalna liczba klientów
#define MAX_FRYZJEROW 5          // Maksymalna liczba fryzjerów
#define LICZBA_MIEJSC_W_POCZEKALNI 5  // Maksymalna liczba miejsc w poczekalni
#define PIENIADZE {10, 20, 50}       // Dostępne nominały
#define CZAS_PRACY 30                // Czas pracy salonu w sekundach

// Deklaracje zmiennych globalnych
extern sem_t fotele, klient, service_done;  // Semafory
extern pthread_mutex_t kasa_mutex, mutex_poczekalnia; // Mutexy
extern pthread_cond_t cond_fryzjer, cond_klient; // Zmienne warunkowe
extern int wolne_fotele, klienci_w_poczekalni; // Zmienne do synchronizacji
extern int kasa[]; // Tablica przechowująca stan kasy
extern int zarobki; // Zarobki fryzjera

#endif // CONFIG_H
