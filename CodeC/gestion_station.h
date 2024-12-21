//
// Created by natha on 19/12/2024.
//

#ifndef GESTION_STATION_H
#define GESTION_STATION_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "macro.h"

enum type_station { DEFAUT, HVB, HVA, LV }; //defaut équivaut à nul, vide, etc

typedef struct Station {
    unsigned long id;
    unsigned int type; // voir enum type_station
    unsigned long capacité;
    unsigned long conso;
} Station, *pStation;

unsigned int get_station_type(unsigned long chaine[8]);
pStation creer_station(unsigned long chaine[8]);



#endif //GESTION_STATION_H
