#include "ShaderStructs.h"
#include "../Core/ShaderManager.h"
#include <iostream>
#include <fstream>

std::string readText(std::string path) {
    std::ifstream file(path);
    std::string str;
    std::string line;
    while (std::getline(file, line)) {
        str += line + "\n";
    }
    return str;
}

int checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    return success;
}

void Shader::load(std::string vertPath, std::string fragPath) {
    std::string vertSource = readText("res/Shaders/" + vertPath);
    std::string fragSource = readText("res/Shaders/" + fragPath);
	
    const char* vShader = vertSource.c_str();
    const char* fShader = fragSource.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShader, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShader, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glLinkProgram(tempID);

    if (checkCompileErrors(tempID, "PROGRAM")) {
        if (m_ID == -1) {
            glDeleteProgram(m_ID);
        }
        
        m_ID = tempID;

        assignUniform("model");
        assignUniform("view");
        assignUniform("projection");        
    }
    else {
        std::cout << "Shader failed to compile: " << vertPath << " " << fragPath <<".\n";
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
};

void Shader::use() {
    glUseProgram(m_ID);
};

void Shader::assignUniform(std::string name) {
    int location = glGetUniformLocation(m_ID, name.c_str());
    if (location == -1) {
        std::cout << "Uniform '" << name << "' not found in shader " << m_ID << ". Excluded from uniform list.\n";
        return;
    }
    uniforms[name] = location;
}

void Shader::updateInt(std::string uniName, int num) {
    glUniform1i(uniforms[uniName], num);
};

void Shader::updateMat4(std::string uniName, glm::mat4 updateVal) {
    glUniformMatrix4fv(uniforms[uniName], 1, GL_FALSE, glm::value_ptr(updateVal));
}; 

void Shader::updateVec3(std::string uniName, glm::vec3 updateVal) {
    glUniform3fv(uniforms[uniName], 1, glm::value_ptr(updateVal));
};  

void Shader::updateVec2(std::string uniName, glm::vec2 updateVal) {
    glUniform2fv(uniforms[uniName], 1, glm::value_ptr(updateVal));
};

void Shader::updateBool(std::string uniName, bool updateVal) {
    glUniform1i(uniforms[uniName], updateVal);
};