#include "config.h"
#include <errno.h>

void odbierz_komunikat(struct komunikat* kom, long odbiorca)
{
    if ((msgrcv(salon, (struct msgbuf *)kom, sizeof(struct komunikat) - sizeof(long), odbiorca, 0)) == -1)
    {
        perror("Nie odało się odczytać komunikatu z kolejki salon.\n");
        exit(EXIT_FAILURE);
    }
}

void wyslij_komunikat(struct komunikat* kom)
{
    if ((msgsnd(salon, (struct msgbuf *)kom, sizeof(struct komunikat) - sizeof(long), 0)) == -1)
    {
        perror("Nie udało się dodać komunikatu do kolejki salon.\n");
        exit(EXIT_FAILURE);
    }
}

int suma_banknoty(int *banknoty)
{
    int suma = 0;
    suma += *banknoty * 10;
    suma += *(++banknoty) * 20;
    suma += *(++banknoty) * 50;
    return suma;
}

void dodaj_do_kasy(int *banknoty, int *kasa)
{
    pthread_mutex_lock(&kasa_mutex);
    for (int i = 0;  i < NOMINALY; i++) {
        *(kasa++) += *(banknoty++);
    }
    pthread_mutex_unlock(&kasa_mutex);
}

void sprawdz_godzine_startu() {
    int godzina_startu, minuta_startu;
    char czas_startu[6];  // Bufor na format hh:mm

    while (1) {
        printf("Podaj godzinę rozpoczęcia symulacji w formacie hh:mm (od 8:00 do 16:00): ");
        
        if (scanf("%5s", czas_startu) != 1) {
            while (getchar() != '\n');
            printf("\033[0;31m[ERROR]: Wprowadzono nieprawidłową godzinę. Spróbuj ponownie.\033[0m\n");
            continue;
        }

        if (sscanf(czas_startu, "%d:%d", &godzina_startu, &minuta_startu) != 2) {
            printf("\033[0;31m[ERROR]: Wprowadzono nieprawidłowy format godziny. Spróbuj ponownie.\033[0m\n");
            continue;
        }

        if (godzina_startu < 8 || godzina_startu >= 17) {
            // TODO:
            printf("\033[0;31m[ERROR]: Godzina %d:%02d jest poza godzinami pracy salonu (8:00 - 17:00).\033[0m\n", godzina_startu, minuta_startu);
            continue;
        }

        if (minuta_startu < 0 || minuta_startu >= 60) {
            printf("\033[0;31m[ERROR]: Minuty %02d są niepoprawne. Spróbuj ponownie.\033[0m\n", minuta_startu);
            continue;
        }

        start_hour = godzina_startu;
        break;
    }
}