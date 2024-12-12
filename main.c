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

//TODO: modifier la strucutre avl pour integrer la struct station
typedef struct avl_struct {
    int value; // La valeur du nœud
    int eq; // Facteur d'équilibre (balance factor)
    struct avl_struct *fg; // Pointeur vers le fils gauche
    struct avl_struct *fd; // Pointeur vers le fils droit
} AVL, *pAVL;

typedef struct Station {
    int id;
    int type; // TODO: préparer un enum pour le type ou juste le mettre en commentaire
    long capacité;
    long conso; // TODO: réparer
} Station, *pStation;

//TODO: refaire la fonction init
pAVL creerAVL(int e) {
    // Alloue de la mémoire pour un nouveau nœud
    pAVL new = (pAVL) malloc(sizeof(AVL));
    if (new == NULL) {
        exit(EXIT_FAILURE); // Arrêt immédiat en cas d'erreur d'allocation
    }
    new->value = e; // Initialisation de la valeur
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
    pivot->fd = a; // `a` devient le fils droit du pivot

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

pAVL insertionAVL(pAVL a, int e, int *h) {
    if (a == NULL) {
        // Si l'arbre est vide, crée un nouveau nœud
        *h = 1; // La hauteur a augmenté
        return creerAVL(e);
    } else if (e < a->value) {
        // Si l'élément est plus petit, insérer à gauche
        a->fg = insertionAVL(a->fg, e, h);
        *h = -*h; // Inverse l'impact de la hauteur
    } else if (e > a->value) {
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

pAVL suppMinAVL(pAVL a, int *h, int *pe) {
    pAVL temp;
    if (a->fg == NULL) {
        // Trouvé le plus petit élément
        *pe = a->value; // Sauvegarde la valeur
        *h = -1; // Réduction de la hauteur
        temp = a;
        a = a->fd; // Le sous-arbre droit devient la racine
        free(temp);
        return a;
    } else {
        a->fg = suppMinAVL(a->fg, h, pe); // Recherche récursive à gauche
        *h = -*h;
    }

    // Mise à jour et rééquilibrage après suppression
    if (*h != 0) {
        a->eq += *h;
        a = equilibrerAVL(a);
        *h = (a->eq == 0) ? -1 : 0;
    }
    return a;
}

pAVL suppressionAVL(pAVL a, int e, int *h) {
    pAVL temp;
    if (a == NULL) {
        // Élément introuvable
        *h = 0; //attenttion faute dans le CM
        return a;
    }
    if (e > a->value) {
        // Recherche dans le sous-arbre droit
        a->fd = suppressionAVL(a->fd, e, h);
    } else if (e < a->value) {
        // Recherche dans le sous-arbre gauche
        a->fg = suppressionAVL(a->fg, e, h);
        *h = -*h;
    } else if (a->fd != NULL) {
        // Si le nœud a un fils droit
        a->fd = suppMinAVL(a->fd, h, &(a->value));
    } else {
        // Cas du nœud feuille ou avec un seul fils gauche
        temp = a;
        a = a->fg;
        free(temp);
        *h = -1;
        return a;
    }
    if (a == NULL) {
        return a;
    }
    // Mise à jour et rééquilibrage après suppression
    if (*h != 0) {
        a->eq += *h;
        a = equilibrerAVL(a);
        *h = (a->eq == 0) ? -1 : 0;
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
           nd->value, nd->eq);

    // Affiche ensuite le sous-arbre gauche
    afficherAVL(nd->fg, niveau + 1);
}

int main() {
    pAVL avl = NULL;
    int hauteur = 0;
    avl = insertionAVL(avl, 10, &hauteur);
    int balance = 0;
    avl = insertionAVL(avl, 3, &balance);
    avl = insertionAVL(avl, 9, &balance);
    afficherAVL(avl, 0);
    printf("Hello World\n");
    FILE *fichier = NULL;

    fichier = fopen("tmp/input.dat", "r");
    if (fichier != NULL) {
        long chaine[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        fscanf(fichier, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4],
               &chaine[5], &chaine[6], &chaine[7]);
        for (int i = 0; i < 8; i++) {
            printf("%ld ", chaine[i]);
        }
        fclose(fichier);
    } else {
        // On affiche un message d'erreur si on veut
        printf("Impossible d'ouvrir le fichier input.dat");
        exit(1);
    }
    return 0;
}
