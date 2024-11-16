#include "kierownik.h"
#include "utils.h"

void handle_signal(int signal) {
    if (signal == SIGUSR1) {
        stop_single_fryzjer(); // Zatrzymanie jednego fryzjera
    } else if (signal == SIGUSR2) {
        evacuate_salon(); // Natychmiastowe opróżnienie salonu
    }
}
