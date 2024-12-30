#include "config.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

extern int kasa[];  // Deklaracja zmiennej globalnej kasa z config.c

// Poniższy kod nie ulega zmianie, ale możesz go dostosować do swoich wymagań
void *fryzjer_praca(void *arg) {
    int id = (long)arg;  // ID fryzjera

    while (praca_trwa) {
        pthread_mutex_lock(&mutex_poczekalnia);

        // Czekaj, aż będzie klient do obsługi
        while (klienci_w_poczekalni == 0 && praca_trwa) {
            printf("\033[0;34m[FRYZJER %d]: Czekam na klienta.\033[0m\n", id);
            pthread_cond_wait(&cond_fryzjer, &mutex_poczekalnia);  // Czekaj na klientów
        }

        if (!praca_trwa) {
            pthread_mutex_unlock(&mutex_poczekalnia);
            break;  // Koniec pracy fryzjera
        }

        // Obsługuje klienta
        klienci_w_poczekalni--;
        printf("\033[0;34m[FRYZJER %d]: Obsługuję klienta. Liczba oczekujących: %d.\033[0m\n", id, klienci_w_poczekalni);

        pthread_mutex_unlock(&mutex_poczekalnia);  // Zwolnij mutex

        // Losowa cena strzyżenia w pełnych dziesiątkach (od 30 do 100 zł)
        int cena_uslugi = (rand() % 8 + 3) * 10;

        // Pobranie pieniędzy od klienta przed strzyżeniem
        printf("\033[0;34m[FRYZJER %d]: Proszę o zapłatę %d zł za strzyżenie.\033[0m\n", id, cena_uslugi);
        zaplac(cena_uslugi);  // Klient płaci przed strzyżeniem

        // Symulacja strzyżenia
        sleep(2);  // Czas obsługi

        // Zakończenie obsługi klienta
        printf("\033[0;34m[FRYZJER %d]: Zakończyłem strzyżenie klienta.\033[0m\n", id);

        sem_post(&service_done);  // Zakończenie usługi dla klienta
    }

    return NULL;
}