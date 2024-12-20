#!/bin/bash
Avant=$SECONDS
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

#verifie si le dossier test existe, le crée sinon
if [ -d tests ]; then
    echo "Le dossier tests existe"
else
    echo "Création du dossier tests..."
    mkdir tests
fi

#deplace les anciens resultats dans tests
if [ -e tmp/stockNom ];then
    current_date=$(date +"%Y%m%d%H%M%S")
    fichier=$(cat tmp/stockNom)
    base_fichier=$(basename "$fichier" .csv)
    ancienTest=$base_fichier"_"$current_date
    mv $fichier tests/$ancienTest.csv
fi

#deplace les anciens resultats dans tests pour "lv_minmax"
if [ -e tmp/stockNom2 ];then
    current_date=$(date +"%Y%m%d%H%M%S")
    fichier=$(cat tmp/stockNom2)
    base_fichier=$(basename "$fichier" .csv)
    ancienTest=$base_fichier"_"$current_date
    mv $fichier tests/$ancienTest.csv
fi


#vérifie si la fonction d'aide est appelée
for arg in "$@"; do
    if [ "$arg" = "-h" ]; then
        h
        exit 1
        break
    fi
done

#vérifie que le fichier est donnée
if [ ! -f $1 ]; then
    echo -e "\033[31m Erreur : le fichier n'est pas donné \033[0m"
    echo "durée du traitement : 0.0 seconde"
    h
    exit 2
fi
#vérifie que le type de station est valide
if [[ $2 != "hvb" && $2 != "hva" && $2 != "lv" ]]; then
    echo -e "\033[31mErreur : Type de station invalide\033[0m"
    echo "durée du traitement : 0.0 seconde"
    h
    exit 3
fi
#vérifie que le type de consommateur est valide
if [[ $3 != "comp" && $3 != "indiv" && $3 != "all" ]]; then
    echo -e "\033[31mErreur : Type de consommateurs invalide\033[0m"
    echo "durée du traitement : 0.0 seconde"
    h
    exit 4
fi
#vérifie que l'option choisie est possible
if [[ ( $2 == "hva" || $2 == "hvb" )&& ( $3 == "indiv" || $3 == "all" ) ]]; then
    echo -e "\033[31mErreur : Option choisie impossible\033[0m"
    echo "durée du traitement : 0.0 seconde"
    h
    exit 5
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

#vérifie si le dossier graphs existe, le crée sinon
if [ -d graphs ]; then
    echo "Le dossier graph existe"
    #vérifie si graphs est vide
    if [ "$(ls -A graphs)" ]; then
        echo "rénitialisation du dossier..."
        rm -r graphs/*
    else
        echo "le dossier est déjà vide"
    fi
else
    echo "Création du dossier graphs..."
    mkdir graphs
fi


#initialisation de la variable
typestation=0



#accorde la variable typeconso avec la colonne du type de consomatteur appelé ( met à 0 pour all car on en aura besoin pour une vérification)
if [ $3 == "all" ]; then
	typeconso=0;
elif [ $3 == "comp" ]; then
	typeconso=5;
elif [ $3 == "indiv" ]; then
	typeconso=6;
fi

nomCentrale=""
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
            echo "durée du traitement : 0.0 seconde"
            h
            exit 6
        else
            centrale=1;
            nomCentrale="_$4"
        fi
    else
        echo -e "\033[31mLe numéro de centrale doit être un entier\033[0m"
        echo "durée du traitement : 0.0 seconde"
        h
        exit 7
    fi
fi



#accorde la variable avec la colonne du type de station appelé et crée le fichier nécessaire si l'utilisateur a utilise "hva", "hvb" ou "all" en type de station
if [ $2 == "hvb" ]; then
	typestation=2;
    touch hvb_comp$nomCentrale.csv
    echo "Station HV-B:Capacité:Consommation (entreprises)" > hvb_comp$nomCentrale.csv
    touch tmp/stockNom
    echo "hvb_comp$nomCentrale.csv" > tmp/stockNom
elif [ $2 == "hva" ]; then
	typestation=3;
    touch hva_comp$nomCentrale.csv
    echo "Station HV-A:Capacité:Consommation (entreprises)" > hva_comp$nomCentrale.csv
    touch tmp/stockNom
    echo "hva_comp$nomCentrale.csv" > tmp/stockNom
elif [ $2 == "lv" ]; then
	typestation=4;
    if [ $3 == "comp" ]; then
        touch lv_comp$nomCentrale.csv
        echo "Station LV:Capacité:Consommation (entreprises)" > lv_comp$nomCentrale.csv
        touch tmp/stockNom
        echo "lv_comp$nomCentrale.csv" > tmp/stockNom
    elif [ $3 == "indiv" ]; then
        touch lv_indiv$nomCentrale.csv
        echo "Station LV:Capacité:Consommation (particuliers)" > lv_indiv$nomCentrale.csv
        touch tmp/stockNom
        echo "lv_indiv$nomCentrale.csv" > tmp/stockNom
    elif [ $3 == "all" ]; then
        touch lv_all$nomCentrale.csv
        echo "Station LV:Capacité:Consommation (tous)" > lv_all$nomCentrale.csv
        touch lv_all_minmax$nomCentrale.csv
        echo "Min and Max 'capacity-load' extreme nodes" > lv_all_minmax$nomCentrale.csv
        echo "Station LV:Capacité:Consommation (tous)" > lv_all_minmax$nomCentrale.csv
        touch tmp/stockNom
        echo "lv_all$nomCentrale.csv" > tmp/stockNom
        touch tmp/stockNom2
        echo "lv_all_minmax$nomCentrale.csv" > tmp/stockNom2
    fi
fi




#creation du fichier output
touch tmp/output.csv
echo "Filtrage des données en cours..."




#vérifie si un identifiant de centrale a été entrer
if [ "$centrale" -eq 0 ]; then
    #filtre les données pour n'avoir que les données nécessaire si l'utilisateur a entrer "lv all"
    if [ "$3" == "all" ]; then
	    awk -F';' -v col="$typestation" '$col != "-" {print $0}' $1 > tmp/temp2.dat
        #supprime la 1ere ligne du fichier pour lever le texte
        sed -i "1d" tmp/temp2.dat
    #filtre pour les autres cas
    else
	    awk -F';' -v col="$typestation" '$col != "-" {print $0}' $1 > tmp/temp.dat

        awk -F';' -v col="$typeconso" -v col2="$typestation" '
    #récupere les stations de base avec leur capacite
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
    #supprime la 1ere ligne du fichier pour lever le texte
    sed -i "1d" tmp/temp2.dat
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
        #récupere les stations de base avec leur capacite
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
#transforme les tiret en 0 pour faciliter la tache a la partie C
tr '-' '0' < tmp/temp2.dat > tmp/input.dat

Intervalle=$(($SECONDS - $Avant))
echo "durée du filtrage : $Intervalle"

#verifie si le repertoire CodeC existe
if [ -d CodeC ]; then
    echo "CodeC existe"
else
    echo -e "\033[31mErreur : Répertoire CodeC non trouvé\033[0m"
    echo "Durée du script : 0 secondes"
    exit 8
fi


#vérifie si l'executable existe
cd CodeC
if ! [ -x Central_tech ]; then
    echo "L'executable C n'existe pas, compilation en cours..."
    make build
    #vérifie que la dernière commande ( càd la compilation ) a fonctionné
    if [ $? -ne 0 ]; then
        echo -e "\033[31mErreur : Échec de la compilation de l'executable\033[0m"
        echo "Durée du script : 0 secondes"
        exit 9
    else
        echo "Compilation réussie."
    fi
fi

Avant2=$SECONDS
echo "Exécution du programme..."
make run
cd ..

Intervalle2=$(($SECONDS - $Avant2))
echo "Durée de l'executable : $Intervalle2 secondes"

#reviens dans le repertoire initial
cd .

#Trie le output selon la consommation et crée le fichier final
if [ -e hva_comp$nomCentrale.csv ];then
    touch tmp/tri.csv
    sort -t':' -k3 -n tmp/output.csv > tmp/tri.csv
    awk '{print $0}' tmp/tri.csv  >> hva_comp$nomCentrale.csv
fi
if [ -e hvb_comp$nomCentrale.csv ];then
    touch tmp/tri.csv
    sort -t':' -k3 -n tmp/output.csv > tmp/tri.csv
    awk '{print $0}' tmp/tri.csv  >> hvb_comp$nomCentrale.csv
fi
if [ -e lv_all$nomCentrale.csv ];then
    touch tmp/tri.csv
    sort -t':' -k3 -n tmp/output.csv > tmp/tri.csv
    awk '{print $0}' tmp/tri.csv  >> lv_all$nomCentrale.csv
    touch tmp/lv_all_mm$nomCentrale.csv
    head tmp/tri.csv > tmp/lv_all_mm$nomCentrale.csv
    tail tmp/tri.csv >> tmp/lv_all_mm$nomCentrale.csv
    touch tmp/donnees_graph.csv
    awk -F':' '{if ($2 != 0) print $0 ":" $2-$3}' tmp/lv_all_mm$nomCentrale.csv | sort -t ':' -k4 -n > tmp/donnees_graph.csv
    echo "Min and Max 'capacity-load' extreme nodes" > lv_all_minmax$nomCentrale.csv
    echo "Station LV:Capacité:Consommation (tous)" >> lv_all_minmax$nomCentrale.csv
    cut -d':' -f1-3 tmp/donnees_graph.csv >> lv_all_minmax$nomCentrale.csv
fi
if [ -e lv_indiv$nomCentrale.csv ];then
    touch tmp/tri.csv
    sort -t':' -k3 -n tmp/output.csv > tmp/tri.csv
    awk '{print $0}' tmp/tri.csv  >> lv_indiv$nomCentrale.csv
fi
if [ -e lv_comp$nomCentrale.csv ];then
    touch tmp/tri.csv
    sort -t':' -k3 -n tmp/output.csv > tmp/tri.csv
    awk '{print $0}' tmp/tri.csv  >> lv_comp$nomCentrale.csv
fi

TempsFinal=$(($Intervalle + $Intervalle2))

echo "Durée du script total : $TempsFinal secondes"