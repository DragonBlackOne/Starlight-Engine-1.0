// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

static void check_compile_errors(GLuint shader, const char* type) {
    int success;
    char infoLog[1024];
    if (type[0] != 'P') {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERRO::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERRO::PROGRAM_LINKING_ERROR of type: %s\n%s\n", type, infoLog);
        }
    }
}

GLuint shader_create_program(const char* vertex_source, const char* fragment_source) {
    GLuint vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");
    
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    check_compile_errors(ID, "PROGRAM");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return ID;
}

GLuint shader_load_program(const char* vertex_path, const char* fragment_path) {
    // Leitura simplificada para Engine C usando leitura binaria rigorosa
    FILE* vShaderFile = fopen(vertex_path, "rb");
    FILE* fShaderFile = fopen(fragment_path, "rb");
    
    if (!vShaderFile || !fShaderFile) {
        printf("ERRO::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
        if(vShaderFile) fclose(vShaderFile);
        if(fShaderFile) fclose(fShaderFile);
        return 0;
    }
    
    fseek(vShaderFile, 0, SEEK_END);
    long vSize = ftell(vShaderFile);
    fseek(vShaderFile, 0, SEEK_SET);
    
    fseek(fShaderFile, 0, SEEK_END);
    long fSize = ftell(fShaderFile);
    fseek(fShaderFile, 0, SEEK_SET);
    
    if (vSize <= 0 || fSize <= 0) {
        printf("ERRO::SHADER::EMPTY_FILE\n");
        fclose(vShaderFile);
        fclose(fShaderFile);
        return 0;
    }
    
    char* vShaderCode = (char*)malloc(vSize + 1);
    char* fShaderCode = (char*)malloc(fSize + 1);
    
    if (!vShaderCode || !fShaderCode) {
        printf("ERRO::SHADER::MALLOC_FAILED\n");
        if(vShaderCode) free(vShaderCode);
        if(fShaderCode) free(fShaderCode);
        fclose(vShaderFile);
        fclose(fShaderFile);
        return 0;
    }
    
    size_t vRead = fread(vShaderCode, 1, vSize, vShaderFile);
    vShaderCode[vRead] = '\0';
    
    size_t fRead = fread(fShaderCode, 1, fSize, fShaderFile);
    fShaderCode[fRead] = '\0';
    
    fclose(vShaderFile);
    fclose(fShaderFile);
    
    GLuint id = shader_create_program(vShaderCode, fShaderCode);
    
    free(vShaderCode);
    free(fShaderCode);
    
    return id;
}

void shader_delete(GLuint program) {
    glDeleteProgram(program);
}
