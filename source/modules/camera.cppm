module;

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

export module camera;
namespace WisE {

export class Camera {
private:
public:
  struct Settings {
    float yaw{90.0f};
    float pitch{0.0f};
    float sensitivity{0.1f};
    uint8_t wasd = 0;
    glm::vec3 pos{0.0f, -3.0f, 0.5f};
    glm::vec3 front{0.0f, 1.0f, 0.0f};
    glm::vec3 up{0.0f, 0.0f, 1.0f};
    float cameraSpeed = 1.5f;

    void addRotation(float xoffset, float yoffset) {
      yaw -= xoffset * sensitivity;
      pitch -= yoffset * sensitivity;
      pitch = std::clamp(pitch, -89.0f, 89.0f);

      glm::vec3 direction;
      direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      direction.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      direction.z = sin(glm::radians(pitch));
      front = glm::normalize(direction);
    }
  };

  void updatePlayerMovement(Settings& settings, float& deltaTime) {
    glm::vec3 moveDirection(0.0f);

    if (settings.wasd & 8)
      moveDirection += settings.front;
    if (settings.wasd & 2)
      moveDirection -= settings.front;

    glm::vec3 right = glm::normalize(glm::cross(settings.front, settings.up));
    if (settings.wasd & 1)
      moveDirection += right;
    if (settings.wasd & 4)
      moveDirection -= right;

    if (glm::length(moveDirection) > 0.0f) {
      moveDirection = glm::normalize(moveDirection);
      settings.pos += moveDirection * settings.cameraSpeed * deltaTime;
    }
  }
};
} // namespace WisE
