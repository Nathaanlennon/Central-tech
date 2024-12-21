# Central-tech

Veillez à tout compiler comme ce qui suit :

Arguments : 

1/ fichier d'entrée 

2/ type de station 

3/ type de consommation 

4/ identifiant de la central éléctrique

`./c-wire.sh Input/[fichier d'entrée] [type_station, hvb, hva, lv] [type_consommation(indiv, comp, all] (id de la central electrique) `

exemple : 

`bash C-wire.sh c-wire_v00.dat hvb comp 1`



L'objectif principal du projet C-Wire est de développer un programme qui permet d'analyser les 
données d'un réseau de distribution d'électricité. Le réseau est 
organisé sous la forme d'un arbre plus exactement un AVL (arbre de recherche automatiquement 
équilibré), avec différents niveaux allant des centrales électriques
(en raçines) aux consommateurs finaux (les fils).
Le projet est divisé en deux parties : un script Shell pour filtrer et automatiser les tâches, 
ainsi qu'un script C pour effectuer les calculs nécessaires.


Membre du groupe :

Bias Nathan    MI-03

Cearic Ivan    MI-03

Saidi Narymane MI-03 22401617
