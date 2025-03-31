#pragma once

#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <vector>
#include "pv112_application.hpp"
#include "geometry.hpp"

// ----------------------------------------------------------------------------
// UNIFORM STRUCTS
// ----------------------------------------------------------------------------

constexpr float PI = 3.14159265358979;

struct CameraUBO {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec4 position;
};

struct LightUBO {
    glm::vec4 position;
    glm::vec4 ambient_color;
    glm::vec4 diffuse_color;
    glm::vec4 specular_color;
};

struct ObjectUBO {
    glm::mat4 model_matrix;  // [  0 -  64) bytes
    glm::vec4 ambient_color; // [ 64 -  80) bytes
    glm::vec4 diffuse_color; // [ 80 -  96) bytes
    // Contains shininess in .w element
    glm::vec4 specular_color; // [ 96 - 112) bytes
};

GLuint load_texture_2d(const std::filesystem::path filename);

GLuint load_cubemap_texture(const std::filesystem::path filepath);

static constexpr glm::vec3 x_axis = glm::vec3(1.f, 0.f, 0.f);
static constexpr glm::vec3 y_axis = glm::vec3(0.f, 1.f, 0.f);
static constexpr glm::vec3 z_axis = glm::vec3(0.f, 0.f, 1.f);

glm::mat4 create_model_matrix(
    float tx = 0, float ty = 0, float tz = 0,
    float rotx = 0, float roty = 0, float rotz = 0,
    float sx = 1, float sy = 1, float sz = 1);

struct MyObject {
	std::shared_ptr<Geometry> geometry;
    std::vector<ObjectUBO> instances;
    GLuint texture = 0;
    bool model_coord_for_texture = false;
    bool procedural_texture = false;
    bool reflects_skybox = false;
    GLuint object_buffer = -1;

    explicit MyObject(std::shared_ptr<Geometry> ptr) : geometry(ptr) {}

    MyObject (std::shared_ptr<Geometry> ptr, glm::mat4 model_matrix,
        glm::vec4 ambient=glm::vec4(), glm::vec4 diffuse = glm::vec4(), glm::vec4 specular = glm::vec4())
        : geometry(ptr), instances(1, {model_matrix, ambient, diffuse, specular}) {}

    void addInstance(const glm::mat4 model_matrix);

    void init_buffers();

    void update_buffers();

    void draw() const;

    int get_texture_flags() const;
};

Geometry from_file_extended(std::filesystem::path path, std::vector<int> which_shapes = {});

#endif