#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // Shader program ID
    unsigned int ID;

    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);

    // Use the shader
    void use();

private:
    // Utility function for checking shader compilation/linking errors
    void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif // SHADER_HPP
