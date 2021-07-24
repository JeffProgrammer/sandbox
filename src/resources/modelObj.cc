#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

#include "modelObj.h"
#include <glm/gtx/hash.hpp>

static bool loadGLTFModel(const std::string& fileName, tinygltf::Model& model)
{
   std::string err;
   std::string warn;
   tinygltf::TinyGLTF loader;
   bool res = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);

   if (!warn.empty())
      printf("GLTF Warn: %s\n", warn.c_str());
   if (!err.empty())
      printf("GLTF Error: %s\n", err.c_str());

   if (!res)
      return false;

   return true;
}

bool loadModel(const std::string& fileName, Model& model)
{
   tinygltf::Model gltfModel;
   if (!loadGLTFModel(fileName, gltfModel))
      return false;

   for (int i = 0; i < gltfModel.textures.size(); ++i)
   {
      const tinygltf::Texture& texture = gltfModel.textures[i];
      const tinygltf::Image& image = gltfModel.images[texture.source];
   }

   for (int i = 0; i < gltfModel.bufferViews.size(); ++i)
   {

   }

   return true;
}