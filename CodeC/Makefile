# Nom du programme
TARGET = Central_tech

# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -g

# Liste des fichiers source
SOURCES = main.c gestion_avl.c gestion_station.c traitement.c

# Liste des fichiers objets correspondant aux fichiers source
OBJECTS = $(SOURCES:.c=.o)

# Règle par défaut
all: $(TARGET)



# Règle de compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -Iinclude     -c $< -o $@

# Règle de construction du programme
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -Iinclude  $(OBJECTS) -o $@

# Règle de nettoyage des fichiers objets et du programme
clean:
	rm -f $(OBJECTS) $(TARGET)
#poubiou
.PHONY: build
build: $(TARGET)

.PHONY: run
run: $(TARGET)
	./Central_tech