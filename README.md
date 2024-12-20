# Central-tech
//*
Veillez à tout compiler comme ce qui suit :

Pour Windows :

Utilisez le fichier makefile avec les commandes suivantes : make pour créer l'exécutable `.\exec` pour lancer le programme make clean pour supprimer les fichiers temporaires.

En cas d'erreur, vous pouvez utiliser la commande de compilation classique `gcc main.c lib/utils.c lib/map.c lib/tile.c lib/game.c -o mon_programme`

Vérifiez les chemins d'accès aux fichiers si vous rencontrez une erreur.

Pour linux :

Utilisez le fichier makefile avec les commandes suivantes : make pour créer l'exécutable `./exec` pour lancer le programme make clean pour supprimer les fichiers temporaires.

En cas d'erreur, vous pouvez utiliser la commande de compilation classique `gcc -o mon_programme main.c lib/utils.c lib/map.c lib/tile.c lib/game.c`

Il ne nécessite aucune extension particulière si ce n'est un # runner de code C et tous les fichiers contenus dans les dossiers lib, include et main.c.
*//

L'objectif principal du projet C-Wire est de développer un programme qui permet d'analyser les données d'un réseau de distribution d'électricité. Le réseau est organisé sous la forme d'un arbre plus exactement un AVL (arbre de recherche automatiquement équilibré), avec différents niveaux allant des centrales électriques (en raçines) aux consommateurs finaux (les fils).
Le projet est divisé en deux parties : un script Shell pour filtrer et automatiser les tâches, ainsi qu'un script C pour effectuer les calculs nécessaires.

