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

void sprawdz_godzine_startu(int godzina_startu) {
    if (godzina_startu < 8 || godzina_startu >= 17) {
        printf("\033[0;31m[ERROR]: Godzina %d jest poza godzinami pracy salonu (8:00 - 17:00).\033[0m\n", godzina_startu);
        exit(0);
    } else {
        printf("\033[0;36m[INFO]: Symulacja rozpocznie się o godzinie %d:00.\033[0m\n", godzina_startu);
    }
}

void zaplac_fryzjerowi(int kwota) {
    pthread_mutex_lock(&kasa_mutex);

    while ((kasa_10zl * 10 + kasa_20zl * 20) < kwota) {
        printf("[INFO]: Brak wystarczających środków w kasie. Fryzjer czeka.\n");
        pthread_cond_wait(&cond_fryzjer, &kasa_mutex);  // Czekamy, aż kasa się uzupełni
    }

    if (kwota == 50) {
        kasa_10zl -= 5;  // 5 dziesięciozłotówek
    } else if (kwota == 100) {
        kasa_20zl -= 5;  // 5 dwudziestozłotówek
    }
    
    printf("[INFO]: Fryzjer otrzymał zapłatę za usługę: %d zł\n", kwota);

    pthread_mutex_unlock(&kasa_mutex);
}

void *symuluj_czas(void *arg) {
    int godzina_startu = *(int *)arg;
    godzina = godzina_startu;
    minuta = 0;

    while (praca_trwa) {
        usleep(1000000);  // Symulacja upływu 1 sekundy

        minuta++;
        if (minuta == 60) {
            minuta = 0;
            godzina++;
            if (godzina == 17) {
                printf("\033[0;36m[INFO]: Minął czas pracy salonu, zamykamy salon.\033[0m\n");
                praca_trwa = 0;
                break;
            }
        }

        if (minuta % 20 == 0) {  // Co 20 minut wyświetlamy godzinę
            printf("\033[0;36m[INFO]: Aktualna godzina: %02d:%02d\033[0m\n", godzina, minuta);
        }
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    sem_init(&fotele, 0, MAX_FOTELI);
    sem_init(&service_done, 0, 0);
    pthread_mutex_init(&mutex_poczekalnia, NULL);
    pthread_mutex_init(&kasa_mutex, NULL);

    printf("\033[0;33mPodaj godzinę rozpoczęcia symulacji (od 8 do 16): \033[0m");
    scanf("%d", &start_hour);
    sprawdz_godzine_startu(start_hour);

    pthread_t symulacja_czasu;
    pthread_create(&symulacja_czasu, NULL, symuluj_czas, (void *)&start_hour);

    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_create(&fryzjerzy[i], NULL, fryzjer_praca, (void *)(long)i);
    }

    for (int i = 0; i < MAX_KLIENTOW; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&klienci[i], NULL, klient_praca, (void *)id);
// Losowy czas oczekiwania przed przyjściem kolejnego klienta (od 1 do 3 sekund)
        sleep( 1);
    }

    for (int i = 0; i < MAX_KLIENTOW; i++) {
        pthread_join(klienci[i], NULL);  // Czekamy na zakończenie wszystkich klientów
    }

    // Czekamy na zakończenie pracy fryzjerów
    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    // Czekamy na zakończenie symulacji czasu
    pthread_join(symulacja_czasu, NULL);

    sem_destroy(&fotele);
    sem_destroy(&service_done);
    pthread_mutex_destroy(&mutex_poczekalnia);
    pthread_mutex_destroy(&kasa_mutex);

    return 0;
}
