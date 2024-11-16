#include "fryzjer.h"
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
}
