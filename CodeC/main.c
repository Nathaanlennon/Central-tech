#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#include "gestion_avl.h"
#include "gestion_station.h"
#include "macro.h"
#include "traitement.h"





int main() {
    pAVL avl = NULL; // initialisation de l'avl
    int hauteur = 0; //hauteur par défaut est de 0
    FILE *fichier = NULL; //préparation du pointeur vers le fichier input

    // TODO: quand les fichiers seront réassemblés selon la consigne, éditer le chemin vers input
    fichier = fopen("../tmp/input.dat", "r"); //ouverture d'input en mode lecture

    if (fichier != NULL) { //si le fopen a réussi
        avl = traitement_input(fichier, avl, &hauteur); //on va traiter le fichier et construire l'avl
        fclose(fichier); //puis on ferme le fichier
    } else {
        // On affiche un message d'erreur si on veut
        printf("Impossible d'ouvrir le fichier input.dat\n");
        exit(1);
    }
    FILE * output = NULL;
    output = fopen("../tmp/output.csv", "w");
    if (output == NULL) {
        printf("Impossible d'ouvrir le fichier output.dat\n");
        exit(1);
    }
    AVL_to_output(avl, output);
    fclose(output);

    liberation_avl(avl); //libération de l'avl
    return 0;
}
