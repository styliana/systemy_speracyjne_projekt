#include "config.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

extern int kasa[];  // Deklaracja zmiennej globalnej kasa z config.c

void zaplac(int cena) {
    int reszta = cena;  // Inicjalizacja zmiennej 'reszta' jako cena do zapłaty
    int klient_dal_50 = 0, klient_dal_20 = 0, klient_dal_10 = 0;  // Liczniki dla każdego nominału, którymi klient płaci

    // Losowanie nominalów, którymi klient płaci
    while (reszta > 0) {  // Dopóki klient nie zapłaci pełnej kwoty
        int nominal = rand() % 3; // Losowanie nominalu (0 - 10 zł, 1 - 20 zł, 2 - 50 zł)
        
        // Sprawdzenie, czy klient ma odpowiednią ilość banknotów i czy reszta jest wystarczająca
        if (nominal == 0 && kasa[0] > 0 && reszta >= 10) {
            kasa[0]--;  // Zmniejsz liczbę 10zł w kasie
            reszta -= 10;  // Zmniejsz resztę do zapłaty
            klient_dal_10++;  // Zwiększ licznik 10zł
        } else if (nominal == 1 && kasa[1] > 0 && reszta >= 20) {
            kasa[1]--;  // Zmniejsz liczbę 20zł w kasie
            reszta -= 20;  // Zmniejsz resztę do zapłaty
            klient_dal_20++;  // Zwiększ licznik 20zł
        } else if (nominal == 2 && kasa[2] > 0 && reszta >= 50) {
            kasa[2]--;  // Zmniejsz liczbę 50zł w kasie
            reszta -= 50;  // Zmniejsz resztę do zapłaty
            klient_dal_50++;  // Zwiększ licznik 50zł
        }
    }

    // Komunikaty o zapłacie
    printf("\033[0;32m[KLient]: Zapłacono %d zł w nominałach:\033[0m\n", cena);
    if (klient_dal_50 > 0) printf("\033[0;32m[KLient]: %d x 50zł\033[0m\n", klient_dal_50);
    if (klient_dal_20 > 0) printf("\033[0;32m[KLient]: %d x 20zł\033[0m\n", klient_dal_20);
    if (klient_dal_10 > 0) printf("\033[0;32m[KLient]: %d x 10zł\033[0m\n", klient_dal_10);

    // Obliczanie reszty
    printf("\033[0;34m[Fryzjer]: Obliczam resztę...\033[0m\n");

    while (reszta > 0) {  // Dopóki reszta nie zostanie zapłacona
        // Sprawdzenie, czy jest wystarczająca ilość banknotów w kasie do wydania reszty
        if (kasa[2] > 0 && reszta >= 50) {
            kasa[2]--;  // Zmniejsz liczbę 50zł w kasie
            reszta -= 50;  // Zmniejsz resztę
            printf("\033[0;34m[Fryzjer]: Wydano resztę: 50zł.\033[0m\n");
        } else if (kasa[1] > 0 && reszta >= 20) {
            kasa[1]--;  // Zmniejsz liczbę 20zł w kasie
            reszta -= 20;  // Zmniejsz resztę
            printf("\033[0;34m[Fryzjer]: Wydano resztę: 20zł.\033[0m\n");
        } else if (kasa[0] > 0 && reszta >= 10) {
            kasa[0]--;  // Zmniejsz liczbę 10zł w kasie
            reszta -= 10;  // Zmniejsz resztę
            printf("\033[0;34m[Fryzjer]: Wydano resztę: 10zł.\033[0m\n");
        }

        // Jeśli kasa nie ma odpowiednich banknotów, czekaj na dopłatę
        if (reszta > 0) {
            printf("\033[0;31m[INFO]: Brak wystarczających środków na resztę.\033[0m\n");
            sleep(1);  // Czekaj na nowe wpłaty
        }
    }
}


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
