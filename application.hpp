// ################################################################################
// Common Framework for Computer Graphics Courses at FI MUNI.
//
// Copyright (c) 2021-2022 Visitlab (https://visitlab.fi.muni.cz)
// All rights reserved.
// ################################################################################

#pragma once
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "camera.h"
#include "pv112_application.hpp"
#include "utils.hpp"
#include "cube.hpp"

// Constants
const float clear_color[4] = {0.0, 0.0, 0.0, 1.0};
const float clear_depth[1] = {1.0};

class Application : public PV112Application {

    // ----------------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------------
    std::filesystem::path images_path;
    std::filesystem::path objects_path;

    // GLSL programs
    GLuint main_program = 0;
    GLuint draw_light_program = 0;
    
    GLuint skybox_program = 0;
    GLuint reflection_program = 0;
    GLuint env_texture = 0;
    Cube cube;

    // List of geometries used in the project
    std::vector<MyObject> objects;
    // Shared pointers are pointers that automatically count how many times they are used. When there are 0 pointers to the object pointed
    // by shared_ptrs, the object is automatically deallocated. Consequently, we gain 3 main properties:
    // 1. Objects are not unnecessarily copied
    // 2. We don't have to track pointers
    // 3. We don't have to deallocate these geometries
    
    // Default camera that rotates around center.
    Camera camera {true};

    GLuint camera_buffer = 0;
    CameraUBO camera_ubo;
    glm::vec3 targetPoint{ 0.f, 5.f, 0.5f };

    std::vector<LightUBO> lights;
    GLuint lights_buffer = 0;

    GLuint board3_blank;
    GLuint board3;
    GLuint board8_blank;
    GLuint board8;

    void update_camera_ubo();
    void update_train_position();
    // ----------------------------------------------------------------------------
    // Constructors & Destructors
    // ----------------------------------------------------------------------------
  public:
    /**
     * Constructs a new @link Application with a custom width and height.
     *
     * @param 	initial_width 	The initial width of the window.
     * @param 	initial_height	The initial height of the window.
     * @param 	arguments	  	The command line arguments used to obtain the application directory.
     */
    Application(int initial_width, int initial_height, std::vector<std::string> arguments = {});

    /** Destroys the {@link Application} and releases the allocated resources. */
    ~Application() override;

    // ----------------------------------------------------------------------------
    // Methods
    // ----------------------------------------------------------------------------

    /** @copydoc PV112Application::compile_shaders */
    void compile_shaders() override;

    /** @copydoc PV112Application::delete_shaders */
    void delete_shaders() override;

    /** @copydoc PV112Application::update */
    void update(float delta) override;

    /** @copydoc PV112Application::render */
    void render() override;

    /** @copydoc PV112Application::render_ui */
    void render_ui() override;

    // ----------------------------------------------------------------------------
    // Input Events
    // ----------------------------------------------------------------------------

    /** @copydoc PV112Application::on_resize */
    void on_resize(int width, int height) override;

    /** @copydoc PV112Application::on_mouse_move */
    void on_mouse_move(double x, double y) override;

    /** @copydoc PV112Application::on_mouse_button */
    void on_mouse_button(int button, int action, int mods) override;

    /** @copydoc PV112Application::on_key_pressed */
    void on_key_pressed(int key, int scancode, int action, int mods) override;
};

#endif