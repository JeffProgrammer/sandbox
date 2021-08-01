#pragma once

#include <string>

namespace tinygltf {
struct Model;
}

bool loadModel(const std::string &fileName, tinygltf::Model *model);
