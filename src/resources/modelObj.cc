#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "modelObj.h"
#include <glm/gtx/hash.hpp>

// taken from Vulkan-Tutorial.com (Public Domain)
namespace std {
   template<> struct hash<ModelVertex> {
      size_t operator()(ModelVertex const& vertex) const {
         return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
      }
   };
}

bool loadTexture(const std::string &basePath, const std::string& file, Texture &outTexture)
{
   outTexture.file = basePath + "/" + file;

   unsigned char* image = stbi_load(
      outTexture.file.c_str(), 
      &outTexture.width, 
      &outTexture.height, 
      &outTexture.components, 
      STBI_default
   );

   if (image == NULL)
      return false;
   
   size_t numberOfPixels = outTexture.width * outTexture.height;
   outTexture.pixels = new uint8_t[numberOfPixels];
   memcpy(outTexture.pixels, image, numberOfPixels * sizeof(uint8_t));

   stbi_image_free(image);
   return true;
}

bool loadModel(const std::string& fileName, Model& model)
{
   std::string basePath = fileName.substr(0, fileName.find_last_of("/"));

   tinyobj::ObjReader reader;
   if (!reader.ParseFromFile(fileName))
      return false;

   for (const auto& material : reader.GetMaterials())
   {
      Material mat;

      std::string diffuse = material.diffuse_texname;
      if (diffuse.length() > 0)
      {
         if (!loadTexture(basePath, diffuse, mat.diffuseTexture))
         {
            printf("Error with loading diffuse texture %s\n", diffuse.c_str());
            return false;
         }
      }

      std::string bump = material.bump_texname;
      if (bump.length() > 0)
      {
         if (!loadTexture(basePath, bump, mat.bumpTexture))
         {
            printf("Error with loading bump texture %s\n", bump.c_str());
            return false;
         }
      }

      std::string specular = material.specular_texname;
      if (specular.length() > 0)
      {
         if (!loadTexture(basePath, specular, mat.bumpTexture))
         {
            printf("Error with loading specular texture %s\n", specular.c_str());
            return false;
         }
      }

      model.materials.push_back(mat);
   }

   const auto& vertices = reader.GetAttrib().vertices;
   const auto& normals = reader.GetAttrib().normals;
   const auto& texCoords = reader.GetAttrib().texcoords;

   std::unordered_map<ModelVertex, int> uniqueVertexCount;

   for (const auto& shape : reader.GetShapes())
   {
      for (size_t i = 0; i < shape.mesh.indices.size() / 3; ++i)
      {
         tinyobj::index_t idx0 = shape.mesh.indices[3 * i + 0];
         tinyobj::index_t idx1 = shape.mesh.indices[3 * i + 1];
         tinyobj::index_t idx2 = shape.mesh.indices[3 * i + 2];

         int materialId = shape.mesh.material_ids[i];

         ModelVertex vert[3];

         vert[0].pos.x = vertices[3 * idx0.vertex_index + 0];
         vert[0].pos.y = vertices[3 * idx0.vertex_index + 1];
         vert[0].pos.z = vertices[3 * idx0.vertex_index + 2];
         vert[0].normal.x = normals[3 * idx0.normal_index + 0];
         vert[0].normal.y = normals[3 * idx0.normal_index + 1];
         vert[0].normal.z = normals[3 * idx0.normal_index + 2];
         vert[0].uv.x = texCoords[2 * idx0.texcoord_index + 0];
         vert[0].uv.x = 1.0 - texCoords[2 * idx0.texcoord_index + 1];

         vert[1].pos.x = vertices[3 * idx1.vertex_index + 0];
         vert[1].pos.y = vertices[3 * idx1.vertex_index + 1];
         vert[1].pos.z = vertices[3 * idx1.vertex_index + 2];
         vert[1].normal.x = normals[3 * idx1.normal_index + 0];
         vert[1].normal.y = normals[3 * idx1.normal_index + 1];
         vert[1].normal.z = normals[3 * idx1.normal_index + 2];
         vert[1].uv.x = texCoords[2 * idx1.texcoord_index + 0];
         vert[1].uv.x = 1.0 - texCoords[2 * idx1.texcoord_index + 1];

         vert[2].pos.x = vertices[3 * idx2.vertex_index + 0];
         vert[2].pos.y = vertices[3 * idx2.vertex_index + 1];
         vert[2].pos.z = vertices[3 * idx2.vertex_index + 2];
         vert[2].normal.x = normals[3 * idx2.normal_index + 0];
         vert[2].normal.y = normals[3 * idx2.normal_index + 1];
         vert[2].normal.z = normals[3 * idx2.normal_index + 2];
         vert[2].uv.x = texCoords[2 * idx2.texcoord_index + 0];
         vert[2].uv.x = 1.0 - texCoords[2 * idx2.texcoord_index + 1];

         model.materialShapes[materialId].verts.push_back(vert[0]);
         model.materialShapes[materialId].verts.push_back(vert[1]);
         model.materialShapes[materialId].verts.push_back(vert[2]);

         model.totalModelVertCount += 3;
      }
   }

   return true;
}