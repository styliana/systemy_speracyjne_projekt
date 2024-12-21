#include "kierownik.h"
<<<<<<< HEAD
#include <stdio.h>
#include <signal.h>

void zamknij_salon() {
    printf("Kierownik zamyka salon.\n");
    raise(SIGINT);
=======
#include "utils.h"

void handle_signal(int signal) {
    if (signal == SIGUSR1) {
        stop_single_fryzjer(); // Zatrzymanie jednego fryzjera
    } else if (signal == SIGUSR2) {
        evacuate_salon(); // Natychmiastowe opróżnienie salonu
    }
>>>>>>> 9bae1b699490254930a05064bc503b7665153041
}
