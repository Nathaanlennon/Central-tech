//
// Created by natha on 19/12/2024.
//

#include "gestion_avl.h"
#include "macro.h"


//crée un chainon d'avl en prenant en argument une station
pAVL creerAVL(pStation station) {
    if (station == NULL) {
        return NULL;
        //si la station que l'on doit mettre dans l'avl est nulle, il n'y a aucune raison de creer l'avl donc on retourne null
    }
    // Alloue de la mémoire pour un nouveau nœud
    pAVL new = NULL;
    new = malloc(sizeof(AVL));
    if (new == NULL) {
        exit(EXIT_FAILURE); // Arrêt immédiat en cas d'erreur d'allocation
    }
    new->station = station; // on intègre la station dans l'avl
    new->fg = NULL; // Pas de fils gauche
    new->fd = NULL; // Pas de fils droit
    new->eq = 0; // Facteur d'équilibre initialisé à 0
    return new;
}

pAVL rotationGauche(pAVL avl) {
    pAVL pivot = avl->fd; // Le fils droit devient le pivot
    int eq_avl = avl->eq, eq_p = pivot->eq;

    avl->fd = pivot->fg; // Le sous-arbre gauche du pivot devient le fils droit de 'a'
    pivot->fg = avl; // 'a' devient le fils gauche du pivot

    // Mise à jour des facteurs d'équilibre
    avl->eq = eq_avl - MAX(eq_p, 0) - 1;
    pivot->eq = MIN(MIN(eq_avl - 2, eq_avl + eq_p - 2), eq_p - 1);

    return pivot; // Le pivot devient la nouvelle racine
}

pAVL rotationDroite(pAVL avl) {
    pAVL pivot = avl->fg; // Le fils gauche devient le pivot
    int eq_a = avl->eq, eq_p = pivot->eq;

    avl->fg = pivot->fd; // Le sous-arbre droit du pivot devient le fils gauche de 'a'
    pivot->fd = avl; // 'a' devient le fils droit du pivot

    // Mise à jour des facteurs d'équilibre
    avl->eq = eq_a - MIN(eq_p, 0) + 1;
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

//insere une nouvelle station dans l'avl
pAVL insertionAVL(pAVL a, pStation station, int *h) {
    if (station == NULL) {
        return a;
    }
    if (a == NULL) {
        // Si l'arbre est vide, crée un nouveau nœud
        *h = 1; // La hauteur a augmenté
        return creerAVL(station);
    } else if (station->id < a->station->id) {
        // Si l'élément est plus petit, insérer à gauche
        a->fg = insertionAVL(a->fg, station, h);
        *h = -*h; // Inverse l'impact de la hauteur
    } else if (station->id > a->station->id) {
        // Si l'élément est plus grand, insérer à droite
        a->fd = insertionAVL(a->fd, station, h);
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

pAVL rechercheAVL(pAVL a, unsigned long id) {
	if(a == NULL) {
    	return NULL;
	}
    if(a->station->id == id) {
    	return a;
    }
    if(id < a->station->id) {
    	return rechercheAVL(a->fg, id);
    }
    if(id > a->station->id) {
    	return rechercheAVL(a->fd, id);
    }
    return NULL;
}

// libère tout l'avl
void liberation_avl(pAVL avl) {
    if (avl == NULL) {
        return; // l'avl est déjà libéré, rien à faire
    }
    if (avl->fd != NULL) {
        liberation_avl(avl->fd); //on libère le sous arbre droit
    }
    if (avl->fg != NULL) {
        liberation_avl(avl->fg); //on libère le sous arbre gauche
    }
    free(avl->station);
    free(avl); //on libère le noeud actuel
}
