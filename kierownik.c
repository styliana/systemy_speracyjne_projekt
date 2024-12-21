#include "kierownik.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Kierownik: Zakończenie pracy fryzjera.\n");
        // Wysłanie sygnału do fryzjera, by zakończył pracę
    } else if (sig == SIGUSR2) {
        printf("Kierownik: Klienci opuszczają salon.\n");
        // Wysłanie sygnału do wszystkich klientów, by opuścili salon
    }
}
