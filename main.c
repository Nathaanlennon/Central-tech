//
// Created by natha on 04/12/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define NL printf("\n")
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

enum type_station {DEFAUT, HVB, HVA, LV}; //defaut équivaut à nul, vide, etc

typedef struct Station {
    int id;
    int type; // voir enum type_station
    long capacité;
    long conso;
} Station, *pStation;


typedef struct AVL {
    pStation station;
    int eq; // Facteur d'équilibre (balance factor)
    struct AVL *fg; // Pointeur vers le fils gauche
    struct AVL *fd; // Pointeur vers le fils droit
} AVL, *pAVL;


int get_type_station(long chaine[8]) {
    if (chaine[4] != 0 || chaine[5] !=0) {
        return DEFAUT;
    }
    if (chaine[3]>0) {
        return LV;
    }
    else if (chaine[2]>0) {
        return HVA;
    }
    else if (chaine[1]>0) {
        return HVB;
    }
    else return DEFAUT;
}

// crée une station en fonction de la chaine associée, renvoie NULL si valeurs incorrectes. Ne s'occupe pas de la consommation
pStation creer_station(long chaine[8]) {
    pStation station = malloc(sizeof(Station));
    int type = get_type_station(chaine);
    switch (type) {
        case LV:
            station->type = LV;
            station->id = chaine[3];
            break;
        case HVA:
            station->type = HVA;
            station->id = chaine[2];
            break;
        case HVB:
            station->type = HVB;
            station->id = chaine[1];
            break;
        case DEFAUT:
            free(station);
            return NULL;
    }
    station->capacité = chaine[6];
    return station;
}


pAVL creerAVL(pStation e) {
    // Alloue de la mémoire pour un nouveau nœud
    pAVL new = (pAVL) malloc(sizeof(AVL));
    if (new == NULL) {
        exit(EXIT_FAILURE); // Arrêt immédiat en cas d'erreur d'allocation
    }
    new->station = e; // Initialisation de la valeur
    new->fg = NULL; // Pas de fils gauche
    new->fd = NULL; // Pas de fils droit
    new->eq = 0; // Facteur d'équilibre initialisé à 0
    return new;
}

pAVL rotationGauche(pAVL a) {
    pAVL pivot = a->fd; // Le fils droit devient le pivot
    int eq_a = a->eq, eq_p = pivot->eq;

    a->fd = pivot->fg; // Le sous-arbre gauche du pivot devient le fils droit de `a`
    pivot->fg = a; // `a` devient le fils gauche du pivot

    // Mise à jour des facteurs d'équilibre
    a->eq = eq_a - MAX(eq_p, 0) - 1;
    pivot->eq = MIN(MIN(eq_a - 2, eq_a + eq_p - 2), eq_p - 1);

    return pivot; // Le pivot devient la nouvelle racine
}

pAVL rotationDroite(pAVL a) {
    pAVL pivot = a->fg; // Le fils gauche devient le pivot
    int eq_a = a->eq, eq_p = pivot->eq;

    a->fg = pivot->fd; // Le sous-arbre droit du pivot devient le fils gauche de `a`
    pivot->fd = a; // 'a' devient le fils droit du pivot

    // Mise à jour des facteurs d'équilibre
    a->eq = eq_a - MIN(eq_p, 0) + 1;
    pivot->eq = MAX(MAX(eq_a + 2, eq_a + eq_p + 2), eq_p + 1);

    return pivot; // Le pivot devient la nouvelle racine
}

pAVL doubleRotationGauche(pAVL a) {
    a->fd = rotationDroite(a->fd);
    return rotationGauche(a);
}

pAVL doubleRotationDroite(pAVL a) {
    a->fg = rotationGauche(a->fg);
    return rotationDroite(a);
}

pAVL equilibrerAVL(pAVL a) {
    if (a->eq >= 2) {
        // Cas où l'arbre est déséquilibré à droite
        if (a->fd->eq >= 0) {
            return rotationGauche(a); // Rotation simple gauche
        } else {
            return doubleRotationGauche(a); // Double rotation gauche
        }
    } else if (a->eq <= -2) {
        // Cas où l'arbre est déséquilibré à gauche
        if (a->fg->eq <= 0) {
            return rotationDroite(a); // Rotation simple droite
        } else {
            return doubleRotationDroite(a); // Double rotation droite
        }
    }
    return a; // Aucun rééquilibrage nécessaire
}

pAVL insertionAVL(pAVL a, pStation e, int *h) {
    if (e==NULL) {
        return a;
    }
    if (a == NULL) {
        // Si l'arbre est vide, crée un nouveau nœud
        *h = 1; // La hauteur a augmenté
        return creerAVL(e);
    } else if (e->id < a->station->id) {
        // Si l'élément est plus petit, insérer à gauche
        a->fg = insertionAVL(a->fg, e, h);
        *h = -*h; // Inverse l'impact de la hauteur
    } else if (e->id > a->station->id) {
        // Si l'élément est plus grand, insérer à droite
        a->fd = insertionAVL(a->fd, e, h);
    } else {
        // Élément déjà présent
        *h = 0;
        return a;
    }

    // Mise à jour du facteur d'équilibre et rééquilibrage si nécessaire
    if (*h != 0) {
        a->eq += *h;
        a = equilibrerAVL(a);
        *h = (a->eq == 0) ? 0 : 1; // Mise à jour de la hauteur
    }
    return a;
}


// Affichage de l'arbre en mode horizontal
void afficherAVL(pAVL nd, int niveau) {
    if (nd == NULL) {
        return;
    }

    // Affiche d'abord le sous-arbre droit (pour un effet visuel en "arbre")
    afficherAVL(nd->fd, niveau + 1);

    // Affiche le noeud actuel avec indentation
    for (int i = 0; i < niveau; i++) {
        printf("     "); // Ajoute des espaces pour indenter
    }
    printf("%d "
           "\x1B[0;34m"
           "%d\n"
           "\x1B[0m",
           nd->station->id, nd->eq);

    // Affiche ensuite le sous-arbre gauche
    afficherAVL(nd->fg, niveau + 1);
}

pAVL traitement_input(FILE* fichier, pAVL avl, int* hauteur) {
    if (fichier == NULL) {
        return NULL;
    }
    if (avl !=NULL) {
        // TODO:fonction qui libère l'avl entièrement
    }
    long chaine[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    pStation station = NULL;

    // TODO: modifier la boucle et la vérification pour éviter de faire e if tation == null à  chaque coup
    while (fscanf(fichier, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4],
           &chaine[5], &chaine[6], &chaine[7]) != EOF) {
        if (get_type_station(chaine) != DEFAUT) {
            station = creer_station(chaine);
            avl = insertionAVL(avl, station, hauteur);
        }
        else {
            if (station == NULL) {
                long temp[8] = {chaine[1], chaine[2], chaine[3], chaine[4], chaine[5],0, 0}; //si la première ligne n'est pas une station, crée une station avec capacité inconnue
                station = creer_station(temp);
            }
            station->conso += chaine[7];
        }
    }
    return avl;
}

int main(){
    pAVL avl = NULL;
    int hauteur = 0;
    FILE *fichier = NULL;

    fichier = fopen("tmp/input.dat", "r");

    if (fichier != NULL) {
        avl = traitement_input(fichier, avl, &hauteur);
        fclose(fichier);
    } else {
        // On affiche un message d'erreur si on veut
        printf("Impossible d'ouvrir le fichier input.dat");
        exit(1);
    }
    afficherAVL(avl, hauteur);


    return 0;
}
