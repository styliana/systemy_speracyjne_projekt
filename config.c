#include "config.h"

sem_t fotele, klient, service_done;  // Semafory
pthread_mutex_t kasa_mutex, mutex_poczekalnia; // Mutexy
pthread_cond_t cond_fryzjer, cond_klient; // Zmienne warunkowe
int wolne_fotele = MAX_FOTELI;  // Inicjalizacja liczby wolnych foteli
int klienci_w_poczekalni = 0;   // Inicjalizacja liczby klientów w poczekalni
int kasa[5] = {10, 10, 0, 0, 0};  // 10 banknotów 10 zł, 10 banknotów 20 zł, 0 banknotów 50 zł, itd.
int zarobki = 0; // Inicjalizacja zarobków fryzjera

// Definicja zmiennej praca_trwa
int praca_trwa = 1; // Salon jest otwarty (praca trwa)

void aktualizuj_kase(int *kasa_10, int *kasa_20, int *kasa_50, int zaplata_10, int zaplata_20, int zaplata_50) {
    *kasa_10 += zaplata_10;
    *kasa_20 += zaplata_20;
    *kasa_50 += zaplata_50;

    int suma = (*kasa_10 * 10) + (*kasa_20 * 20) + (*kasa_50 * 50);
    printf("\033[0;36m[INFO]: Stan kasy: %d zł (10 zł: %d, 20 zł: %d, 50 zł: %d)\033[0m\n", suma, *kasa_10, *kasa_20, *kasa_50);
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

extern int kasa[5];  // Dostęp do zmiennej globalnej kasa
pthread_mutex_t kasa_mutex;  // Mutex dla operacji na kasie

void zaplac(int cena) {
    // Losowanie nadwyżki (0, 10, 20 lub 30 zł)
    int nadwyzka = (rand() % 4) * 10;  // Wartości 0, 10, 20, 30
    int kwota_do_zaplaty = cena + nadwyzka;  // Kwota, którą klient płaci (z nadwyżką)
    int reszta = kwota_do_zaplaty - cena;  // Reszta do wydania klientowi
    int klient_dal_50 = 0, klient_dal_20 = 0, klient_dal_10 = 0;  // Liczniki dla każdego nominału
    int suma_zaplacona = 0;  // Zmienna do przechowywania sumy zapłaconej przez klienta

    // Wybór płatności przez klienta
    while (kwota_do_zaplaty > 0) {  // Dopóki klient nie zapłaci pełnej kwoty
        int nominal = rand() % 3;  // Losowanie nominalu (0 - 10 zł, 1 - 20 zł, 2 - 50 zł)

        if (nominal == 0 && kasa[0] > 0 && kwota_do_zaplaty >= 10) {
            kasa[0]--;  // Zmniejsz liczbę 10zł w kasie
            kwota_do_zaplaty -= 10;  // Zmniejsz resztę do zapłaty
            klient_dal_10++;  // Zwiększ licznik 10zł
            suma_zaplacona += 10;  // Dodaj 10zł do zapłaconej kwoty
        } else if (nominal == 1 && kasa[1] > 0 && kwota_do_zaplaty >= 20) {
            kasa[1]--;  // Zmniejsz liczbę 20zł w kasie
            kwota_do_zaplaty -= 20;  // Zmniejsz resztę do zapłaty
            klient_dal_20++;  // Zwiększ licznik 20zł
            suma_zaplacona += 20;  // Dodaj 20zł do zapłaconej kwoty
        } else if (nominal == 2 && kasa[2] > 0 && kwota_do_zaplaty >= 50) {
            kasa[2]--;  // Zmniejsz liczbę 50zł w kasie
            kwota_do_zaplaty -= 50;  // Zmniejsz resztę do zapłaty
            klient_dal_50++;  // Zwiększ licznik 50zł
            suma_zaplacona += 50;  // Dodaj 50zł do zapłaconej kwoty
        }
    }

    // Komunikaty o zapłacie
    printf("\033[0;32m[KLIENT]: Zapłacono %d zł (w tym nadwyżka %d zł) w nominałach:\033[0m\n", suma_zaplacona, nadwyzka);
    if (klient_dal_50 > 0) printf("\033[0;32m[KLient]: %d x 50zł\033[0m\n", klient_dal_50);
    if (klient_dal_20 > 0) printf("\033[0;32m[KLient]: %d x 20zł\033[0m\n", klient_dal_20);
    if (klient_dal_10 > 0) printf("\033[0;32m[KLient]: %d x 10zł\033[0m\n", klient_dal_10);

    // Obliczanie reszty
    printf("\033[0;34m[Fryzjer]: Obliczam resztę...\033[0m\n");

    // Zmienna do przechowywania informacji o wydanej reszcie
    int wydana_reszta = 0;

    // Jeśli nadwyżka została zapłacona, wydaj resztę
    while (reszta > 0) {  // Dopóki reszta nie zostanie zapłacona
        // Sprawdzenie, czy jest wystarczająca ilość banknotów w kasie do wydania reszty
        if (kasa[2] > 0 && reszta >= 50) {
            kasa[2]--;  // Zmniejsz liczbę 50zł w kasie
            reszta -= 50;  // Zmniejsz resztę
            wydana_reszta += 50;  // Zwiększ wydaną resztę
            printf("\033[0;34m[Fryzjer]: Wydano resztę: 50zł.\033[0m\n");
        } else if (kasa[1] > 0 && reszta >= 20) {
            kasa[1]--;  // Zmniejsz liczbę 20zł w kasie
            reszta -= 20;  // Zmniejsz resztę
            wydana_reszta += 20;  // Zwiększ wydaną resztę
            printf("\033[0;34m[Fryzjer]: Wydano resztę: 20zł.\033[0m\n");
        } else if (kasa[0] > 0 && reszta >= 10) {
            kasa[0]--;  // Zmniejsz liczbę 10zł w kasie
            reszta -= 10;  // Zmniejsz resztę
            wydana_reszta += 10;  // Zwiększ wydaną resztę
            printf("\033[0;34m[Fryzjer]: Wydano resztę: 10zł.\033[0m\n");
        }

        // Jeśli kasa nie ma odpowiednich banknotów, czekaj na nowe wpłaty
        if (reszta > 0) {
            printf("\033[0;31m[INFO]: Brak wystarczających środków na resztę.\033[0m\n");
            sleep(1);  // Czekaj na nowe wpłaty
        }
    }

    // Wyświetlanie całkowitej wydanej reszty
    printf("\033[0;34m[Fryzjer]: Całkowita wydana reszta: %d zł.\033[0m\n", wydana_reszta);

    // Wywołanie funkcji do aktualizacji stanu kasy
    pthread_mutex_lock(&kasa_mutex);  // Zablokuj mutex na kasie
    kasa[0] += klient_dal_10;  // Zaktualizuj stan kasy
    kasa[1] += klient_dal_20;
    kasa[2] += klient_dal_50;
    pthread_mutex_unlock(&kasa_mutex);  // Zwolnij mutex

    // Wyświetlenie stanu kasy po zapłacie
    aktualizuj_kase(&kasa[0], &kasa[1], &kasa[2], klient_dal_10, klient_dal_20, klient_dal_50);
}
