#include "application.hpp"

#include <memory>
#include "scene.hpp"

#define DEPTH_TEST

using std::make_shared;

Application::Application(int initial_width, int initial_height, std::vector<std::string> arguments)
    : PV112Application(initial_width, initial_height, arguments) {
    this->width = initial_width;
    this->height = initial_height;

    images_path = lecture_folder_path / "images";
    objects_path = lecture_folder_path / "objects";

    update_camera_ubo();

    // --------------------------------------------------------------------------
    //  Load/Create Objects
    // --------------------------------------------------------------------------
    lights = create_lights();
    objects = create_scene(objects_path, images_path);
    
    env_texture = load_cubemap_texture(lecture_folder_path / "images" / "urban-skyboxes" / "Sodermalmsallen2");
    board3 = load_texture_2d(images_path / "tabula.jpg");
    board3_blank = load_texture_2d(images_path / "tabulaBlank.jpg");
    board8 = load_texture_2d(images_path / "tabula8.jpg");
    board8_blank = load_texture_2d(images_path / "tabula8Blank.jpg");

    // --------------------------------------------------------------------------
    // Create Buffers
    // --------------------------------------------------------------------------
    glCreateBuffers(1, &camera_buffer);
    glNamedBufferStorage(camera_buffer, sizeof(CameraUBO), &camera_ubo, GL_DYNAMIC_STORAGE_BIT);

    glCreateBuffers(1, &lights_buffer);

    for (auto& obj : objects) {
        obj.init_buffers();
    }

    compile_shaders();
}

Application::~Application() {
    delete_shaders();

    glDeleteBuffers(1, &camera_buffer);
    glDeleteBuffers(1, &lights_buffer);
    for (auto& obj : objects) {
        glDeleteBuffers(1, &obj.object_buffer);
    }
}

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void Application::delete_shaders() {
    glDeleteProgram(main_program);
    glDeleteProgram(draw_light_program);
    glDeleteProgram(skybox_program);
    glDeleteProgram(reflection_program);
}

void Application::compile_shaders() {
    delete_shaders();
    main_program = create_program(lecture_shaders_path / "main.vert", lecture_shaders_path / "main.frag");
    draw_light_program = create_program(lecture_shaders_path / "draw_light.vert", lecture_shaders_path / "draw_light.frag");
    skybox_program = create_program(lecture_shaders_path / "skybox.vert", lecture_shaders_path / "skybox.frag");
    reflection_program = create_program(lecture_shaders_path / "main.vert", lecture_shaders_path / "reflect.frag");
}

void Application::update(float delta) {}

void Application::update_camera_ubo() {
    camera_ubo.position = glm::vec4(camera.get_eye_position(), 1.0f);
    camera_ubo.projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.01f, 1000.0f);
    camera_ubo.view = glm::lookAt(camera.get_eye_position() + targetPoint, targetPoint, glm::vec3(0.0f, 0.0f, 1.0f));
}

float get_train_x(float t0) {
    return t0 < 10 ? -64 - (10 - t0) * 16
        : t0 < 18 ? -(t0 - 18) * (t0 - 18)
        : t0 < 33 ? 0
        : t0 < 41 ? 0 + (t0 - 33) * (t0 - 33)
        : 64 + (t0 - 41) * 16;
}

void Application::update_train_position() {
    long long tmil = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    float t0 = tmil % 51000 / 1000.f;
    float x0 = get_train_x(t0);
    auto model0 = create_model_matrix(5+x0, 0, 0.5, PI / 2, 0, PI / 2, 0.05, 0.05, 0.05);
    objects[0].instances[0].model_matrix = model0;
    objects[1].instances[0].model_matrix = model0;
    objects[2].texture = t0 <= 33 ? board3 : board3_blank;

    float t1 = tmil % 51147 / 1000.f;
    float x1 = get_train_x(t1);
    auto model1 = create_model_matrix(-x1-5, 10, 0.5, PI / 2, 0, -PI / 2, 0.05, 0.05, 0.05);
    objects[0].instances[1].model_matrix = model1;
    objects[1].instances[1].model_matrix = model1;
    objects[3].texture = t1 <= 33 ? board8 : board8_blank;

    objects[0].update_buffers();
    objects[1].update_buffers();

    lights[0].position.x = lights[1].position.x = x0 + 5 + 32.2;
    lights[2].position.x = lights[3].position.x = x0 + 5 - 53.6;
    lights[4].position.x = lights[5].position.x = -x1 - 5 - 32.2;
    lights[6].position.x = lights[7].position.x = -x1 - 5 + 53.6;
}

void Application::render() {
    // --------------------------------------------------------------------------
    // Update UBOs
    // --------------------------------------------------------------------------
    // Camera
    update_camera_ubo();
    update_train_position();
    glNamedBufferSubData(camera_buffer, 0, sizeof(CameraUBO), &camera_ubo);
    glNamedBufferData(lights_buffer, sizeof(LightUBO) * lights.size(), lights.data(), GL_DYNAMIC_DRAW);

    // --------------------------------------------------------------------------
    // Draw scene
    // --------------------------------------------------------------------------
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configure fixed function pipeline

    glEnable(GL_CULL_FACE);
    
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glCullFace(GL_FRONT);
    glUseProgram(skybox_program);
    glBindTextureUnit(9, env_texture);
    cube.draw();
    glCullFace(GL_BACK);

    // Configure fixed function pipeline
#ifdef DEPTH_TEST
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
#endif

    // Draw objects
    glUseProgram(main_program);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lights_buffer);

    for (auto& obj : objects) {
        if (obj.reflects_skybox) {
            glUseProgram(reflection_program);
            obj.draw();
            glUseProgram(main_program);
        }
        else {
            obj.draw();
        }
    }
}

void Application::render_ui() { const float unit = ImGui::GetFontSize(); }

// ----------------------------------------------------------------------------
// Input Events
// ----------------------------------------------------------------------------

void Application::on_resize(int width, int height) {
    // Calls the default implementation to set the class variables.
    PV112Application::on_resize(width, height);
}

void Application::on_mouse_move(double x, double y) { camera.on_mouse_move(x, y); }

void Application::on_mouse_button(int button, int action, int mods) { camera.on_mouse_button(button, action, mods); }

void Application::on_key_pressed(int key, int scancode, int action, int mods) {
    // Calls default implementation that invokes compile_shaders when 'R key is hit.
    PV112Application::on_key_pressed(key, scancode, action, mods);
    if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
        glm::vec3 dir = camera.get_eye_position();
        dir.z = 0;
        if (dir.x == 0 && dir.y == 0) {
            return;
        }
        dir = glm::normalize(dir) * 0.1;
        if (key == GLFW_KEY_UP) {
            targetPoint += glm::vec3(-dir.x, -dir.y, 0);
        }
        else if (key == GLFW_KEY_DOWN) {
            targetPoint += glm::vec3(dir.x, dir.y, 0);
        }
        else if (key == GLFW_KEY_LEFT) {
            targetPoint += glm::vec3(dir.y, -dir.x, 0);
        }
        else if (key == GLFW_KEY_RIGHT) {
            targetPoint += glm::vec3(-dir.y, dir.x, 0);
        }
    }
}