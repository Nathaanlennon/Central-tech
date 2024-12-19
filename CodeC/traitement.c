//
// Created by natha on 19/12/2024.
//

#include "traitement.h"

// TODO: vérifier que les consommateurs sont bien reliés à la station
// Traite le fichier mis en argument pour le convertir en AVL. L'avl mit en argument doit être nul et la hauteur doit valoir 0, sinon les deux seront réinitialisés
pAVL traitement_input(FILE *fichier, pAVL avl, int *hauteur) {
    if (fichier == NULL) {
        return NULL; //Si le fichier n'existe pas, rien à convertir en avl
    }
    if (avl != NULL) {
        //l'avl n'est pas nul
        liberation_avl(avl); //on vide l'avl
        avl = NULL; //on le remet à null
        *hauteur = 0; // et on remet la hauteur à 0
    }
    unsigned long chaine[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //initialisation de la chaine de caractère qui prend la ligne actuelle
    pStation station = NULL; //préparation de la station que l'on va créer

    //la première iteration de la boucle est en dehors das le cas où la premièr ligne n'est pas une station, auquel cas on suppose sa station et sa capacité est mise à 0 par défaut
    if (fscanf(fichier, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4],
               &chaine[5], &chaine[6], &chaine[7]) != EOF) { //récupération de la première ligne, eof signifie que le fichier est vide
        if (get_type_station(chaine) != DEFAUT) { //on vérifie que le type n'est pas incorrect et qu'il s'agit bien d'une station, défaut veut dire que ce n'est pas une station
            station = creer_station(chaine); //on crée la nouvelle station à partir de la ligne
            avl = insertionAVL(avl, station, hauteur); //et on intègre la station dans l'AVL
        } else { //la ligne n'est pas une station, donc il s'agit d'un consommateur, auquel cas on récupère sa consommation pour l'intégrer à celle de la station
            if (station == NULL) { // si la première ligne n'est pas une station, crée une station avec capacité inconnue, à 0 par défaut
                unsigned long temp[8] = {chaine[0], chaine[1], chaine[2], chaine[3], 0, 0, 0, 0};
                station = creer_station(temp);
                avl = insertionAVL(avl, station, hauteur);
            }
            if (chaine[station->type] == station->id) {
                station->conso += chaine[7];//integration de la consommation du consommateur à celle de sa station
            }
            else {
                // TODO: faire la partie où le consommateur n'est pas lié à la station + tout rajouter dans la boucle while
                // potentiellement le stocker quelque part et ensuite reparcourir l'avl pour trouver sa station, le supprimer sinon
            }
        }
    }
    // Traitement générique du fichier. Le commentary est le même puisque le code est dédoublé pour éviter une vérification que la station existe à chaque itération
    while (fscanf(fichier, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3],
                  &chaine[4],
                  &chaine[5], &chaine[6], &chaine[7]) != EOF) {
        if (get_type_station(chaine) == station->type) { //ici, on vérifie que le type de station est identique au type de la station précédante pour avoir un avl cohérant
            station = creer_station(chaine);
            avl = insertionAVL(avl, station, hauteur);
        } else {
            if (chaine[station->type] == station->id) {
                station->conso += chaine[7];//integration de la consommation du consommateur à celle de sa station
            }
            else {
                // TODO: faire la partie où le consommateur n'est pas lié à la station + tout rajouter dans la boucle while
                // potentiellement le stocker quelque part et ensuite reparcourir l'avl pour trouver sa station, le supprimer sinon
            }
        }
    }
    return avl; // on retourne l'avl
}

void AVL_to_output(pAVL a, FILE* fichier) {
    if (a==NULL || fichier == NULL) {
        return;
    }
    if (a->fg != NULL) {
        AVL_to_output(a->fg, fichier);
    }
    fprintf(fichier, "%lu:%lu:%lu\n", a->station->id, a->station->capacité, a->station->conso);
    if (a->fd != NULL) {
        AVL_to_output(a->fd, fichier);
    }
}