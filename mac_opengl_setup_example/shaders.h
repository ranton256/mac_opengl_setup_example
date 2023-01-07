//
//  shaders.hpp
//  opengl_setup_example
//
//  Created by Richard Anton on 12/13/22.
//

#ifndef shaders_hpp
#define shaders_hpp

#include <GL/glew.h>

bool InitShader(GLuint& outProgram, const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);

#endif /* shaders_hpp */
