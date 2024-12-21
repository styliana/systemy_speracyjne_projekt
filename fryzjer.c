#include "fryzjer.h"
<<<<<<< HEAD
#include <stdio.h>
#include <unistd.h>

void fryzjer_pracuje(Fryzjer *fryzjer) {
    while (1) {
        printf("Fryzjer %d czeka na klienta.\n", fryzjer->id);
        sleep(1); // Symulacja oczekiwania
        printf("Fryzjer %d obsługuje klienta.\n", fryzjer->id);
        sleep(2); // Symulacja strzyżenia
        printf("Fryzjer %d zakończył obsługę.\n", fryzjer->id);
    }
=======
#include "utils.h"

void *fryzjer_routine(void *arg) {
    while (1) {
        Klient *klient = dequeue_client(); // Pobierz klienta z poczekalni
        if (!klient) continue;

        zajmij_fotel(); // Zajęcie fotela
        pobierz_opłatę(klient); // Pobranie płatności
        wykonaj_usługę(klient); // Symulacja strzyżenia
        zwolnij_fotel(); // Zwolnienie fotela
        wydaj_resztę(klient); // Wydanie reszty
    }
    return NULL;
>>>>>>> 9bae1b699490254930a05064bc503b7665153041
}
