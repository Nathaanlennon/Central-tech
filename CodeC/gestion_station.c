//
// Created by natha on 19/12/2024.
//

#include "gestion_station.h"
#include "macro.h"

//Permet d'obtenir le type de la station que l'on crée et le retourne sous la forme d'un int, voir enum type_station
unsigned int get_station_type(unsigned long chaine[8]) {
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
    unsigned int type = get_station_type(chaine);
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
