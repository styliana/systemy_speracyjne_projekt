#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include "config.h"
#include "utils/pamiec_dzielona.h"
#include "utils/kolejka_komunikatow.h"
#include "utils/semafory.h"

void sprawdz_limit_procesow();
void tworz_klientow();
void tworz_fryzjerow();
void koniec(int s);
void szybki_koniec(int s);
void czekaj_na_procesy(int n);
void zakoncz_symulacje_czasu();
void zwolnij_zasoby();
void print_stan_kasy();
void *symuluj_czas(void *arg);
void sprawdz_godzine_startu(int* godzina, int* minuta);
void wyslij_s1();
void wyslij_s2();