#include "fryzjer.h"

long ja;
int fotele, kasa; // Semafory
int kolejka; // Kolejka komunikatów
int pamiec_id; // Pamięć dzielona
int *pamiec;
int zajmuje_fotel = 0, zajmuje_kase = 0;

int  main() {
    ja = getpid();
    if (signal(SIGHUP, sygnal_1) == SIG_ERR)
    {
        perror("Blad obslugi sygnalu");
        exit(EXIT_FAILURE);
    }

    long klient_id;
    key_t klucz;

    struct komunikat kom;
    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_KOLEJKA);
    kolejka = utworz_kolejke_komunikatow(klucz);

    klucz = ftok(KLUCZ_PATH, KLUCZ_CHAR_SEM_FOTELE);
    fotele = utworz_semafor(klucz);

    klucz = ftok(KLUCZ_PATH,KLUCZ_CHAR_SEM_KASA);
    kasa = utworz_semafor(klucz);

    klucz = ftok(KLUCZ_PATH,KLUCZ_CHAR_SEM_KASA);
    pamiec_id = utworz_pamiec_dzielona(klucz);
    pamiec = dolacz_pamiec_dzielona(pamiec_id);

    while (1) {
        // Czeka na klienta
        odbierz_komunikat(kolejka, &kom, KOMUNIKAT_POCZEKALNIA);

        // Czeka z klientem na fotel
        klient_id = kom.podpis;

        sem_p(fotele, 1); // Zajmujemy fotel
        zajmuje_fotel = 1;

        // Losowa cena strzyżenia w pełnych dziesiątkach (od 30 do 100 zł)
        int cena_uslugi = (rand() % 8 + 3) * 10;

        // Pobranie pieniędzy od klienta przed strzyżeniem
        printf("\033[0;34m[FRYZJER %ld]: Proszę klienta %ld o zapłatę %d zł za strzyżenie.\033[0m\n", ja, klient_id, cena_uslugi);
        // Podaje cenę klientowi
        kom.mtype = klient_id;
        kom.podpis = ja;
        kom.msg[0] = cena_uslugi;
        wyslij_komunikat(kolejka, &kom);

        // Odbiera zapłatę i wkłada do wspólnej kasy
        odbierz_komunikat(kolejka, &kom, ja);
        
        sem_p(kasa, 1);
        zajmuje_kase = 1;
        pamiec[0] += kom.msg[0];
        pamiec[1] += kom.msg[1];
        pamiec[2] += kom.msg[2];
        sem_v(kasa, 1);
        zajmuje_kase = 0;

        int zaplacono = suma_banknoty(kom.msg);
        int reszta =  zaplacono - cena_uslugi;
        printf("\033[0;34m[FRYZJER %ld]: Otrzymałem od klienta %ld o zapłatę %d zł za strzyżenie, nadwyzka: %d.\033[0m\n", ja, klient_id, zaplacono, reszta);

        // Obsługuje klienta
        printf("\033[0;34m[FRYZJER %ld]: Obsługuję klienta.\033[0m\n", ja);
        sleep(CZAS_USLUGI);  // Czas obsługi

        // Zakończenie obsługi klienta
        printf("\033[0;34m[FRYZJER %ld]: Zakończyłem strzyżenie klienta, zwalniam fotel.\033[0m\n", ja);
        sem_v(fotele, 1);
        zajmuje_fotel = 0;

        // Przygotuj komunikat z resztą dla klienta
        kom.mtype = klient_id;
        kom.podpis = ja;
        kom.msg[0] = 0;
        kom.msg[1] = 0;
        kom.msg[2] = 0;
        if (reszta) {
            sem_p(kasa, 1);
            zajmuje_kase = 1;
            
            //kasa[0] = 0; // TEST BRAKU BANKNOTÓW ZALICZONY
            
            // Jeśli nadwyżka została zapłacona, wydaj resztę
            while (reszta > 0) {  // Dopóki reszta nie zostanie zapłacona
                // Sprawdzenie, czy jest wystarczająca ilość banknotów w kasie do wydania reszty
                if (pamiec[2] > 0 && reszta >= 50) {
                    pamiec[2]--;  // Zmniejsz liczbę 50zł w kasie
                    kom.msg[2]++; // Przekaż klientowi
                    reszta -= 50;  // Zmniejsz resztę
                } else if (pamiec[1] > 0 && reszta >= 20) {
                    pamiec[1]--;  // Zmniejsz liczbę 20zł w kasie
                    kom.msg[1]++; // Przekaż klientowi
                    reszta -= 20;  // Zmniejsz resztę
                } else if (pamiec[0] > 0 && reszta >= 10) {
                    pamiec[0]--;  // Zmniejsz liczbę 10zł w kasie
                    kom.msg[0]++; // Przekaż klientowi
                    reszta -= 10;  // Zmniejsz resztę
                } else if (pamiec[0] == 0) {
                // Jeśli kasa nie ma odpowiednich banknotów, czekaj na nowe wpłaty
                    printf("\033[0;34m[FRYZJER %ld]: Brak wystarczających środków na resztę, czekam.\033[0m\n", ja);
                    sem_v(kasa, 1);
                    zajmuje_kase = 0;
                    sleep(3);  // Czekaj na nowe wpłaty
                    sem_p(kasa, 1);
                    zajmuje_kase = 1;
                }
            }
            sem_v(kasa, 1);
            zajmuje_kase = 0;
        }

        // Przekazanie reszty
        printf("\033[0;34m[FRYZJER %ld]: Przekazuję klientowi %ld resztę %d zł.\033[0m\n", ja, klient_id, suma_banknoty(kom.msg));
        wyslij_komunikat(kolejka, &kom);
    }

    fprintf(stderr, "Wypadłem z pętli fryzjera %ld", ja);
    exit(EXIT_FAILURE);
}

void sygnal_1(int sig) {
    printf("\033[0;34m[FRYZJER %ld]: Otrzymałem sygnał 1, żegnam.\033[0m\n", ja);
    if (zajmuje_fotel) {
        printf("\033[0;34m[FRYZJER %ld]: Zwalniam fotel.\033[0m\n", ja);
        sem_v(fotele, 1);
    }
    if (zajmuje_kase) {
        printf("\033[0;34m[FRYZJER %ld]: Zwalniam kasę.\033[0m\n", ja);
        sem_v(kasa, 1);
    }
    odlacz_pamiec_dzielona(pamiec);
    exit(EXIT_SUCCESS);
}