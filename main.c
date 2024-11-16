#include "fryzjer.h"
#include "klient.h"
#include "kierownik.h"
#include "utils.h"

int main() {
    initialize_salon(); // Inicjalizacja kas, poczekalni, mutexów
    start_threads();    // Uruchomienie wątków fryzjerów, klientów, kierownika
    cleanup_resources(); // Usunięcie pamięci dzielonej, semaforów itp.
    return 0;
}
