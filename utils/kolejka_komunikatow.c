#include "kolejka_komunikatow.h"

int utworz_kolejke_komunikatow(key_t klucz)
{
    int kolejka;
    if ((kolejka = msgget(klucz, IPC_CREAT | 0600)) == -1)
    {
        perror("");
        fprintf(stderr,"Nie udalo sie stworzyc kolejki komunikatów. PID: %d\n", getpid());
        exit(EXIT_FAILURE);
    }
    return kolejka;
}

void odbierz_komunikat(int kolejka, struct komunikat *kom, long odbiorca)
{
    int res = msgrcv(kolejka, (struct msgbuf *)kom, sizeof(struct komunikat) - sizeof(long), odbiorca, 0);
    if (res == -1)
    {
        if (errno == EINTR) {
            odbierz_komunikat(kolejka, kom, odbiorca);
        } else {
            perror("");
            fprintf(stderr,"Nie odało się odczytać komunikatu z kolejki komunikatów. PID: %d\n", getpid());
            exit(EXIT_FAILURE);
        }
    }
}

void wyslij_komunikat(int kolejka, struct komunikat* kom)
{
    int res = msgsnd(kolejka, (struct msgbuf *)kom, sizeof(struct komunikat) - sizeof(long), 0);
    if (res == -1)
    {
        if (errno == EINTR) {
            wyslij_komunikat(kolejka, kom);
        } else {
            perror("");
            fprintf(stderr,"Nie udało się dodać komunikatu do kolejki komunikatów. PID: %d\n", getpid());
            exit(EXIT_FAILURE);
        }
    }
}

void usun_kolejke_komunikatow(int kolejka) {
    if (msgctl(kolejka, IPC_RMID, NULL) == -1)
    {
        perror("");
        fprintf(stderr,"Nie odało się usunąć kolejki komunikatów. PID: %d\n", getpid());
        exit(EXIT_FAILURE);
    }
    
}