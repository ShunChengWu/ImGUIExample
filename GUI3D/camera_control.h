#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace SC{
    class CameraControlBase {
    public:
        CameraControlBase(): bShowUI(false), Position(0, 0, 0), WorldUp(0, 0, 0){}
        virtual void mouse(const glm::vec2& p, int button, bool down)=0;
        virtual void drag(const glm::vec2& p, int button) = 0;
        virtual void scroll(const glm::vec2 &rel)=0;
        virtual glm::mat4 GetViewMatrix() = 0;
        virtual void drawUI(){
            if(!bShowUI)return;
            ImGui::Begin("Camera Control", &bShowUI, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::DragFloat3("Position",&Position[0]);
        }
        void show(){ bShowUI=true;}

        glm::vec3 Position;
        bool bShowUI;
    protected:
        glm::vec3 WorldUp;
    };


    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        UP,
        DOWN,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW         = -90.0f;
    const float PITCH       =  0.0f;
    const float SPEED       =  2.5f;
    const float TRANSLATIONSENSITIVITY =  0.01f;
    const float ROTATIONSENSITIVITY = 0.2f;
    const float SCROLLSENSITIVITY = 0.2f;

    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class CameraControl : public CameraControlBase
    {
    public:
        // Camera Attributes
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;

        // Euler Angles
        float Row;
        float Yaw;
        float Pitch;

        // Camera options
        float MovementSpeed;
        float RotationSensitivity;
        float TranslationSensitivity;
        float ScrollSensitivity;
        // Constructor with vectors
        CameraControl(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = YAW,
               float pitch = PITCH) :
                Yaw(yaw),
                MovementSpeed(SPEED), TranslationSensitivity(TRANSLATIONSENSITIVITY),
                RotationSensitivity(ROTATIONSENSITIVITY), ScrollSensitivity(SCROLLSENSITIVITY)
        {
            Position = position;
            WorldUp = up;
            left_button_down = middle_button_down = right_button_down = false;
            updateCameraVectors();
        }
        // Constructor with scalar values
        CameraControl(float posX, float posY, float posZ,
               float upX, float upY, float upZ, float yaw, float pitch) :
                Yaw(yaw),
                Pitch(pitch),
                MovementSpeed(SPEED), TranslationSensitivity(TRANSLATIONSENSITIVITY),
                RotationSensitivity(ROTATIONSENSITIVITY), ScrollSensitivity(SCROLLSENSITIVITY)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            left_button_down = middle_button_down = right_button_down = false;
            updateCameraVectors();
        }

        // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix() override
        {
//            return Eigen::Map<Eigen::Matrix4f>(glm::value_ptr(mat));
            return glm::lookAt(Position, Position + Front, Up);
        }

        void mouse(const glm::vec2& p, int button, bool down) override {
            if (button == 0) {
                left_button_down = down;
            }
            if (button == 2) {
                middle_button_down = down;
            }
            if (button == 1) {
                right_button_down = down;
            }
            drag_last_pos = p;
        }

        void drag(const glm::vec2& p, int button) override {
            glm::vec2 rel = p - drag_last_pos;
            if(left_button_down){
                ProcessKeyboard(Camera_Movement::LEFT, rel[0]*TranslationSensitivity);
                ProcessKeyboard(Camera_Movement::UP, -rel[1]*TranslationSensitivity);
            }

            if (right_button_down) {
                ProcessMouseMovement(rel.x,-rel.y);
            }

            if (middle_button_down) {
//                Position = glm::angleAxis<float>(float(theta+M_PI_2), glm::vec3(0,0,1)) * glm::vec3(-rel[0],rel[1],0.f)*glm::vec3(distance*1e-3f);
            }

            drag_last_pos = p;
        }

        void scroll(const glm::vec2 &rel) override {
            ProcessKeyboard(BACKWARD, 1 * rel.x * ScrollSensitivity);
//            if (rel[0] > 0) {
////                Position -= Up * rel[0] * MovementSpeed ;
//                distance = distance * 0.9f;
//            } else if (rel[0] < 0) {
////                Position += Up * rel[0] * MovementSpeed ;
//                distance = distance * 1.1f;
//            }
//
//            distance = std::max(0.1, distance);
        }

        void drawUI() override {
            if(!bShowUI)return;
            ImGui::Begin("Camera Control", &bShowUI, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::DragFloat3("Position",&Position[0]);
            ImGui::DragFloat("Yaw",&Yaw);
            ImGui::DragFloat("Pitch",&Pitch,0.1f, -89.f,89.f);
            ImGui::DragFloat("Rotation Sensitivity",&RotationSensitivity, 0.01f,0.001f,5.f);
            ImGui::DragFloat("Translation Sensitivity",&TranslationSensitivity, 0.01f,0.001f,5.f);
            ImGui::DragFloat("Scroll Sensitivity",&ScrollSensitivity, 0.01f,0.001f,5.f);
            ImGui::End();
        }

        void setCamPose(float row, float yaw, float pitch, float x, float y, float z){
            Row = row;
            Pitch = pitch;
            Yaw = yaw;

            Position.x = x;
            Position.y = y;
            Position.z = z;
//            printf("Row,Yaw,Pitch,x,y,z: %f %f %f %f %f %f\n", Row, Yaw, Pitch, Position.x, Position.y, Position.z);
            updateCameraVectors();
        }

        void setPosition(float x, float y, float z){
            Position.x = x;
            Position.y = y;
            Position.z = z;
        }

        // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, float deltaTime)
        {
            //printf("MovingSpeed: %f\n", MovementSpeed);
            float velocity = MovementSpeed * deltaTime;
            if (direction == UP)
                Position -= Up * velocity;
            if (direction == DOWN)
                Position += Up * velocity;
            if (direction == FORWARD)
                Position -= Front * velocity;
            if (direction == BACKWARD)
                Position += Front * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
        }

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
        {
            xoffset *= RotationSensitivity;
            yoffset *= RotationSensitivity;

            Yaw   += xoffset;
            Pitch += yoffset;

            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // Update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset, float deltaTime)
        {
            float velocity = MovementSpeed * deltaTime;
            Position -= Up * yoffset * velocity;
        }

        void setMovingSpeed(float value)
        {
            MovementSpeed = value;
        }

    private:
        bool left_button_down,middle_button_down,right_button_down;
        double distance;
        glm::vec2 drag_last_pos;

        // Calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {
            // Calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            // Also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up    = glm::normalize(glm::cross(Right, Front));
        }
    };

    class ArcCameraContorl: public CameraControlBase
    {
    public:

        // Constructor with vectors
        explicit ArcCameraContorl(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                      float yaw = YAW,
                      float pitch = PITCH) :
                theta(0.0f), phi(-60.0f * M_PI / 180.0f)
        {
            Position = position;
            WorldUp = up;
            left_button_down = middle_button_down = right_button_down = false;
        }
        // Constructor with scalar values
        ArcCameraContorl(float posX, float posY, float posZ,
                      float upX, float upY, float upZ, float yaw, float pitch) :
                theta(0.0f), phi(-60.0f * M_PI / 180.0f)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            left_button_down = middle_button_down = right_button_down = false;
        }

        // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix() override
        {
            glm::vec3 offset = rotation() * glm::vec3(distance, 0.0f, 0.0f);
            glm::vec3 eye = Position + offset;

            glm::mat4 mat = glm::lookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(Position[0], Position[1], Position[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            return mat;
        }

        void mouse(const glm::vec2& p, int button, bool down) override  {
            if (button == 0) {
                left_button_down = down;
            }
            if (button == 2) {
                middle_button_down = down;
            }
            drag_last_pos = p;
        }

        void drag(const glm::vec2& p, int button) override  {
            glm::vec2 rel = p - drag_last_pos;
            if (left_button_down) {
                theta -= rel[0] * 0.01f;
                phi -= rel[1] * 0.01f;

                phi = std::min(M_PI_2 - 0.01, std::max(-M_PI_2 + 0.01, phi));
            }

            if (middle_button_down) {
                Position += glm::angleAxis<float>(float(theta+M_PI_2), glm::vec3(0,0,1)) * glm::vec3(-rel[0],rel[1],0.f)*glm::vec3(distance*1e-3f);
            }

            drag_last_pos = p;
        }

        void scroll(const glm::vec2 &rel) override {
            if (rel[0] > 0) {
                distance = distance * 0.9f;
            } else if (rel[0] < 0) {
                distance = distance * 1.1f;
            }

            distance = std::max(0.1, distance);
        }

        glm::quat rotation() const { return glm::angleAxis<float>(theta, glm::vec3(0,0,1)) * glm::angleAxis<float>(phi, WorldUp); }

    private:
        bool left_button_down,middle_button_down,right_button_down;
        double distance{};
        glm::vec2 drag_last_pos{};
        double theta;
        double phi;
    };
};