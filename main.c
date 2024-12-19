#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define NL printf("\n")
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

enum type_station { DEFAUT, HVB, HVA, LV }; //defaut équivaut à nul, vide, etc

typedef struct Station {
    unsigned long id;
    unsigned int type; // voir enum type_station
    unsigned long capacité;
    unsigned long conso;
} Station, *pStation;


typedef struct AVL {
    pStation station;
    int eq; // Facteur d'équilibre
    struct AVL *fg; // Pointeur vers le fils gauche
    struct AVL *fd; // Pointeur vers le fils droit
} AVL, *pAVL;

//Permet d'obtenir le type de la station que l'on crée et le retourne sous la forme d'un int, voir enum type_station
unsigned int get_type_station(unsigned long chaine[8]) {
    if (chaine[4] != 0 || chaine[5] != 0) {
        //vérifie qu'il s'agit d'une station, retourne le type defaut donc erreur. Il s'agit donc probablement d'un consommateur
        return DEFAUT;
    }
    if (chaine[3] > 0) {
        //on commence par lv car les sous stations possèdent quand même l'id de leur station mère
        return LV;
    } else if (chaine[2] > 0) {
        //même principe
        return HVA;
    } else if (chaine[1] > 0) {
        return HVB;
    } else return DEFAUT;
}

// crée une station en fonction de la chaine associée, renvoie NULL si valeurs incorrectes. Ne s'occupe pas de la consommation
pStation creer_station(unsigned long chaine[8]) {
    pStation station = NULL;
    station = malloc(sizeof(Station)); //allocation mémoire en fonction de la taille de la structure
    unsigned int type = get_type_station(chaine);
    switch (type) {
        case LV:
            station->id = chaine[3]; //récupère l'id dans la colone associée au type (pareil pour chaque cas du switch)
            break;
        case HVA:
            station->id = chaine[2];
            break;
        case HVB:
            station->id = chaine[1];
            break;
        case DEFAUT:
        default:
            free(station);
        //si le type n'est pas dans l'enum ou est en défaut, ce qui veut dire invalide, retourne NULL et libère la mémoire car il ne s'agit pas d'une station
            return NULL;
    }
    station->type = type; //renseigne le type dans la structure
    station->capacité = chaine[6]; //récupère la capacité de la station dans sa colone
    station->conso = 0; //par défaut, la consomation est à 0, car c'est avant calcul
    return station; //retourne la station
}

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
    printf("%lu "
           "\x1B[0;34m"
           "%d\n"
           "\x1B[0m",
           nd->station->id, nd->eq);

    // Affiche ensuite le sous-arbre gauche
    afficherAVL(nd->fg, niveau + 1);
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
    free(avl); //on libère le noeud actuel
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

int main() {
    pAVL avl = NULL; // initialisation de l'avl
    int hauteur = 0; //hauteur par défaut est de 0
    FILE *fichier = NULL; //préparation du pointeur vers le fichier input

    // TODO: quand les fichiers seront réassemblés selon la consigne, éditer le chemin vers input
    fichier = fopen("tmp/input.dat", "r"); //ouverture d'input en mode lecture

    if (fichier != NULL) { //si le fopen a réussi
        avl = traitement_input(fichier, avl, &hauteur); //on va traiter le fichier et construire l'avl
        fclose(fichier); //puis on ferme le fichier
    } else {
        // On affiche un message d'erreur si on veut
        printf("Impossible d'ouvrir le fichier input.dat");
        exit(1);
    }


    FILE * output = NULL;
    output = fopen("tmp/output.csv", "w");
    if (output == NULL) {
        printf("Impossible d'ouvrir le fichier output.dat");
        exit(1);
    }
    AVL_to_output(avl, output);
    fclose(output);

    liberation_avl(avl); //libération de l'avl
    return 0;
}
