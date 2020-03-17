#pragma once
#include "camera_control.h"
#include "projection_control.hpp"
#include <memory>

namespace glUtil{
    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera
    {
    public:
        explicit Camera(int windowWidth, int windowHeight, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = -90.0f,
               float pitch = 0){
            bShowUI=false;
            camera_control_.reset(new SC::CameraControl(position,up,yaw,pitch));
            projection_control_.reset(new SC::ProjectionControl(windowWidth,windowHeight));
        }

        void setSize(int windowWidth, int windowHeight){
            projection_control_->setSize(windowWidth,windowHeight);
        }

        void mouse_control() {
            ImGuiIO& io = ImGui::GetIO();
            auto mouse_pos = ImGui::GetMousePos();
            auto drag_delta = ImGui::GetMouseDragDelta();

            glm::vec2 p(mouse_pos.x, mouse_pos.y);

            for (int i = 0; i < 3; i++) {
                if (ImGui::IsMouseClicked(i)) {
                    camera_control_->mouse(p, i, true);
                }
                if (ImGui::IsMouseReleased(i)) {
                    camera_control_->mouse(p, i, false);
                }
                if (ImGui::IsMouseDragging(i)) {
                    camera_control_->drag(p, i);
                }

                camera_control_->scroll(glm::vec2(io.MouseWheel, io.MouseWheelH));
            }
        }

        void drawUI(){
            bShowUI=true;
            ImGui::Begin("GL Camera", &bShowUI);
            if(ImGui::BeginMenu("Camera", bShowUI)){
                ImGui::MenuItem("Projection Control", NULL, &projection_control_->bShowUI);
                ImGui::MenuItem("Camera Control", NULL, &camera_control_->bShowUI);
                ImGui::EndMenu();
            }
            ImGui::End();
            projection_control_->draw_ui();
            camera_control_->drawUI();
        }

        std::unique_ptr<SC::CameraControlBase> camera_control_;
        std::unique_ptr<SC::ProjectionControl> projection_control_;

        bool bShowUI;
    private:

    };
    
} // end of namespace glUtil