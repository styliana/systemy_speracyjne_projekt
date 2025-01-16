#include "config.h"

int suma_banknoty(int *banknoty)
{
    int suma = 0;
    suma += *banknoty * 10;
    suma += *(++banknoty) * 20;
    suma += *(++banknoty) * 50;
    return suma;
}