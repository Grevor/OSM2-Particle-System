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

#This command will only work under cygwin or (possibly)UNIX.
testParticleSystem: depend $(OBJ)
	$(COMPILER) $(FLAGS) test/test.cpp -o "test.exe" 
	
particleEngine:
	$(MKDIR_COMMAND) $(OBJ_FOLDER)
	$(COMPILER) -c $(SRC_FOLDER)/ParticleEngine.cpp -o $(OBJ_FOLDER)/particleEngine.o "C:/MinGW/lib/libboost_thread-vc110-mt-1_55.lib" $(FLAGS)
	
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