test:
	g++ -ggdb -Wall -std=c++11 -L/usr/local/lib/ -L/usr/include/boost src/cellist/Cellist.h src/cellist/Cellist.cpp src/Flags.h src/IncCurve.h src/WaterParticleStruct.h src/WaterParticle.h src/WaterParticle.cpp src/StaticParticle.h src/StaticParticle.cpp src/WaterParticleTest.cpp -lboost_thread -lboost_system `pkg-config --static --libs glfw3` -lGLU -lglut -lGL

make sph:
	g++ -std=c++11 src/cellist/Cellist.h src/cellist/Cellist.cpp src/Flags.h src/IncCurve.h src/WaterParticleStruct.h src/WaterParticle.h src/WaterParticle.cpp src/StaticParticle.h src/StaticParticle.cpp src/WaterParticleTest.cpp -lboost_thread -lboost_system `pkg-config --static --libs glfw3` -lGLU -lglut -lGL
