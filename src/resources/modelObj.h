#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

struct ModelVertex
{
   glm::vec3 pos;
   glm::vec3 normal;
   glm::vec2 uv;
};

struct Texture
{
   std::string file;
   uint8_t* pixels = nullptr;
   int components;
   int width;
   int height;

   inline bool exists() {
      return pixels != nullptr;
   }
};

struct Material
{
   Texture diffuseTexture;
   Texture bumpTexture;
   Texture specularTexture;
};

struct Model
{
   struct Mesh
   {
      std::vector<ModelVertex> verts;
   };

   std::unordered_map<int, Mesh> materialShapes;
   int totalModelVertCount;
   std::vector<Material> materials;
};

bool loadModel(const std::string &fileName, Model &model);