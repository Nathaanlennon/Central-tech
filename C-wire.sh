#!/bin/bash

param_present=0
#fonction d'aide
h () {
    echo -e "
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
        \033[31mATTENTION : Options suivantes interdites car les stations HV-A et HV-B sont exclusives aux entreprises :\033[0m
                - hbv all
                - hvb indiv
                - hva all
                - hva indiv
    4 : Identifiant de centrale ( Optionnel ) :
                -> Filtre les résultats pour une centrale spécifique 
    
    \033[32mExemple : bash C-wire.sh c-wire_v00.dat hvb comp 1\033[0m
    "
       
}
#vérifie si la fonction d'aide est appelée
for arg in "$@"; do
    if [ "$arg" = "-h" ]; then
        h
        exit
        break
    fi
done
#vérifie que le chemin est donnée
if [ ! -f $1 ]; then
    echo -e "\033[31m Erreur : le chemin n'est pas donné \033[0m"
    echo "durée du traitement : 0 seconde"
    h 
    exit
fi
#vérifie que le type de station est valide
if [[ $2 != "hvb" && $2 != "hva" && $2 != "lv" ]]; then
    echo -e "\033[31mErreur : Type de station invalide\033[0m"
    echo "durée du traitement : 0 seconde"
    h
    exit 1
fi
#vérifie que le type de consommateur est valide
if [[ $3 != "comp" && $3 != "indiv" && $3 != "all" ]]; then
    echo -e "\033[31mErreur : Type de consommateurs invalide\033[0m"
    echo "durée du traitement : 0 seconde"
    h
    exit 1
fi
#vérifie que l'option choisie est possible
if [[ ( $2 == "hva" || $2 == "hvb" )&& ( $3 == "indiv" || $3 == "all" ) ]]; then
    echo -e "\033[31mErreur : Option choisie impossible\033[0m"
    echo "durée du traitement : 0 seconde"
    h
    exit
fi

#initialisation de la variable
typestation=0

#accorde la variable avec la colonne du type de station appelé et crée le fichier nécessaire si l'utilisateur a utilise "hva" ou "hvb" en type de station
if [ $2 == "hvb" ]; then
	typestation=2;
    touch hvb_comp.cvs
elif [ $2 == "hva" ]; then
	typestation=3;
    touch hva_comp.csv
elif [ $2 == "lv" ]; then
	typestation=4;
fi

#accorde la variable typeconso avec la colonne du type de consomatteur appelé ( met à 0 pour all car on en aura besoin pour une vérification) et crée le fichier nécessaire si l'utilisateur a choisis "lv" en type de station
if [ $3 == "all" ]; then
	typeconso=0;
    touch lv_all.cvs
elif [ $3 == "comp" ]; then
    if [ "$typestation"=="4" ];then
        touch lv_comp.cvs
    fi
	typeconso=5;
elif [ $3 == "indiv" ]; then
	typeconso=6;
    if [ "$typestation"=="4" ];then
        touch lv_indiv.cvs
    fi
fi 

#vérifie si un identifiant de centrale est appelé
if [ -z $4 ]; then
    centrale=0;
else 
    #vérifie que le numéro de centrale est un entier
    if [[ $4 == +([0-9]) ]]; then
        #vérifie que la centrale existe ( c'est à dire est incluse dans l'intervalle de l'identifiant de la 1ere et de la derniere centrale inclus )
        premierElem=$(awk -F';' 'NR==2 {print $1}' "$1")
        dernierElem=$(awk -F';' 'END {print $1}' "$1")
        if [ "$4" -lt "$premierElem" ] || [ "$4" -gt "$dernierElem" ]; then
            echo -e "\033[31mCentrale non existante\033[0m"
            h
            exit
        else
            centrale=1;
            echo "$centrale"
        fi
    else 
        echo -e "\033[31mLe numéro de centrale doit être un entier\033[0m"
        h
        exit
    fi
fi



#vérifie si le dossier tmp existe, le créé sinon
if [ -d tmp ]; then
    echo "Le dossier tmp existe"
    #vérifie si le dossier est vide ou pas
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

#vérifie si le dossier graph existe, le crée sinon
if [ -d graph ]; then
    echo "Le dossier graph existe"
    #vérifie si graph est vide
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
Avant=$SECONDS

#supprime la 1ere ligne de texte pour avoir seulement les données dans le fichier
sed -i '1d' $1
#vérifie si un identifiant de centrale a été entrer 
if [ "$centrale" -eq 0 ]; then 
    #filtre les données pour n'avoir que les données nécessaire si l'utilisateur a entrer "lv all"
    if [ "$3" == "all" ]; then
	    awk -F';' -v col="$typestation" '$col != "-" {print $0}' $1 > tmp/temp2.dat
    #filtre pour les autres cas
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
    }' tmp/temp.dat > tmp/temp2.dat
    fi
else 
    #filtre selon la centrale
    awk -F';' -v idcentrale="$4" '$1 == idcentrale {print $0}' "$1" > tmp/temp3.dat
        #filtre les données pour n'avoir que les données nécessaire si l'utilisateur a entrer "lv all"
        if [ ""$3 == "all" ]; then
	        awk -F';' -v col="$typestation" '$col != "-" {print $0}' tmp/temp3.dat > tmp/temp2.dat
        #filtre pour les autres cas 
        else
	        awk -F';' -v col="$typestation" '$col != "-" {print $0}' tmp/temp3.dat > tmp/temp.dat
    	
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
        }' tmp/temp.dat > tmp/temp2.dat
    fi
fi
tr '-' '0' < tmp/temp2.dat > tmp/input.dat

Intervalle=$(($SECONDS - $Avant))
echo "Durée du filtrage : $Intervalle secondes"

#vérifie si l'executable existe
if ! [ -x Central_tech ]; then 
    echo "L'executable C n'existe pas, compilation en cours..."
    make build
    #vérifie que la dernière commande ( càd la compilation ) a fonctionné 
    if [ $? -ne 0 ]; then
        echo -e "\033[31mErreur : Échec de la compilation de l'executable\033[0m"
        exit 1
    else
        echo "Compilation réussie."
    fi
fi

echo "Exécution du programme..."
make run