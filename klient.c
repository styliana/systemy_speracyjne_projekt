#include "klient.h"
#include "utils.h"

void *klient_routine(void *arg) {
    while (1) {
        if (try_enter_salon()) {
            enqueue_client(); // Zajęcie miejsca w poczekalni
            wait_for_fryzjer(); // Oczekiwanie na obsługę
        } else {
            zarabiaj_pieniądze(); // Brak miejsca -> zarabianie pieniędzy
        }
    }
    return NULL;
}
