//
// Created by natha on 19/12/2024.
//

#ifndef TRAITEMENT_H
#define TRAITEMENT_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "gestion_avl.h"

pAVL traitement_input(FILE* fichier, pAVL avl, int *hauteur);
void AVL_to_output(pAVL a, FILE* fichier);

#endif //TRAITEMENT_H
