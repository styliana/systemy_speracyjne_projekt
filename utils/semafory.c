#include "semafory.h"

int utworz_semafor(key_t klucz)
{
    int id = semget(klucz, 1, 0600 | IPC_CREAT);
    if (id == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    return id;
}

void usun_semafor(int id)
{
    int res = semctl(id, 0, IPC_RMID);
    if (res == -1)
    {
        perror("semctl IPC_RMID");
        exit(EXIT_FAILURE);
    }
}

void ustaw_semafor(int id, int max)
{
    int res = semctl(id, 0, SETVAL, max);
    if (res == -1)
    {
        perror("semctl SETVAL");
        exit(EXIT_FAILURE);
    }
}

void sem_p(int id, int n)
{
    struct sembuf sem_buff;
    sem_buff.sem_num = 0;
    sem_buff.sem_op = -n;
    sem_buff.sem_flg = 0;
    int res = semop(id, &sem_buff, 1);
    if (res == -1)
    {
        if (errno == EINTR)
        {
            sem_p(id, n);
        }
        else
        {
            perror("semop p");
            exit(EXIT_FAILURE);
        }
    }
}

void sem_v(int id, int n)
{
    struct sembuf sem_buff;
    sem_buff.sem_num = 0;
    sem_buff.sem_op = n;
    sem_buff.sem_flg = 0;
    int res = semop(id, &sem_buff, 1);
    if (res == -1)
    {
        if (errno == EINTR)
        {
            sem_v(id, n);
        }
        else
        {
            perror("semop v");
            exit(EXIT_FAILURE);
        }
    }
}

int sem_p_nowait(int id, int n)
{
    struct sembuf sem_buff;
    sem_buff.sem_num = 0;
    sem_buff.sem_op = -n;
    sem_buff.sem_flg = IPC_NOWAIT;
    int res = semop(id, &sem_buff, 1);
    if (res == -1)
    {
        if (errno == EAGAIN)
        {
            return 1;
        }
        else if (errno == EINTR)
        {
            sem_p_nowait(id, n);
        }
        else
        {
            perror("semop p nowait");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

int sem_wartosc(int id) {
    int res = semctl(id, 0, GETVAL);
    if (res == -1)
    {
        perror("semctl GETVAL");
        exit(EXIT_FAILURE);
    }
    return res;
}