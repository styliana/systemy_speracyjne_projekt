#include "pamiec_dzielona.h"

int utworz_pamiec_dzielona(key_t klucz)
{
    int id = shmget(klucz, sizeof(int[NOMINALY]), 0600 | IPC_CREAT);
    if (id == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return id;
}

int *dolacz_pamiec_dzielona(int id)
{
    int *ptr = shmat(id, 0, 0);
    if (*ptr == -1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void odlacz_pamiec_dzielona(int *ptr)
{
    int res = shmdt(ptr);
    if (res == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void usun_pamiec_dzielona(int id)
{
    int res = shmctl(id, IPC_RMID, 0);
    if (res == -1)
    {
        perror("shmctl IPC_RMID");
        exit(EXIT_FAILURE);
    }
}