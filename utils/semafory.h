#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

int utworz_semafor(key_t klucz);
void usun_semafor(int id);
void ustaw_semafor(int id, int max);
void sem_p(int id, int n);
void sem_v(int id, int n);
int sem_p_nowait(int id, int n);
int sem_wartosc(int id);