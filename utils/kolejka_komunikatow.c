#include "kolejka_komunikatow.h"

int utworz_kolejke_komunikatow(key_t klucz)
{
    int kolejka;
    if ((kolejka = msgget(klucz, IPC_CREAT | 0600)) == -1)
    {
        perror("Nie udalo sie stworzyc kolejki komunikatów.");
        exit(EXIT_FAILURE);
    }
    return kolejka;
}

void odbierz_komunikat(int kolejka, struct komunikat *kom, long odbiorca)
{
    if ((msgrcv(kolejka, (struct msgbuf *)kom, sizeof(struct komunikat) - sizeof(long), odbiorca, 0)) == -1)
    {
        perror("Nie odało się odczytać komunikatu z kolejki komunikatów.\n");
        exit(EXIT_FAILURE);
    }
}

void wyslij_komunikat(int kolejka, struct komunikat* kom)
{
    if ((msgsnd(kolejka, (struct msgbuf *)kom, sizeof(struct komunikat) - sizeof(long), 0)) == -1)
    {
        perror("Nie udało się dodać komunikatu do kolejki komunikatów.\n");
        exit(EXIT_FAILURE);
    }
}

void usun_kolejke_komunikatow(int kolejka) {
    if (msgctl(kolejka, IPC_RMID, NULL) == -1)
    {
        perror("Nie odało się usunąć kolejki komunikatów.\n");
        exit(EXIT_FAILURE);
    }
    
}