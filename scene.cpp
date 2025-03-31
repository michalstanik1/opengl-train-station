#include "scene.hpp"
#include "cube.hpp"
#include "cylinder.hpp"
#include "sphere.hpp"
#include "board.hpp"

std::vector<MyObject> create_scene(const std::filesystem::path &objects_path, const std::filesystem::path &images_path) {
	std::vector<MyObject> objects;
    int diff = 8;

    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "train2.obj", { 0, 2, 3, 4 })),
        create_model_matrix(0, 0, 0.5, PI / 2, 0, PI / 2, 0.05, 0.05, 0.05)
    );
    objects.back().texture = load_texture_2d(images_path / "train3.png");
    objects.back().addInstance(create_model_matrix(0, 10, 0.5, PI / 2, 0, -PI / 2, 0.05, 0.05, 0.05));
    
    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "train2.obj", { 1 })),
        create_model_matrix(0, 0, 0.5, PI / 2, 0, PI / 2, 0.05, 0.05, 0.05),
        glm::vec4(1, 1, 1, 1)
    );
    objects.back().reflects_skybox = true;
    objects.back().addInstance(create_model_matrix(0, 10, 0.5, PI / 2, 0, -PI / 2, 0.05, 0.05, 0.05));

    objects.emplace_back(
        std::make_shared<Board>(),
        create_model_matrix(0, 3, 4.6, 0, 0, 0, 0.1, 1, 0.3)
    );
    for (int x = 2 * diff; x < 44; x += 2 * diff) {
        objects.back().addInstance(create_model_matrix(x, 3, 4.6, 0, 0, 0, 0.1, 1, 0.3));
        objects.back().addInstance(create_model_matrix(-x, 3, 4.6, 0, 0, 0, 0.1, 1, 0.3));
    }

    objects.emplace_back(
        std::make_shared<Board>(),
        create_model_matrix(0, 7, 4.6, 0, 0, 0, 0.1, 1, 0.3)
    );
    for (int x = 2 * diff; x < 44; x += 2 * diff) {
        objects.back().addInstance(create_model_matrix(x, 7, 4.6, 0, 0, 0, 0.1, 1, 0.3));
        objects.back().addInstance(create_model_matrix(-x, 7, 4.6, 0, 0, 0, 0.1, 1, 0.3));
    }

    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "Wood_Bench.obj", {0})),
        create_model_matrix(0, 5, 1, PI / 2, 0, 0, 0.1, 0.1, 0.1)
    );
    objects.back().texture = load_texture_2d(images_path / "wood.jpg");
    for (int i = 1; i * diff < 45; i++) {
        float rot = (i % 2) * PI;
        objects.back().addInstance(create_model_matrix(i * diff, 5, 1, PI / 2, 0, rot, 0.1, 0.1, 0.1));
        objects.back().addInstance(create_model_matrix(-i * diff, 5, 1, PI / 2, 0, rot, 0.1, 0.1, 0.1));
    }

    objects.emplace_back(
        std::make_shared<Cube>(),
        create_model_matrix(0, 5, 0, 0, 0, 0, 50, 3.5, 1)
    );
    objects.back().model_coord_for_texture = true;
    objects.back().procedural_texture = true;
    objects.back().texture = load_texture_2d(images_path / "paving.jpg");

    objects.emplace_back(
        std::make_shared<Cube>(),
        create_model_matrix(0, 5, 5, 0, 0, 0, 50, 3.5, 0.1),
        glm::vec4(0.7, 0.7, 0.7, 1.0),
        glm::vec4(0.7, 0.7, 0.7, 1.0)
    );

    objects.emplace_back(
        std::make_shared<Cylinder>(),
        create_model_matrix(4, 5, 2, PI / 2, 0, 0, 0.5, 3, 0.5),
        glm::vec4(0.7, 0.7, 0.7, 1.0),
        glm::vec4(0.7, 0.7, 0.7, 1.0)
    );
    for (int x = 4 + diff; x < 50; x += diff) {
        objects.back().addInstance(create_model_matrix(x, 5, 2, PI / 2, 0, 0, 0.5, 3, 0.5));
    }
    for (int x = 4 - diff; x > -50; x -= diff) {
        objects.back().addInstance(create_model_matrix(x, 5, 2, PI / 2, 0, 0, 0.5, 3, 0.5));
    }

    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "VisitorKidSit.obj", {0})),
        create_model_matrix(0, 5, 0.9, 0, 0, 200 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "VisitorKidGreen.png");
    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "VisitorKidSit.obj", {1})),
        create_model_matrix(0, 5, 0.9, 0, 0, 200 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "Book.png");

    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "FemaleVisitor.obj", {0})),
        create_model_matrix(3, 4, 1, 0, 0, -20 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "FemaleVisitor.png");
    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "FemaleVisitor.obj", {1})),
        create_model_matrix(3, 4, 1, 0, 0, -20 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "Handbag.png");

    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "MaleVisitorStatic.obj", {1})),
        create_model_matrix(-4, 7, 1, 0, 0, 45 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "MaleVisitor.png");
    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "MaleVisitorStatic.obj", { 0 })),
        create_model_matrix(-4, 7, 1, 0, 0, 45 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "PhoneMale.png");

    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "Nurse.obj", {0})),
        create_model_matrix(-3, 6, 1, 0, 0, -60 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "NurseFemaleDiffuse.png");
    objects.emplace_back(
        std::make_shared<Geometry>(from_file_extended(objects_path / "Nurse.obj", {1})),
        create_model_matrix(-3, 6, 1, 0, 0, -60 * PI / 180)
    );
    objects.back().texture = load_texture_2d(objects_path / "Clipboard.png");

    std::vector<glm::vec4> colors = { {}, {0.054, 0.03, 0.014, 1}, {0.194, 0.194, 0.194, 1} };
    for (int i = 0; i < 3; i++) {
        objects.emplace_back(
            std::make_shared<Geometry>(from_file_extended(objects_path / "railroadtrack.obj", { i })),
            create_model_matrix(0, 0, 0, PI / 2, 0, 0),
            colors[i], colors[i]
        );
        if (i == 0) {
            objects.back().model_coord_for_texture = true;
            objects.back().texture = load_texture_2d(images_path / "gravel.jpg");
        }
        else if (i == 2) {
            objects.back().instances[0].specular_color = glm::vec4(1.f, 1.f, 1.f, 0.5f);
        }
        objects.back().addInstance(create_model_matrix(0, 10, 0, PI / 2, 0, 0));
        for (int i = 1; i < 8; i++) {
            objects.back().addInstance(create_model_matrix(i * 24, 0, 0, PI / 2, 0, 0));
            objects.back().addInstance(create_model_matrix(i * 24, 10, 0, PI / 2, 0, 0));
            objects.back().addInstance(create_model_matrix(-i * 24, 0, 0, PI / 2, 0, 0));
            objects.back().addInstance(create_model_matrix(-i * 24, 10, 0, PI / 2, 0, 0));
        }
    }

	return objects;
}

std::vector<LightUBO> create_lights() {
	std::vector<LightUBO> lights;
    for (float y : {0, 10}) {
        for (float dy : {-0.5, 0.5}) {
            lights.push_back({
                glm::vec4(32.2, y + dy, 1.5, 1), // position
                glm::vec4(0.0f), // ambient
                glm::vec4(2.0f), // diffuse
                glm::vec4(2.0f), // specular
            });
        }
        for (float dy : {-0.7, 0.7}) {
            lights.push_back({
                glm::vec4(-53.6, y + dy, 1.5, 1), // position
                glm::vec4(0.0f), // ambient
                glm::vec4(2.0f, 0.0f, 0.0f, 0.0f), // diffuse
                glm::vec4(2.0f, 0.0f, 0.0f, 0.0f), // specular
            });
        }
    }
    lights.push_back({
        glm::vec4(0.f, 0.f, 1.f, 0.f), // position
        glm::vec4(0.00f), // ambient
        glm::vec4(0.03f), // diffuse
        glm::vec4(0.03f), // specular
    });

    for (int x = -40; x <= 40; x += 8) {
        lights.push_back({
            glm::vec4(x, 5, 4.89, 1), // position
            glm::vec4(0.0f), // ambient
            glm::vec4(1.0f), // diffuse
            glm::vec4(1.0f), // specular
        });
    }

	return lights;
}