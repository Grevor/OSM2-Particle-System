#------------------------------------------------------------------------------
INCLUDES="-IC:\\Program Files\\glm\\glm-0.9.4.0"
LIBFLAGS=-lglfw3 -lglew32s -lgdi32 -lglu32 -lopengl32
BIN=bin
OBJ=obj
SRC=src
CC=g++
CCFLAGS=-O0 -g3 -Wall -std=c++11 -DGLFW_STATIC
MKDIR_COMMAND= mkdir -p
#------------------------------------------------------------------------------

all: main

main: $(OBJ)/main.o $(OBJ)/shader.o
	$(CC) -o "$(BIN)/OpenGLExperimenting.exe" $(OBJ)/main.o $(OBJ)/shader.o $(LIBFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(MKDIR_COMMAND) $(OBJ)
	$(CC) $(INCLUDES) $(CCFLAGS) -c -o "$@" "$<"

clean:
	rm -f $(OBJ)/%.o