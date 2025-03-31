#include "utils.hpp"
#include "tiny_obj_loader.h"
#include "glm/vec3.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static constexpr float final_scale = 1;

GLuint load_texture_2d(const std::filesystem::path filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.generic_string().data(), &width, &height, &channels, 4);

    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    glTextureStorage2D(texture, std::log2(width), GL_RGBA8, width, height);

    glTextureSubImage2D(texture,
        0,                         //
        0, 0,                      //
        width, height,             //
        GL_RGBA, GL_UNSIGNED_BYTE, //
        data);

    stbi_image_free(data);

    glGenerateTextureMipmap(texture);

    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

GLuint load_cubemap_texture(const std::filesystem::path filepath) {
    GLuint texture;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture);

    std::vector<std::string> v = { "posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg" };
    for (int i = 0; i < 6; i++) {
        int width, height, channels;
        std::filesystem::path filename = filepath / v[i];
        unsigned char* data = stbi_load(filename.generic_string().data(), &width, &height, &channels, 4);

        if (i == 0) {
            glTextureStorage2D(texture, std::log2(width), GL_RGBA8, width, height);
        }
        glTextureSubImage3D(texture, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    glGenerateTextureMipmap(texture);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return texture;
}

glm::mat4 create_model_matrix(float tx, float ty, float tz, float rotx, float roty, float rotz, float sx, float sy, float sz) {
    glm::mat4 translation = glm::translate(glm::vec3(tx, ty, tz) * final_scale);
    glm::mat4 xrotation = glm::rotate(rotx, x_axis);
    glm::mat4 yrotation = glm::rotate(roty, y_axis);
    glm::mat4 zrotation = glm::rotate(rotz, z_axis);
    glm::mat4 scale = glm::scale(glm::vec3(sx, sy, sz) * final_scale);
    return translation * zrotation * yrotation * xrotation * scale;
}

void MyObject::addInstance(const glm::mat4 model_matrix) {
    if (instances.size() > 0) {
        instances.push_back(instances.back());
        instances.back().model_matrix = model_matrix;
    }
    else {
        instances.push_back({ model_matrix });
    }
}

void MyObject::init_buffers() {
    glCreateBuffers(1, &object_buffer);
    update_buffers();
}

void MyObject::update_buffers() {
    glNamedBufferData(object_buffer, sizeof(ObjectUBO) * instances.size(), instances.data(), GL_DYNAMIC_DRAW);
}

void MyObject::draw() const {
    glUniform1i(20, get_texture_flags());
    glBindTextureUnit(10, texture);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, object_buffer);
    geometry->draw_instanced(instances.size());
}

int MyObject::get_texture_flags() const {
    return (int(procedural_texture) << 2) | (int(model_coord_for_texture) << 1) | int(texture > 0);
}

Geometry from_file_extended(std::filesystem::path path, std::vector<int> which_shapes) {
    const std::string extension = path.extension().generic_string();

    if (extension == ".obj") {
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path.generic_string())) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> tex_coords;
        std::cout << "Loaded " << path << " with " << shapes.size() << " shapes\n";
        
        for (int si = 0; si < shapes.size(); si++) {
            if (!which_shapes.empty() && find(which_shapes.begin(), which_shapes.end(), si) == which_shapes.end()) {
                continue;
            }

            const tinyobj::shape_t& shape = shapes[si];
            
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                // Loop over vertices in the face.
                for (size_t v = 0; v < 3; v++) {
                    // Access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

                    tinyobj::real_t nx;
                    tinyobj::real_t ny;
                    tinyobj::real_t nz;
                    if (!attrib.normals.empty()) {
                        nx = attrib.normals[3 * idx.normal_index + 0];
                        ny = attrib.normals[3 * idx.normal_index + 1];
                        nz = attrib.normals[3 * idx.normal_index + 2];
                    }
                    else {
                        nx = 0.0;
                        ny = 0.0;
                        nz = 0.0;
                    }

                    tinyobj::real_t tx;
                    tinyobj::real_t ty;
                    if (!attrib.texcoords.empty() && idx.texcoord_index != -1) {
                        tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                        ty = 1 - attrib.texcoords[2 * idx.texcoord_index + 1];
                    }
                    else {
                        tx = 0.0;
                        ty = 0.0;
                    }

                    positions.insert(positions.end(), { vx, vy, vz });
                    normals.insert(normals.end(), { nx, ny, nz });
                    tex_coords.insert(tex_coords.end(), { tx, ty });
                }
                index_offset += 3;
            }
        }

        return Geometry{ GL_TRIANGLES, positions, {/*indices*/}, normals, {/*colors*/}, tex_coords };
    }
    std::cerr << "Extension " << extension << " not supported" << std::endl;

    return Geometry{};
}