#include "config.h"
#include "fryzjer.h"
#include "klient.h"
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define LICZBA_FRYZJEROW 3  // Zmniejszenie liczby fryzjerów dla testów
#define LICZBA_KLIENTOW 100  // Maksymalna liczba klientów (dla testów)

int main() {
    // Inicjalizacja mutexów
    pthread_mutex_init(&kasa_mutex, NULL);
    pthread_mutex_init(&mutex_poczekalnia, NULL);

    // Inicjalizacja semaforów
    sem_init(&fotele, 0, MAX_FOTELI);  // Semafor dla foteli
    sem_init(&klient, 0, 0);            // Semafor dla klientów
    sem_init(&service_done, 0, 0);      // Semafor do sygnalizowania zakończenia usługi

    pthread_t fryzjerzy[LICZBA_FRYZJEROW];
    pthread_t klienci[LICZBA_KLIENTOW];

    // Tworzenie wątków fryzjerów
    for (int i = 0; i < LICZBA_FRYZJEROW; i++) {
        if (pthread_create(&fryzjerzy[i], NULL, fryzjer_praca, (void *)(long)i) != 0) {
            perror("Błąd tworzenia wątku fryzjera");
            exit(1);
        }
    }

    // Losowe generowanie klientów w odstępach czasowych
    srand(time(NULL));  // Inicjalizacja generatora liczb losowych
    int klient_id = 0;

    while (klient_id < LICZBA_KLIENTOW) {
        // Tworzenie nowego klienta
        if (pthread_create(&klienci[klient_id], NULL, klient_praca, (void *)(long)klient_id) != 0) {
            perror("Błąd tworzenia wątku klienta");
            exit(1);
        }

        // Zwiększamy ID klienta
        klient_id++;

        // Czekamy losowy czas przed dodaniem kolejnego klienta (losowy czas między 1 a 5 sekund)
        sleep(rand() % 5 + 1);  // Sleep w przedziale 1-5 sekund
    }

    // Czekamy na zakończenie wątków fryzjerów
    for (int i = 0; i < LICZBA_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }
    // Czekamy na zakończenie wątków klientów
    for (int i = 0; i < klient_id; i++) {
        pthread_join(klienci[i], NULL);
    }

    // Czyszczenie zasobów
    sem_destroy(&fotele);
    sem_destroy(&klient);
    sem_destroy(&service_done);
    pthread_mutex_destroy(&kasa_mutex);
    pthread_mutex_destroy(&mutex_poczekalnia);

    return 0;
}
