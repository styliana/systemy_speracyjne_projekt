#include "config.h"
#include "klient.h"
#include "fryzjer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int start_hour;

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

void *symuluj_czas(void *arg) {
    int godzina_startu = *(int *)arg;
    int godzina = godzina_startu;
    int minuta = 0;

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

        if (minuta % 20 == 0) {
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

    printf("\033[0;33mPodaj godzinę rozpoczęcia symulacji (od 8 do 16): \033[0m");
    scanf("%d", &start_hour);
    sprawdz_godzine_startu(start_hour);

    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_create(&fryzjerzy[i], NULL, fryzjer_praca, (void *)(long)i);
    }

    for (int i = 0; i < MAX_KLIENTOW; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&klienci[i], NULL, klient_praca, (void *)id);
        sleep(1);
    }

    for (int i = 0; i < MAX_KLIENTOW; i++) {
        pthread_join(klienci[i], NULL);
    }

    for (int i = 0; i < MAX_FRYZJEROW; i++) {
        pthread_join(fryzjerzy[i], NULL);
    }

    sem_destroy(&fotele);
    sem_destroy(&service_done);
    pthread_mutex_destroy(&mutex_poczekalnia);

    return 0;
}