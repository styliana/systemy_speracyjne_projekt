#include "klient.h"
<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void klient_generuj(Klient *klient, int id) {
    klient->id = id;
    klient->kwota = (rand() % 3 + 1) * 10; // 10, 20, lub 50 zł
    klient->fotel = -1;
    printf("Klient %d przychodzi z kwotą %d zł.\n", klient->id, klient->kwota);
=======
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
>>>>>>> 9bae1b699490254930a05064bc503b7665153041
}
