# Projeto 1 - Computacao Visual
# Baseado nos makefiles do repositorio CompVis262.

TARGET = projeto1

# Ajustar SDL_DIR para a instalacao local das bibliotecas SDL3.
# O grupo informou que utiliza Windows + GCC + Visual Studio Code.
SDL_DIR = d:\\dev\\compvis\\libs\\SDL3
SDL_INC_DIR = $(SDL_DIR)\\include
SDL_LIB_DIR = $(SDL_DIR)\\lib
SDL_DLL_DIR = $(SDL_DIR)\\bin

SDL_DLL_FILE = SDL3.dll
SDL_IMAGE_DLL_FILE = SDL3_image.dll
SDL_TTF_DLL_FILE = SDL3_ttf.dll

CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic
LDFLAGS = -L$(SDL_LIB_DIR)
LDLIBS = -lSDL3 -lSDL3_image -lSDL3_ttf -lm
INC_DIRS = $(addprefix -I, $(SDL_INC_DIR))

SUBDIR =
INC = $(wildcard *.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRC = $(wildcard *.c $(foreach fd, $(SUBDIR), $(fd)/*.c))
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

clean:
	-del /S *.o
	-del /S $(SDL_DLL_FILE)
	-del /S $(SDL_IMAGE_DLL_FILE)
	-del /S $(SDL_TTF_DLL_FILE)
	-del /S $(TARGET).exe

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INC_DIRS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	copy $(SDL_DLL_DIR)\\$(SDL_DLL_FILE) .\\$(SDL_DLL_FILE)
	copy $(SDL_DLL_DIR)\\$(SDL_IMAGE_DLL_FILE) .\\$(SDL_IMAGE_DLL_FILE)
	copy $(SDL_DLL_DIR)\\$(SDL_TTF_DLL_FILE) .\\$(SDL_TTF_DLL_FILE)

%.o: %.c $(INC)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@
