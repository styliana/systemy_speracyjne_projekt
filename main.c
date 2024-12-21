#include <stdio.h> 
#include "fryzjer.h"
#include "klient.h"
#include "kierownik.h"

#define F 3
#define K 5

int main() {
    Fryzjer fryzjerzy[F];
    Klient klienci[K];

    for (int i = 0; i < F; i++) {
        fryzjerzy[i].id = i;
        fryzjerzy[i].status = 0;
        fryzjerzy[i].kasa = 0;
        printf("Fryzjer %d gotowy do pracy.\n", i);
    }

    for (int i = 0; i < K; i++) {
        klient_generuj(&klienci[i], i);
    }

    return 0;
}
