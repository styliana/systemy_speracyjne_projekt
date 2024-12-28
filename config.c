#include "config.h"

sem_t fotele, klient, service_done;  // Semafory
pthread_mutex_t kasa_mutex, mutex_poczekalnia; // Mutexy
pthread_cond_t cond_fryzjer, cond_klient; // Zmienne warunkowe
int wolne_fotele = MAX_FOTELI;  // Inicjalizacja liczby wolnych foteli
int klienci_w_poczekalni = 0;   // Inicjalizacja liczby klientów w poczekalni
int kasa[5] = {10, 10, 0, 0, 0};  // 10 banknotów 10 zł, 10 banknotów 20 zł, 0 banknotów 50 zł, itd.
int zarobki = 0; // Inicjalizacja zarobków fryzjera

// Definicja zmiennej praca_trwa
int praca_trwa = 1; // Salon jest otwarty (praca trwa)
