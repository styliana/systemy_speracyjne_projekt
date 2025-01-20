#! /bin/bash
gcc kierownik.c config.c utils/kolejka_komunikatow.c utils/pamiec_dzielona.c utils/semafory.c -I./utils -o kierownik -pthread
gcc klient.c config.c utils/kolejka_komunikatow.c utils/pamiec_dzielona.c utils/semafory.c -I./utils -o klient -pthread
gcc fryzjer.c config.c utils/kolejka_komunikatow.c utils/pamiec_dzielona.c utils/semafory.c -I./utils -o fryzjer -pthread