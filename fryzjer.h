#ifndef FRYZJER_H
#define FRYZJER_H

<<<<<<< HEAD
typedef struct {
    int id;
    int status; // 0 - wolny, 1 - zajęty
    int kasa;
} Fryzjer;

void fryzjer_pracuje(Fryzjer *fryzjer);
=======
void *fryzjer_routine(void *arg); // Funkcja wykonywana przez każdy wątek fryzjera
>>>>>>> 9bae1b699490254930a05064bc503b7665153041

#endif
