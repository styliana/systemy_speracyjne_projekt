#include "fryzjer.h"
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
}
