#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../config.h"

int utworz_pamiec_dzielona(key_t klucz);
int *dolacz_pamiec_dzielona(int id);
void odlacz_pamiec_dzielona(int* ptr); 
void usun_pamiec_dzielona(int id);