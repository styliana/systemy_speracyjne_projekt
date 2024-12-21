#include "klient.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void klient_generuj(Klient *klient, int id) {
    klient->id = id;
    klient->kwota = (rand() % 3 + 1) * 10; // 10, 20, lub 50 zł
    klient->fotel = -1;
    printf("Klient %d przychodzi z kwotą %d zł.\n", klient->id, klient->kwota);
}
