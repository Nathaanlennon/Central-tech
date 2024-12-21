//
// Created by natha on 19/12/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "traitement.h"


// Traite le fichier mit en argument pour le convertir en AVL. L'avl mit en argument doit être nul et la hauteur doit valoir 0, sinon les deux seront réinitialisés. La fonction essaye au maximum de fonctionner malgré un tri approximatif ou un manque de données et fournira à l'utilisteur le maximum de données possibles en cas de probleme
pAVL traitement_input(FILE* fichier, pAVL avl, int *hauteur){
    if(fichier == NULL){ //fichier null ou n'existe pas, rien à traiter
        return NULL;
    }
    if(avl != NULL){ //si l'avl n'est pas nul, on le vide entièrement
        liberation_avl(avl);
        avl = NULL;
        *hauteur = 0;
    }
    unsigned long chaine[8]={0,0,0,0,0,0,0,0}; //chaine qui va récupérer les lignes du fichier
    pStation station = NULL;
    bool filtrage_ok = true; //définition d'un booléen qui suppose quele filtrage en shell a correctement fonctionné

    FILE* fichier2 = NULL; //le fichier où l'on mettra les les consommateurs sans station
    fichier2 = fopen("../tmp/consommateurs_seuls.csv", "w"); //récupère les consommateurs qui ne sont pas reliés à la station actuelle du traitement (prbleme au niveau du tri du fichier)
    while(fscanf(fichier, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4], &chaine[5], &chaine[6], &chaine[7]) != EOF){
        unsigned int station_type = get_station_type(chaine); //on récupère le type de la chaine selon l'enum type station
      	if (station_type != DEFAUT) { //on vérifie que le type n'est pas incorrect et qu'il s'agit bien d'une station, défaut veut dire que ce n'est pas une station
            if(avl != NULL){ //si la racine existe,  on vérifie que la nouvelle station est du même type que la racine
        		if(station_type == avl->station->type){ //on vérifie si le type de station est bien le même que l'avl (le premier élément est choisit comme référence)
                    station = creer_station(chaine);
                	avl = insertionAVL(avl, station, hauteur);
        		}
            }
            else{ //si la racine n'existe pas, alors on crée la racine
                station = creer_station(chaine);
        		avl = insertionAVL(avl, station, hauteur);
            }
        } else{ //partie pour les consommateurs
            if(station == NULL){ //si la station actuelle n'existe pas, concretement ça veut dire que la première ligne n'est pas une station, donc on met le consommateur dans le fichier temporaire
            	filtrage_ok = false; //si on arrive ici c'est que le filtrage a échoué, on devra donc compenser
                if(fichier2 != NULL){ //si on a réussi à ouvrir le fichier temporaire
                	fprintf(fichier2,"%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n", chaine[0], chaine[1], chaine[2], chaine[3], chaine[4], chaine[5], chaine[6], chaine[7]); //on ajoute la ligne du consommateur dans le fichier temporaire
                }
            }
            else if(chaine[station->type] == station->id){ //la station existe, on vérifie que le consommateur est bien relié à la station
              	switch(station->type){ //on vérifie ici que le type de consommateur est bien correct, seules les compagies peuvent être reliées à des hvb ou hva
                  	case HVB:
                	case HVA:
                		if(chaine[5] == 0){ //la 6e colone correspond aux particuliers. Une compagnie aura la valeur 0 dans cette colone
                        	station->conso += chaine[7]; //si toutes les conditions sont respectées, ajout de la consommation dans la station
                		}
                        break;
                    default:
                      	station->conso += chaine[7]; //pour lv, tous les types de consommateurs fonctionnent. Par defaut, le programme considérera lv_all, il est incapable de savoir si le consommateur souhaite un type en particulier
                      	break;
              	}

            }
            else{ //le consommateur n'est pas relié à la station actuelle
            	filtrage_ok = false; //ça veut dire que le filtrage a échoué, ou du moins le tri lors du filtrage
            	if(fichier2 != NULL){ //même chose que l'autre cas où ça a échoué
                    fprintf(fichier2,"%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n", chaine[0], chaine[1], chaine[2], chaine[3], chaine[4], chaine[5], chaine[6], chaine[7]);
                }
            }
        }
    }
    if(filtrage_ok == false){ //si le filtrage a échoué, un second passage est necessaire, sur le fichier temporaire cette fois
        if(avl != NULL){ //on vérifie que l'avl existe et qu'il y a donc des stations sur lesquelles ajouter les consommateurs
            printf("Tri lors du filtrage incorrect, compensation\n"); //on indique au consommateur ce qu'il se passe
            if(fichier2 == NULL){ //on n'avait pas réussi à ouvrir le fichier temporaire donc on a pas pu sauvegarder les differents consommateurs seuls
                printf("Impossibilité d'ouverture du fichier de sauvegarde des consommateurs inconnus, perte de donnée possible\n");
            }
            else{//on avait réussi à ouvrir le fichier et avons pu sauvegarder les consommateurs seuls
                fclose(fichier2); //fermeture et réouverture en mode lecture poru revenir au début du fichier
                fichier2 = NULL;
                fichier2 = fopen("../tmp/consommateurs_seuls.csv", "r");

                if(fichier2 == NULL){ //on arrive pas à ouvrir le fichier pour le lire, les données ne seront pas prises en compte dans le reste du traitement mais sont toujours visionnables par le consommateur
                    printf("Impossible de lecture du fichier de sauvegarde des consommateurs inconnus, perte de donnée possible au niveau du fichier final\n");
                }
                else{ //on a réussi à ouvrir le fichier pour la lecture
                    unsigned long inconnus = 0; //on prépare une variable pour connaitre le nombre de consommateurs inconnus, c'est à dire que l'on a pas trouvé leur station dans l'avl
                    FILE* fichier3 = NULL; //création du fichier dans lequel les consommateurs sans station seront tout de même sauvegardés
                    fichier3 = fopen("../tmp/consommateurs_inconnus.csv", "w");
                    if(fichier3 != NULL){ //on a réussi à ouvrir le fichier
                        while(fscanf(fichier2, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4], &chaine[5], &chaine[6], &chaine[7]) != EOF){
                        	pAVL avl_station = rechercheAVL(avl, chaine[avl->station->type]); //on regarde pour chaque consommateur si sa station est dans l'avl
                            if(avl_station == NULL){ //si la station n'est pas dans l'avl
                            	if(chaine[avl->station->type] != 0){ //si l'id du type de la racine vaut 0, le consommateur ne fait pas partie de ce que demande l'utilisateur du programme
                                	inconnus++; //on augmente le nombre de consommateurs inconnus
                                	fprintf(fichier3, "%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n", chaine[0], chaine[1], chaine[2], chaine[3], chaine[4],chaine[5],chaine[6],chaine[7]); //et on l'ajoute dans le fichier des consomateurs inconnus
                            	}
							}
                            else{ //la station est dans l'avl, donc on rajoute la consommation du consommateur dans la statioj
                                switch(station->type){ //on respecte toujours les regles pour hvb et hva
                  					case HVB:
                					case HVA:
                						if(chaine[5] == 0){
                  					      	station->conso += chaine[7];
                						}
                  				     	break;
                  					default:
                      					station->conso += chaine[7];
                      					break;
              					}
                            }
                        }

                        fclose(fichier3); //une fois que tout le fichier des consommateurs inconnus a été rempli, on le ferme
                        if(inconnus != 0){ //on indique à l'utilisateur si des consommateurs n'ont pas été traités car on a pas trouvé leur station, selon le type de station qu'avait demnder l'utilisateur. Ces consommateurs non traités sont toujours accessibles pour l'utilisateur dans le fichier indiqué
                            printf("%lu consommateurs n'ont pas été traités, ils sont sauvegardés dans Central-Tech/tmp/consommateurs_seuls.csv\n", inconnus);
                        }
                    }
                    else{// on a pas réussi à ouvrir le fichier pour les consommateurs inconnus, donc on récupèrera ceux ajoutable dans l'avl et on laissera les autres
                        while(fscanf(fichier2, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4], &chaine[5], &chaine[6], &chaine[7]) != EOF){
                            pAVL avl_station = rechercheAVL(avl, chaine[avl->station->type]); //on recherche la station de chaque consommateur
                            if(avl_station == NULL){ //si on ne trouve pas la station, le consommateur n'est pas rajouté à l'avl, malheureusement on ne peut pas le sauvegarder dans le fichier des consommateurs inconnus car ici nous n'avons pas pu l'ouvrir
                                inconnus++; //on indente quand même pour prévenir l'utilisateur de la perte de données
                            }
                            else{ //on trouve la station et on rajoute la consommation
                                avl_station->station->conso += chaine[7];
                            }
                        }
                        if(inconnus != 0){ //si des consommateurs n'ont pas été identifiés, on en indique le nombre à l'utilisateur
                            printf("Impossible d'ouvrir le fichier de sauvegarde des consommateurs inconnus non traités, %lu consommateurs inconnus ont été perdus\n", inconnus);
                        }
                    }
                }
            }
        }
        else{// l'avl des stations est vide, donc il ne nous reste que des consommateurs seuls, on indique à l'utilisateur où les trouver
			printf("Le fichier ne contient aucune station, les consommateurs sont dans Central-Tech/tmp/consommateurs_seuls.csv\n");
        }
    	fclose(fichier2);//et on ferme enfin le fichier des consommateurs seuls
    }
    return avl; //et on retourne l'avl terminé
}

//récupère les informations de consommation de l'avl pour les envoyer dans le fichier de sortie entré en argument, sous la forme de colones séparées par des :
void AVL_to_output(pAVL a, FILE* fichier) {
    if (a==NULL || fichier == NULL) { //vérification que les arguments sont valides
        return;
    }

	//la fonction est un peu l'équivalent d'un parcours infixe pour faire un pré-tri du fichier par ordre croissan des ID
    if (a->fg != NULL) { //
        AVL_to_output(a->fg, fichier);
    }
    fprintf(fichier, "%lu:%lu:%lu\n", a->station->id, a->station->capacité, a->station->conso);
    if (a->fd != NULL) {
        AVL_to_output(a->fd, fichier);
    }
}