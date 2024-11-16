#ifndef UTILS_H
#define UTILS_H

void initialize_salon();
void cleanup_resources();
void zajmij_fotel();
void zwolnij_fotel();
void pobierz_opłatę(Klient *klient);
void wydaj_resztę(Klient *klient);
void enqueue_client();
Klient* dequeue_client();

#endif
