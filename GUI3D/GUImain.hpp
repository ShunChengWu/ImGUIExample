//
//  GUImain.hpp
//  DFGGUI
//
//  Created by Shun-Cheng Wu on 18/Dec/2018.
//

#ifndef GUImain_hpp
#define GUImain_hpp

//#include <ElasticFusion.h>
#include "glPlyLoader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glCamera.hpp"
#include "glShader.hpp"
#include "glMesh.hpp"
#include "glModel.hpp"
#include "glMaterialLighting.hpp"
#include "glUtils.hpp"
#include <Utilities/LogUtil.hpp>
#include <Eigen/Core>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <GLFW/glfw3.h>
#include <stdio.h>
//#include <frame_grabber.h>
#include "config.hpp"
//#include <sophus/se3.hpp>
#include <map>
#include <memory>

#include <functional>



namespace DFGProject {
//    #define GUIInstance GUI::getInstance()
    enum DisplayMode {RGB, DEPTH};
    
    struct GLFWWindowContainer {
        GLFWwindow* window;
        int width, height;
        int runtimeWidth, runtimeHeight;
        float nearPlane=0.4, farPlane=500.f;
        std::string name_;
        GLFWWindowContainer(std::string name, int width, int height, float nearPlane=0.4, float farPlane=500.f){
            name_ = std::move(name);
            this->width = width;
            this->height = height;
            this->nearPlane = nearPlane;
            this->farPlane = farPlane;
            window = glfwCreateWindow(width,height,name_.c_str(),NULL,NULL);
        }
        ~GLFWWindowContainer(){
            glfwDestroyWindow(window);
        };
    };
    class GUI_base {
    public:
        static GUI_base& getInstance()
        {
            return *ptrInstance;
        }
        GUI_base();
        ~GUI_base();
        
        GLFWWindowContainer* initWindow(std::string name, int width, int height);
        
        void init();
        //void setCamPose(float x, float y, float z);
        
        void process(GLFWWindowContainer* window);
    protected:
        virtual void processInput(GLFWwindow* window);
        // Call Back Functions
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
            getInstance().key_callback_impl(window, key, scancode, action, mods);
        }
        virtual void key_callback_impl(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
            getInstance().mouse_button_callback_impl(window, button, action, mods);
        }
        virtual void mouse_button_callback_impl(GLFWwindow* window, int button, int action, int mods);
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos){
            getInstance().mouse_callback_impl(window, xpos, ypos);
        }
        virtual void mouse_callback_impl(GLFWwindow* window, double xpos, double ypos);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
            getInstance().scroll_callback_impl(window, xoffset, yoffset);
        }
        virtual void scroll_callback_impl(GLFWwindow* window, double xoffset, double yoffset);
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
            getInstance().framebuffer_size_callback_impl(window, width, height);
        }
        virtual void framebuffer_size_callback_impl(GLFWwindow* window, int width, int height);
        static void error_callback(int error, const char* description){
            getInstance().error_callback_impl(error, description);
        }
        virtual void error_callback_impl(int error, const char* description);
        
        /// The actual processing stuff. Inherit this and while running process this function will be called in right time.
        virtual void process_impl(){};
    
        static GUI_base *ptrInstance;
        glUtil::Camera *glCam;
        bool mouseInit, mousePressedRight;
        float mouseXpre, mouseYpre;
        double deltaFrameTime, lastFrameTime, currentFrameTime;
        glm::vec3 camPose, camUp;
        float yaw, pitch, roll, fov, fovMax, camSpeed;
    };

} // end of namespace DFGProejct

#endif /* GUImain_hpp */
