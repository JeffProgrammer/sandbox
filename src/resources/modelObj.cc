#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "modelObj.h"

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

   for (const auto& shape : reader.GetShapes())
   {
      for (size_t i = 0; i < shape.mesh.indices.size() / 3; ++i)
      {
         tinyobj::index_t idx0 = shape.mesh.indices[3 * i + 0];
         tinyobj::index_t idx1 = shape.mesh.indices[3 * i + 1];
         tinyobj::index_t idx2 = shape.mesh.indices[3 * i + 2];

         int materialId = shape.mesh.material_ids[i];

         Model::Mesh mesh;
         for (int j = 0; j < 3; ++j)
         {
            ModelVertex vert;
         }

         model.materialShapes[materialId].meshes.push_back(mesh);
      }
   }

   return true;
}