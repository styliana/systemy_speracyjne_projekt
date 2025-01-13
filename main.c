#include "config.h"
#include "klient.h"
#include "kierownik.h"
#include "fryzjer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

int start_hour = 8;  // Domyślna godzina rozpoczęcia
int godzina = 8;     // Początkowa godzina
int minuta = 0;      // Początkowa minuta

pthread_t klienci[MAX_KLIENTOW];
pthread_t fryzjerzy[MAX_FRYZJEROW];
pthread_t symulacja_czasu;

sem_t fotele, poczekalnia; // Semafory
int salon; // Kolejka komunikatów

int salon_otwarty = 1;  // Flaga, która kontroluje, czy salon jest otwarty

int kasa[NOMINALY] = {10, 10, 0};  // 10 banknotów 10 zł, 10 banknotów 20 zł, 0 banknotów 50 zł
pthread_mutex_t kasa_mutex; // Mutex dostępu do kasy

void print_stan_kasy() {
    pthread_mutex_lock(&kasa_mutex);
    printf("\033[0;36m[INFO]: Stan kasy: %d zł.\033[0m\n", suma_banknoty(kasa));
    pthread_mutex_unlock(&kasa_mutex);
}

void *symuluj_czas(void *arg) {
    int godzina_startu = *(int *)arg;
    godzina = godzina_startu;  // Inicjalizacja godziny na wartość podaną przez użytkownika
    minuta = 0;  // Zaczynamy od początku (00 minut)

    while (1) {
        sleep(1);

        minuta++;
        if (minuta == 60) {
            minuta = 0;
            godzina++;
            // Zamknięcie
            if (godzina == 17) {
                printf("\033[0;36m[INFO]: Minął czas pracy salonu, zamknięcie salonu.\033[0m\n");

                // Wyświetlenie końcowego stanu kasy po dniu
                print_stan_kasy();

                salon_otwarty = 0;  // Zamykamy salon
            }
            // Otwarcie
            else if (godzina == 8) {
                printf("\033[0;36m[INFO]: Otwarcie salonu.\033[0m\n");
                print_stan_kasy();

                salon_otwarty = 1;  // Otwieramy salon
            }

            else if (godzina == 24) {
                godzina = 0;
            }

            //TODO: komunikat ?
            else if (godzina == 16 && minuta == 30) {

            }
        }

        // Co 5 minut wyświetlamy godzinę
        if (minuta % 5 == 0) {
            printf("\033[0;36m[INFO]: Aktualna godzina: %02d:%02d\033[0m\n", godzina, minuta);
        } else if (minuta % 20 == 0) {
        // Co 20 minut wyświetlamy godzinę oraz stan kasy
            printf("\033[0;36m[INFO]: Aktualna godzina: %02d:%02d\033[0m\n", godzina, minuta);
            print_stan_kasy();  // Wyświetlenie stanu kasy
        }
    }

    return NULL;
}


void sprzatanie(int sig);

int main() {
    srand(time(NULL));

    if (signal(SIGINT, sprzatanie) == SIG_ERR)
    {
        perror("Blad obslugi sygnalu");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja kolejek komunikatów
    key_t klucz = ftok("/tmp/",'s');
    if ((salon = msgget(klucz, IPC_CREAT | 0600)) == -1)
    {
        perror("Nie udalo sie stworzyc kolejki komunikatów salon");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja semaforów
    if (sem_init(&fotele, 0, MAX_FOTELI) != 0) {  // Semafor prywatny dla wątków
        perror("Nie udalo sie stworzyc semafora fotele");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&poczekalnia, 0, MAX_POCZEKALNIA) != 0) {  // Semafor prywatny dla wątków
        perror("Nie udalo sie stworzyc semafora poczekalnia");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja mutexów
    if (pthread_mutex_init(&kasa_mutex, NULL) != 0) {  // Mutex domyślnie prywatny dla wątków
        perror("Nie udalo sie stworzyc mutexa kasa_mutex");
        exit(EXIT_FAILURE);
    }

    // Sprawdzamy godzinę rozpoczęcia symulacji
    sprawdz_godzine_startu();

    // Tworzymy wątek do symulacji czasu
    pthread_create(&symulacja_czasu, NULL, symuluj_czas, (void *)&start_hour);

    // Tworzymy wątki fryzjerów
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_create(&fryzjerzy[i], NULL, fryzjer_praca, (void *)(long)i);
    }

    // Tworzymy wątki klientów, sprawdzając, czy salon jest otwarty
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        pthread_create(&klienci[i], NULL, klient_praca, (void *)(long)i);

        // Losowy czas oczekiwania przed przyjściem kolejnego klienta (od 4 do 10 sekund)
        int czas_oczekiwania = rand() % 7 + 4;
        sleep(czas_oczekiwania);
    }

    // Możemy wysyłać sygnały lub zakończyć program
    while (1) {
        // scanf();
    }
    // Czekamy na zakończenie symulacji czasu
    pthread_join(symulacja_czasu, NULL);

    // Czekamy na zakończenie wszystkich klientów
    for (int i = 0; i < MAX_KLIENTOW; i++) {
        pthread_join(klienci[i], NULL);  // Czekamy na zakończenie wszystkich klientów
    }

    // Czekamy na zakończenie pracy fryzjerów
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    sprzatanie(0);
    return 0;
}

void sprzatanie(int sig) {

    // Czyszczenie zasobów
    msgctl(salon, IPC_RMID, NULL);
    sem_destroy(&fotele);
    sem_destroy(&poczekalnia);
    pthread_mutex_destroy(&kasa_mutex);

    printf("\033[0;36m[INFO]: Zasoby zwolnione\033[0m\n");
    exit(0);
}