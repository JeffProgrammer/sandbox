#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

#include "modelObj.h"

struct Primitive
{
   int indicesCount;
};

struct Attribute
{
   enum class Type {
      SCALAR,
      VEC2,
      VEC3,
      VEC4
   };
   
   std::string semanticName;
   int offset;
   int stride;
   Type type;
};

struct MeshAttribute
{
   std::vector<Attribute> attribs;
};

struct Texture
{
   std::string name;
   int samplerIndex;
   int width;
   int height;
   std::vector<unsigned char> pixels;
};

struct Material
{
   Texture diffuseTexture;
   int diffuseTextureCoordIndex;
};

static bool loadFromDisk(const std::string& fileName, tinygltf::Model *model)
{
   std::string err;
   std::string warn;
   tinygltf::TinyGLTF loader;
   bool res = loader.LoadASCIIFromFile(model, &err, &warn, fileName);

   if (!warn.empty())
      printf("GLTF Warn: %s\n", warn.c_str());
   if (!err.empty())
      printf("GLTF Error: %s\n", err.c_str());
   
   return res;
}

bool loadModel(const std::string& fileName, tinygltf::Model *model)
{
   if (!loadFromDisk(fileName, model))
      return false;

   // Buffer holds the vertex and index data. for now just assume 1 buffer and 1 mesh.
   // Eventually may want to make this more robust...
   const tinygltf::Buffer& buffer = model->buffers[0];
   const tinygltf::Mesh& mesh = model->meshes[0];
   const std::vector<tinygltf::Image>& images = model->images;

   // Materials, key is the material index, value is the material that holds texture and data
   std::unordered_map<int, Material> materialMap;

//   std::unordered_map<MeshAttribute, std::tuple<int, std::vector<Primitive>
//
   for (const tinygltf::Primitive& meshPrim: mesh.primitives)
   {
      if (meshPrim.indices <= 0)
         continue;
      
      const tinygltf::Material& mat = model->materials[meshPrim.material];
      const tinygltf::TextureInfo& diffuseTextureInfo = mat.pbrMetallicRoughness.baseColorTexture;
      const tinygltf::Texture& diffuseTexture = model->textures[diffuseTextureInfo.index];
      const tinygltf::Image& diffuseImage = model->images[diffuseTexture.source];
      
      Material material;
      material.diffuseTexture.samplerIndex = diffuseTexture.sampler;
      material.diffuseTexture.name = diffuseTexture.name;
      material.diffuseTexture.height = diffuseImage.height;
      material.diffuseTexture.width = diffuseImage.width;
      material.diffuseTexture.pixels = diffuseImage.image;
      
      material.diffuseTextureCoordIndex = diffuseTextureInfo.index;

      Primitive primitive = {};
      primitive.indicesCount = meshPrim.indices;

   }
//
//   // Group mesh primitives (shapes) by materials
//   std::unordered_map<Material, std::vector<Primitive>> materialPrimitiveGroup;
   
   return true;
}
