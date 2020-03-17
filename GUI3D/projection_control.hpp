#ifndef PROJECTION_CONTROL_HPP
#define PROJECTION_CONTROL_HPP

#include <glm/glm.hpp>
#include "glCamera.hpp"
namespace SC {

class ProjectionControl {
public:

  ProjectionControl(unsigned int windowWidth, unsigned int windowHeight, glUtil::Camera *camera);
  ~ProjectionControl();

  void setSize(unsigned int windowWidth, unsigned int windowHeight) {
      width_ = windowWidth;
      height_ = windowHeight;
  }
  void setCamera(glUtil::Camera *camera){
      camera_ =camera;
  }

  glm::mat4 projection_matrix() const;

  void draw_ui();

  void show();

private:
    glUtil::Camera *camera_;
  bool show_window;
  unsigned int width_, height_;

  int projection_mode;

  float fovy;
  float width;
  float near;
  float far;
};

}

#endif