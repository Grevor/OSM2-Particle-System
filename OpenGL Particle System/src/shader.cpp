#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.hpp"

bool readFile(const char * file, string* dest) {
	//std::string fileContent;
	std::ifstream fileStream(file, std::ios::in);
	cout << "Opening file: " << file << endl;
	if(fileStream.is_open()){
		cout << "Successfully opened file" << endl;
		std::string line = "";
		while(getline(fileStream, line))
			*dest += "\n" + line;
		fileStream.close();
	}
	else {
		//fileStream.close();
		cout << "Impossible to open: " << file << ". Are you in the right directory?\n";
		//getchar();
		return false;
	}
	return true;
}

GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	cout << "Creating shaders..." << endl;
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

/*	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	cout << "Reading vertex shader: " << vertex_file_path << endl;
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		cout << "Impossible to open: " << vertex_file_path << ". Are you in the right directory ? Don't forget to read the FAQ !\n";
		getchar();
		return 0;
	}*/

	string vertexShaderCode;
	if (!readFile(vertex_file_path, &vertexShaderCode)) {
		cout << "Failed to load vertex shader." << endl;
		return 0;
	}

/*	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	cout << "Reading fragment shader: " << fragment_file_path << endl;
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		cout << "Impossible to open: " << fragment_file_path << ". Are you in the right directory ? Don't forget to read the FAQ !\n";
		getchar();
		return 0;
	}*/

	string fragmentShaderCode;
	if (!readFile(fragment_file_path, &fragmentShaderCode)) {
		cout << "Failed to load fragment shader." << endl;
		return 0;
	}

	GLint isCompiled = GL_FALSE;
	int InfoLogLength;



	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		std::cout << "Compilation failed:" << endl;
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			std::cout << &VertexShaderErrorMessage[0] << endl;
		}
	}
	else {
		std::cout << "Compilation successful." << endl << endl;
	}


	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		std::cout << "Compilation failed:" << endl;
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			std::cout << &FragmentShaderErrorMessage[0] << endl;
		}
	}
	else {
		std::cout << "Compilation successful." << endl << endl;
	}


	// Link the program
	cout << "Linking program\n";
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		std::cout << "Linking failed:" << endl;
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			cout << &ProgramErrorMessage[0] << endl;
		}
	}
	else {
		std::cout << "Linking successful." << endl;
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


