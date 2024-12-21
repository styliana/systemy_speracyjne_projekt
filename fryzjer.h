#ifndef FRYZJER_H
#define FRYZJER_H

typedef struct {
    int id;
    int status; // 0 - wolny, 1 - zajęty
    int kasa;
} Fryzjer;

void fryzjer_pracuje(Fryzjer *fryzjer);

#endif
