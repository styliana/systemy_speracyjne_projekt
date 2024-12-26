#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "fryzjer.h"
#include "klient.h"
#include "kierownik.h"
#include "config.h"

// Deklaracje semaforów globalnych
sem_t poczekalnia;
sem_t fotel[MAX_FOTELI];

// Funkcja do inicjalizacji semaforów
void init_semaphores() {
    if (sem_init(&poczekalnia, 0, MAX_KLIENTOW) == -1) {
        perror("Błąd inicjalizacji semafora poczekalnia");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_FOTELI; i++) {
        if (sem_init(&fotel[i], 0, 1) == -1) {
            perror("Błąd inicjalizacji semafora fotel");
            exit(EXIT_FAILURE);
        }
    }
}

// Funkcja do tworzenia wątków z obsługą błędów
void create_threads(pthread_t *threads, int num_threads, void* (*start_routine)(void*), void *args[]) {
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, start_routine, args[i]) != 0) {
            perror("Błąd tworzenia wątku");
            exit(EXIT_FAILURE);
        }
    }
}

// Główna funkcja programu
int main() {
    srand(time(NULL)); // Inicjalizacja generatora liczb losowych

    // Inicjalizacja semaforów
    init_semaphores();

    // Tablice wątków i argumentów
    pthread_t fryzjerzy[MAX_FOTELI], klienci[MAX_KLIENTOW];
    int fryzjer_id[MAX_FOTELI];
    Klient klient[MAX_KLIENTOW];
    void *klient_args[MAX_KLIENTOW], *fryzjer_args[MAX_FOTELI];

    // Przygotowanie argumentów dla fryzjerów i klientów
    for (int i = 0; i < MAX_FOTELI; i++) {
        fryzjer_id[i] = i + 1;
        fryzjer_args[i] = &fryzjer_id[i];
    }
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        klient[i].id = i + 1;
        klient_args[i] = &klient[i];
    }

    // Tworzenie wątków dla fryzjerów i klientów
    create_threads(fryzjerzy, MAX_FOTELI, fryzjer_praca, fryzjer_args);
    create_threads(klienci, MAX_KLIENTOW, klient_praca, klient_args);

    // Dołączanie wątków klientów
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        pthread_join(klienci[i], NULL);
    }

    // Dołączanie wątków fryzjerów
    for (int i = 0; i < MAX_FOTELI; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    // Zwalnianie zasobów
    sem_destroy(&poczekalnia);
    for (int i = 0; i < MAX_FOTELI; i++) {
        sem_destroy(&fotel[i]);
    }

    return 0;
}
