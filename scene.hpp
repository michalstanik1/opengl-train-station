#pragma once

#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "utils.hpp"

std::vector<MyObject> create_scene(const std::filesystem::path& objects_path, const std::filesystem::path& images_path);

std::vector<LightUBO> create_lights();

#endif