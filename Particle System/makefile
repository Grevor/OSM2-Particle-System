COMPILER=g++
SOURCES=demo.cpp #fire/main.cpp
SOURCES_FULL=$(addprefix $(SRC_FOLDER)/, $(SOURCES))
OBJ=$(addprefix $(OBJ_FOLDER)/, $(patsubst %.cpp, %.o, $(SOURCES)))
DEP=$(addprefix $(OBJ_FOLDER)/, $(patsubst %.cpp, %.d, $(SOURCES)))
RM_COMMAND=rm -f
FLAGS=-ggdb -Wall -std=c++11
OBJ_FOLDER=obj
SRC_FOLDER=src
MKDIR_COMMAND= mkdir -p

demo: depend $(OBJ)
	$(COMPILER) $(FLAGS) $(SRC_FOLDER)/demo.cpp -o "demo.exe" 
	
fire: depend $(OBJ)
	$(COMPILER) $(FLAGS) $(SRC_FOLDER)/fire/main.cpp -o "fire.exe" 

depend: $(SOURCE)
	$(RM_COMMAND) ./depend
	$(COMPILER) $(FLAGS) -MM $^>>./depend
	echo rebuilt dependencies
	
#Forces a dependency build, and invokes the real build afterward with the newly-built dependencies.	
$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.cpp
	$(MKDIR_COMMAND) $(OBJ_FOLDER)
	$(COMPILER) $(FLAGS) -c -MMD -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" $<
	
clean:
	$(RM_COMMAND) $(OBJ)
	$(RM_COMMAND) $(DEP)

include ./depend