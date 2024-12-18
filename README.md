# Central-tech

Make sure to compile everything like this :

For windows : 

Use the makefile with the following commands : `make` to create the executable
    `.\exec` to run the program
    `make clean` to delete temporary files

 In the event of an error, you can use the classic compilation command
    `gcc main.c lib/utils.c lib/map.c lib/tile.c lib/game.c -o my_program`

Check file paths if you encounter any error.

For linux :

Use the makefile with the following commands : `make` to create the executable
     `./exec` to run the program
     `make clean` to delete temporary files

 In the event of an error, you can use the classic compilation command
    `gcc -o my_program main.c lib/utils.c lib/map.c lib/tile.c lib/game.c`


It doesn't need any special extention except for a C code runner and all the files contained in lib's folder, include's folder and main.c.
