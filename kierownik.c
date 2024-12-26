#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "config.h"

void zamknij_salon() {
    kill(0, SIGUSR1);
}

void ewakuacja() {
    kill(0, SIGUSR2);
}
