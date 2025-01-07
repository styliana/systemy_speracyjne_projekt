#include "config.h"
#include "kierownik.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Funkcja do zamykania salonu
void zamknij_salon() {
    praca_trwa = 0;  // Ustawiamy flagę, aby zakończyć pracę salonu
    kill(0, SIGUSR1);  // Wysyła sygnał do wszystkich procesów
}

// Funkcja do ewakuacji
void ewakuacja() {
    kill(0, SIGUSR2);  // Wysyła sygnał do wszystkich procesów
}