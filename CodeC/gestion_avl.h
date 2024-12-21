//
// Created by natha on 19/12/2024.
//

#ifndef GESTION_AVL_H
#define GESTION_AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "gestion_station.h"
#include "macro.h"

typedef struct AVL {
    pStation station;
    int eq; // Facteur d'équilibre
    struct AVL *fg; // Pointeur vers le fils gauche
    struct AVL *fd; // Pointeur vers le fils droit
} AVL, *pAVL;


pAVL creerAVL(pStation station);
pAVL rotationGauche(pAVL avl);
pAVL rotationDroite(pAVL avl);
pAVL doubleRotationGauche(pAVL a);
pAVL doubleRotationDroite(pAVL a);
pAVL equilibrerAVL(pAVL a);
pAVL insertionAVL(pAVL a, pStation station, int *h);
pAVL rechercheAVL(pAVL a, unsigned long id);
void liberation_avl(pAVL avl);
void afficherAVL(pAVL nd, int niveau);

#endif //GESTION_AVL_H
