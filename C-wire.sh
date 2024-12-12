#!/bin/bash

param_present=0

h () {
    echo "
    L'ordre des paramètre est :
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
    
    Exemple : bash C-wire.sh c-wire_v00.dat hvb comp 1
    "
       
}

for arg in "$@"; do
    if [ "$arg" = "-h" ]; then
        h
        exit
        break
    fi
done

if [ ! -f $1 ]; then
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

typestation=0

if [ $2 == "hvb" ]; then
	typestation=2;
fi

if [ $2 == "hva" ]; then
	typestation=3;
fi

if [ $2 == "lv" ]; then
	typestation=4;
fi

if [ $3 == "all" ]; then
	typeconso=0;
fi

if [ $3 == "comp" ]; then
	typeconso=5;
fi

if [ $3 == "indiv" ]; then
	typeconso=6;
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

echo "Filtrage des données en cours..."

if [ $3 == all ]; then
	awk -F';' -v col="$typestation" '$col != "-" {print $0}' $1 > tmp/input.dat
else
	awk -F';' -v col="$typestation" '$col != "-" {print $0}' $1 > tmp/temp.dat
    	
    awk -F';' -v col="$typeconso" -v col2="$typestation" '
{
    if (col2 != 4) {
        if ($col != "-" || $(col2 + 1) == "-") {
            print $0
        }
    } else {
        if ($col != "-" || ($5 == "-" && $6 == "-")) {
            print $0
        }
    }
}' tmp/temp.dat > tmp/input.dat

fi






