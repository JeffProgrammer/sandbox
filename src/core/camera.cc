#include <glm/gtc/matrix_transform.hpp>
#include "core/camera.h"

#define CAMERA_SPEED 10.0f
#define MOUSE_SPEED -0.0025f
#define PI 3.14159f
#define PI_2 1.570796f

#define PITCH_MIN -PI_2
#define PITCH_MAX (PI_2 - 0.2f)

Camera::Camera() : 
    mPosition(glm::vec3(0.0f)), 
    mPitch(0.0f), 
    mYaw(0.0f), 
    mViewMatrix(glm::mat4()), 
    mProjMatrix(glm::mat4()) 
{
}

void Camera::update(double dt, const Move& move)
{
    mYaw += move.yaw * MOUSE_SPEED;
    mPitch += move.pitch * MOUSE_SPEED;

    if (mPitch < PITCH_MIN) mPitch = PITCH_MIN;
    if (mPitch > PITCH_MAX) mPitch = PITCH_MAX;

    glm::vec3 direction;
    direction.x = glm::cos(mPitch) * glm::sin(mYaw);
    direction.y = glm::sin(mPitch);
    direction.z = glm::cos(mPitch) * glm::cos(mYaw);

    glm::vec3 right;
    right.x = glm::sin(mYaw - PI_2);
    right.y = 0.0f;
    right.z = glm::cos(mYaw - PI_2);

    glm::vec3 up = glm::cross(right, direction);

    mPosition += move.y * direction * (float)(dt / 1000.0) * CAMERA_SPEED;
    mPosition += move.x * right * (float)(dt / 1000.0) * CAMERA_SPEED;

    glm::vec3 center = mPosition + direction;
    mViewMatrix = glm::lookAt(mPosition, center, up);
}