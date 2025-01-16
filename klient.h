#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "config.h"
#include "utils/pamiec_dzielona.h"
#include "utils/kolejka_komunikatow.h"
#include "utils/semafory.h"

void sygnal_2(int sig);