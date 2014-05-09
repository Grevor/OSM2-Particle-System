standard:
	g++ -std=c++11 -lrt -Wall -ggdb -o standard.exe main.cpp
	
standardgl:
	g++ -Wall -ggdb -DGLFW_STATIC -o standardGL.exe standardGL.cpp -lglfw3 -lgdi32 -lglu32 -lopengl32 -std=c++11

lstandardgl: 
	g++ -o standardGL.exe standardGL.cpp -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -std=c++11
#-llibboost-timer.a -llibboost-chrono.a <- flags for boost libraries