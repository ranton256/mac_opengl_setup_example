//
//  main.cpp
//  mac_opengl_setup_example
//
//  Created by Richard Anton on 1/7/23.
//
//
//  main.cpp
//  opengl_setup_example
//
//  Created by Richard Anton on 12/12/22.
//

#include <iostream>

// NOTE: not entirely portable, see note about getcwd()
#ifndef _WIN32
#include <unistd.h>
#endif

// Using this for std::filesystem::current_path() which requires C++17
#include <filesystem>

// CODE_SIGN_IDENTITY gets set to '-' this refuses to launch...

// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "shaders.h"

const char* kVertexShaderPath = "shaders/vertex_shader.glsl";
const char* kFragmentShaderPath = "shaders/fragment_shader.glsl";

int main(int argc, const char** argv)
{
    const GLint  kWindowWidth = 1024;
    const GLint kWindowHeight = 768;

    std::cout << "GL setup example\n";

    // get and print current working directory for diagnostic purposes.
    // There is no portable way to do this before C++17.
    // This way works on Linux and MacOS X but not Windows
    // on Windows calling _getcwd() instead (with underscore) should work.
    // if you can use C++17 then the std::filesystem support can do this.
#ifndef _WIN32
    constexpr int kDirBufSize = 512;
    char buffer[kDirBufSize];
    char *wd = getcwd(buffer, sizeof(buffer));
    if (wd) {
        std::cout << "getcwd: Current working directory is: " << wd << std::endl;
    }
#else
    std::string s_cwd;
    s_cwd = std::filesystem::current_path();
    
    std::cout << "C++ Current working directory is: " << s_cwd << std::endl;
#endif
    
    if (!glfwInit())
        return 1;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    
    // Required to work on Mac.
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    
    // Disallow resizing.
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE);
    
    GLFWwindow *window = glfwCreateWindow(kWindowWidth, kWindowHeight, "OpenGL Setup Example", nullptr, nullptr);
    
    // may differ than expected due to Retina display
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if( window == nullptr ) {
        std::cerr << "Failed creating window with GLFW" << std::endl;
        getchar(); // wait on keypress
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );
    
    
    glClearColor((GLfloat)0.5f, (GLfloat)0.5f, (GLfloat)0.5f, (GLfloat)1.0f);

    GLuint program;
    if(!InitShader(program, kVertexShaderPath, kFragmentShaderPath)) {
        std::cerr << "Could not initialize shaders" << std::endl;
        return 1;
    }

    const float fovyInDegrees = 45.0f;
    const float aspectRatio = (float) screenWidth / (float)screenHeight;
    const float znear = 0.001f;
    const float zfar = 100.0f;

    glm::mat4 Projection = glm::perspective(glm::radians(fovyInDegrees), aspectRatio, znear, zfar);
    
    glm::mat4 View = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f), // target
        glm::vec3(0.0f, 1.0f, 0.0f) //  up vector
        );

    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(0.6f, 0.5f, 0.5f));
    
    glm::mat4 mvMatrix = View * Model;
    glm::mat4 projMatrix = Projection;
    
    GLint mvMatrixLocation = glGetUniformLocation(program, "mvMatrix");
    GLint projMatrixLocation = glGetUniformLocation(program, "projMatrix");
    
    GLint triColorLocation = glGetUniformLocation(program, "tri_color");
    if (triColorLocation == -1) {
        std::cerr << "Could not bind attribute tri_color" << std::endl;
    }

    GLint aPositionLocation = glGetAttribLocation(program, "a_position");
    if (aPositionLocation == -1) {
        std::cerr << "Could not bind attribute a_position" << std::endl;
    }
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    static const GLfloat g_vertex_buffer_data[] = {
       -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f,  1.0f, 0.0f,
    };
    
    GLuint vao, vertexbuffer;
    glGenVertexArrays(1, &vao);
    
    glGenBuffers(1, &vertexbuffer);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    
    float angle = 0.0f;
    const float kRotSpeed = 0.01;
    
    while( !glfwWindowShouldClose(window)) {
        
        glClear( GL_COLOR_BUFFER_BIT );
        glClear (GL_DEPTH_BUFFER_BIT );
        
        glUseProgram(program);
        
        glUniform4f(triColorLocation, (GLfloat)0.7f, (GLfloat)0.0f, (GLfloat)0.5f, (GLfloat)1.0f);

        
        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::scale(Model, glm::vec3(0.6f, 0.5f, 0.5f));
        Model = glm::rotate(Model, angle, glm::vec3(0, 0.2, 1));
        
        glm::mat4 mvMatrix = View * Model;
        glm::mat4 projMatrix = Projection;
        
        glUniformMatrix4fv(mvMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
        glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projMatrix));
        
        glEnableVertexAttribArray(aPositionLocation);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
           0,
           3,
           GL_FLOAT,
           GL_FALSE,
           0,
           (void*)0
        );
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        angle += kRotSpeed;
    }
    
    glfwTerminate();
    std::cout << "GL setup exiting cleanly\n";
    return 0;
}
