#include "kierownik.h"
#include <stdio.h>
#include <signal.h>

void zamknij_salon() {
    printf("Kierownik zamyka salon.\n");
    raise(SIGINT);
}
