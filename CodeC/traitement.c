//
// Created by natha on 19/12/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "traitement.h"



pAVL traitement_input(FILE* fichier, pAVL avl, int *hauteur){
    if(fichier == NULL){ //fichier null ou n'existe pas, rien à traiter*
    	printf("saucisse\n");
        return NULL;
    }
    if(avl != NULL){ //si l'avl n'est pas nul, on le vide entièrement
        liberation_avl(avl);
        avl = NULL;
        *hauteur = 0;
    }
    unsigned long chaine[8]={0,0,0,0,0,0,0,0}; //chaine qui va récupérer les lignes du fichier
    pStation station = NULL;
    bool filtrage_ok = true;

    FILE* fichier2 = NULL;
    fichier2 = fopen("../tmp/utilisateurs_seuls.csv", "w"); //récupère les utilisateurs qui ne sont pas reliés à la station actuelle du traitement (prbleme au niveau du tri du fichier)
    while(fscanf(fichier, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4], &chaine[5], &chaine[6], &chaine[7]) != EOF){
        unsigned int station_type = get_station_type(chaine);
      	if (station_type != DEFAUT) { //on vérifie que le type n'est pas incorrect et qu'il s'agit bien d'une station, défaut veut dire que ce n'est pas une station
            if(avl != NULL){
        		if(station_type == avl->station->type){ //on vérifie si le type de station est bien le même que l'avl (le premier élément est choisit comme référence)
                	printf("5\n");
                    station = creer_station(chaine);
                	avl = insertionAVL(avl, station, hauteur);
        		}
            }
            else{
				printf("2\n");
                station = creer_station(chaine);
        		avl = insertionAVL(avl, station, hauteur);
            }
        } else{
            if(station == NULL){
            	printf("3\n");
            	filtrage_ok = false;
                if(fichier2 != NULL){
                	fprintf(fichier2,"%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n", chaine[0], chaine[1], chaine[2], chaine[3], chaine[4], chaine[5], chaine[6], chaine[7]);
                }
            }
            else if(chaine[station->type] == station->id){
              	printf("4\n");
				station->conso += chaine[7];
            }
            else{
            	filtrage_ok = false;
            	if(fichier2 != NULL){
                    fprintf(fichier2,"%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n", chaine[0], chaine[1], chaine[2], chaine[3], chaine[4], chaine[5], chaine[6], chaine[7]);
                }
            }
        }
    }
    printf("%d poule \n", filtrage_ok+ 5);
    if(filtrage_ok == false){
        if(avl != NULL){
            printf("Tri lors du filtrage incorrect, compensation");
            if(fichier2 == NULL){
                printf("Impossibilité d'ouverture du fichier de sauvegarde des utilisateurs inconnus, perte de donnée possible\n");
            }
            else{
                fclose(fichier2);
                fichier2 = NULL;
                fichier2 = fopen("../tmp/utilisateurs_seuls.csv", "r");

                if(fichier2 == NULL){
                    printf("Impossible de lecture du fichier de sauvegarde des utilisateurs inconnus, perte de donnée possible au niveau du fichier final\n");
                }
                else{
                    unsigned long inconnus = 0;
                    FILE* fichier3 = NULL;
                    fichier3 = fopen("../tmp/utilisateurs_inconnus.csv", "w");
                    printf("ok\n");
                    if(fichier3 != NULL){
                      	printf("ta mère\n");
                        while(fscanf(fichier2, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4], &chaine[5], &chaine[6], &chaine[7]) != EOF){
                            printf("%d monsieur\n", avl->station->type);
                        	pAVL avl_station = rechercheAVL(avl, chaine[avl->station->type]);
                            if(avl_station == NULL){
                            	if(chaine[avl->station->type] != 0){
                                	inconnus++;
                                	fprintf(fichier3, "%lu;%lu;%lu;%lu;%lu;%lu;%lu;%lu\n", chaine[0], chaine[1], chaine[2], chaine[3], chaine[4],chaine[5],chaine[6],chaine[7]);
                            	}
							}
                            else{
                                avl_station->station->conso += chaine[7];
                            }
                        }

                        fclose(fichier3);
                        if(inconnus != 0){
                          	printf("lalala\n");
                            printf("%lu utilisateurs n'ont pas été traités, ils sont sauvegardés dans Central-Tech/tmp/utilisateurs_seuls.csv\n", inconnus);
                        }
                    }
                    else{
                      	printf("aiaiaia\n");
                        while(fscanf(fichier2, "%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", &chaine[0], &chaine[1], &chaine[2], &chaine[3], &chaine[4], &chaine[5], &chaine[6], &chaine[7]) != EOF){
                            pAVL avl_station = rechercheAVL(avl, chaine[avl->station->type]);
                            if(avl_station == NULL){
                                inconnus++;
                            }
                            else{
                                avl_station->station->conso += chaine[3];
                            }
                        }
                        if(inconnus != 0){
                            printf("Impossible d'ouvrir le fichier de sauvegarde des utilisateurs inconnus non traités, %lu utilisateurs inconnus ont été perdus\n", inconnus);
                        }
                    }
                }
            }
        }
        else{
			printf("Le fichier ne contient aucune station, les utilisateurs sont dans Central-Tech/tmp/utilisateurs_seuls.csv\n");
        }
    	fclose(fichier2);
    }
    return avl;
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