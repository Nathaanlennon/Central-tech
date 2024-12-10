#!/bin/bash

param_present=0

h () {
    echo "L'ordre des paramètre est :
    1 : Chemin du fichier ( Obligatoire )
    2 : Type de station à traiter ( Obligatoire )
        Valeurs possibles :
                - hvb ( high-voltage B )
                - hva ( high-voltage A )
                - lv  ( low-volatge )
    3 : Type de consommateur ( Obligatoire )
        Valeurs possibles :
                - comp  ( entreprises )
                - indiv ( particuliers )
                - all   ( tous )
        ATTENTION : Options suivantes interdites car les stations HV-A et HV-B sont exclusives aux entreprises :
                - hbv all
                - hvb indiv
                - hva all
                - hva indiv
    4 : Identifiant de centrale ( Optionnel ) :
                -> Filtre les résultats pour une centrale spécifique 
            "
}

for arg in "$@"; do
    if [ "$arg" = "-h" ]; then
        h
        exit
        break
    fi
done

if [ -z $1 ]; then
    echo "Erreur : le chemin n'est pas donné"
    h 
    exit
fi

if [[ $2 != "hvb" && $2 != "hva" && $2 != "lv" ]]; then
    echo "Erreur : Type de station invalide"
    h
    exit 1
fi

if [[ $3 != "comp" && $3 != "indiv" && $3 != "all" ]]; then
    echo "Erreur : Type de consommateurs invalide"
    h
    exit 1
fi

if [[ ( $2 == "hva" || $2 == "hvb" )&& ( $3 == "indiv" || $3 == "all" ) ]]; then
    echo "Erreur : Option choisie impossible"
    h
    exit
fi

if ! [ -x main ]; then 
    echo "L'executable C n'existe pas, compilation en cours..."
    gcc -o main main.c
    if [ $? -ne 0 ]; then
        echo "Erreur : Échec de la compilation de l'executable"
        exit 1
    else
        echo "Compilation réussie."
    fi
fi

echo "Exécution du programme..."
./main "$@"


if [ -d tmp ]; then
    echo "Le dossier tmp existe"
    if [ "$(ls -A tmp)" ]; then
        echo "rénitialisation du dossier..."
        rm -r tmp/*
    else
        echo "le dossier est déjà vide"
    fi
else
    echo "Création du dossier tmp..."
    mkdir tmp
fi

if [ -d graph ]; then
    echo "Le dossier graph existe"
    if [ "$(ls -A graph)" ]; then
        echo "rénitialisation du dossier..."
        rm -r graph/*
    else
        echo "le dossier est déjà vide"
    fi
else
    echo "Création du dossier graph..."
    mkdir graph
fi