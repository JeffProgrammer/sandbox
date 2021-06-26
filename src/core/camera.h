#include <glm/glm.hpp>

struct Move
{
   float x = 0.0f;
   float y = 0.0f;
   float yaw = 0.0f;
   float pitch = 0.0f;
};

class Camera
{
public:
   Camera();

   void update(double dt, const Move& move);

   inline void setProjectionMatrix(const glm::mat4 &projMatrix) 
   { 
      mProjMatrix = projMatrix; 
   }

   inline void setPosition(const glm::vec3& pos)
   {
      mPosition = pos;
   }

   inline glm::vec3 getPosition() const
   {
      return mPosition;
   }

   inline void setYawPitch(float yaw, float pitch)
   {
      mYaw = yaw;
      mPitch = pitch;
   }

   inline void getMatrices(glm::mat4& projMatrix, glm::mat4& viewMatrix)
   {
      projMatrix = mProjMatrix;
      viewMatrix = mViewMatrix;
   }

private:
   glm::vec3 mPosition;
   float mPitch;
   float mYaw;
   glm::mat4 mViewMatrix;
   glm::mat4 mProjMatrix;
};
