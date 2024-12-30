#include "config.h"
#include "klient.h"
#include "fryzjer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int kasa_10zl = 10;  // 10 dziesięciozłotówek
int kasa_20zl = 10;  // 10 dwudziestozłotówek
int start_hour = 8;  // Domyślna godzina rozpoczęcia
int godzina = 8;     // Początkowa godzina
int minuta = 0;      // Początkowa minuta

pthread_t klienci[MAX_KLIENTOW];
pthread_t fryzjerzy[MAX_FRYZJEROW];

int salon_otwarty = 1;  // Flaga, która kontroluje, czy salon jest otwarty

pthread_mutex_t mutex_poczekalnia;
pthread_mutex_t kasa_mutex;
sem_t fotel, service_done;

void *symuluj_czas(void *arg) {
    int godzina_startu = *(int *)arg;
    godzina = godzina_startu;  // Inicjalizacja godziny na wartość podaną przez użytkownika
    minuta = 0;  // Zaczynamy od początku (00 minut)

    while (salon_otwarty) {
        usleep(1000000);  // Symulacja upływu 1 sekundy

        minuta++;
        if (minuta == 60) {
            minuta = 0;
            godzina++;
            if (godzina == 17) {
                printf("\033[0;36m[INFO]: Minął czas pracy salonu, zamknięcie salonu.\033[0m\n");

                // Wyświetlenie końcowego stanu kasy po dniu
                int suma = kasa[0] * 10 + kasa[1] * 20 + kasa[2] * 50;
                printf("\033[0;36m[INFO]: Ostateczna suma w kasie: %d zł.\033[0m\n", suma);

                salon_otwarty = 0;  // Zamykamy salon
                break;
            }
        }

        // Co 20 minut wyświetlamy godzinę oraz stan kasy
        if (minuta % 20 == 0 || (godzina == godzina_startu && minuta == 0)) {
            printf("\033[0;36m[INFO]: Aktualna godzina: %02d:%02d\033[0m\n", godzina, minuta);
            int suma = kasa[0] * 10 + kasa[1] * 20 + kasa[2] * 50;
            printf("\033[0;36m[INFO]: Stan kasy: %d zł.\033[0m\n", suma);  // Wyświetlenie stanu kasy
        }
    }

    return NULL;
}



int main() {
    srand(time(NULL));

    // Inicjalizujemy semafory i mutexy
        // Inicjalizacja semaforów
    if (sem_init(&fotele, 0, MAX_FOTELI) != 0) {
        perror("Nie udalo sie stworzyc semafora fotel");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&service_done, 0, 0) != 0) {
        perror("Nie udalo sie stworzyc semafora service_done");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja mutexów
    if (pthread_mutex_init(&mutex_poczekalnia, NULL) != 0) {
        perror("Nie udalo sie stworzyc mutexa mutex_poczekalnia");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&kasa_mutex, NULL) != 0) {
        perror("Nie udalo sie stworzyc mutexa kasa_mutex");
        exit(EXIT_FAILURE);
    }

    // Sprawdzamy godzinę rozpoczęcia symulacji
    sprawdz_godzine_startu();

    // Tworzymy wątek do symulacji czasu
    pthread_t symulacja_czasu;
    pthread_create(&symulacja_czasu, NULL, symuluj_czas, (void *)&start_hour);

    // Tworzymy wątki fryzjerów
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_create(&fryzjerzy[i], NULL, fryzjer_praca, (void *)(long)i);
    }

    // Tworzymy wątki klientów, sprawdzając, czy salon jest otwarty
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        if (!salon_otwarty) {
            break;  // Zatrzymujemy tworzenie nowych klientów, jeśli salon jest już zamknięty
        }
        
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&klienci[i], NULL, klient_praca, (void *)id);

        // Losowy czas oczekiwania przed przyjściem kolejnego klienta (od 1 do 3 sekund)
        sleep(1);
    }

    // Czekamy na zakończenie wszystkich klientów
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        pthread_join(klienci[i], NULL);  // Czekamy na zakończenie wszystkich klientów
    }

    // Czekamy na zakończenie pracy fryzjerów
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    // Czekamy na zakończenie symulacji czasu
    pthread_join(symulacja_czasu, NULL);

    // Czyszczenie zasobów
    sem_destroy(&fotel);
    sem_destroy(&service_done);
    pthread_mutex_destroy(&mutex_poczekalnia);
    pthread_mutex_destroy(&kasa_mutex);

    return 0;
}
