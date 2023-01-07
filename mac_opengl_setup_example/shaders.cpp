//
//  shaders.cpp
//  opengl_setup_example
//
//  Created by Richard Anton on 12/13/22.
//

#include "shaders.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>


using namespace std;

static void logShaderError(GLuint vs) {
    GLint maxLength = 0;
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);
    
    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
    if(!errorLog.empty()) {
        cerr << "\t" << (const GLchar *)errorLog.data() << endl;
    }
}

static std::string LoadShaderFile(const char* shaderPath)
{
    std::string shaderSource;
    std::ifstream shaderFileStream(shaderPath, std::ios::in);
    if(!shaderFileStream.is_open()){
        cerr << "error opening shader source file: " << shaderPath << endl;
        return "";
    }

    
    std::stringstream sstr;
    sstr << shaderFileStream.rdbuf();
    shaderSource = sstr.str();
    shaderFileStream.close();
    
    
    return shaderSource;
}



bool InitShader(GLuint& outProgram, const char* vertexShaderPath, const char* fragmentShaderPath)
{
    string vertexShaderSource = LoadShaderFile(vertexShaderPath);
    string fragmentShaderSource = LoadShaderFile(fragmentShaderPath);
    
    if(vertexShaderSource.empty() || fragmentShaderSource.empty())  {
        return false;
    }
    
    GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

    const char* vsPtr = vertexShaderSource.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsPtr, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
    if (!compile_ok) {
        cerr << "Error in vertex shader" << endl;
        
        logShaderError(vs);
        glDeleteShader(vs);
    
        return false;
    }
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    
    const char* fsPtr = fragmentShaderSource.c_str();
    glShaderSource(fs, 1, &fsPtr, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
    if (!compile_ok) {
        cerr << "Error in fragment shader" << endl;
        logShaderError(fs);
        
        glDeleteShader(vs);
        glDeleteShader(fs);
    
        return false;
    }

    outProgram = glCreateProgram();
    glAttachShader(outProgram, vs);
    glAttachShader(outProgram, fs);
    glLinkProgram(outProgram);
    glGetProgramiv(outProgram, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        cerr << "Error in glLinkProgram" << endl;
        
        logShaderError(vs);
        
        glDeleteShader(vs);
        glDeleteShader(fs);
    
        return false;
    }
    
    return true;
}

