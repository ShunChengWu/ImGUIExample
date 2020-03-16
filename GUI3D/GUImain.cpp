//
//  GUImain.cpp
//  DFGGUI
//
//  Created by Shun-Cheng Wu on 18/Dec/2018.
//

#include "GUImain.hpp"
#include <Eigen/Core>
#include <utility>
//#include <glog/logging.h>

#ifdef WIN32 //For create/delete files
#include <direct.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//#undef STB_IMAGE_IMPLEMENTATION

using namespace DFGProject;

GUI_base *GUI_base::ptrInstance;

GUI_base::GUI_base(): glCam(NULL), mouseInit(false), mousePressedRight(false){
    ptrInstance = this;
    camPose = glm::vec3(0.0f, 0.f,  -4.5f);
    camUp = glm::vec3(0.f, 1.f, 0.f);
    camSpeed = 1.f;
    fov = 45.f;
    fovMax = 90.f;
    yaw = 90.f; // y-axis 0-180-360: +z, 90-270-450: -z
    pitch = 0.f; // z-axis
    roll = 0.f; // x-axis

    // Camera
    if (glCam != NULL) delete glCam;
    glCam = new glUtil::Camera(camPose, camUp, yaw, pitch);
}

GUI_base::~GUI_base(){
    glfwTerminate();
}

void GUI_base::init(){
    glfwSetErrorCallback(this->error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}

GLFWWindowContainer* GUI_base::initWindow(std::string name, int width, int height){
//    GUI_CONFIG.windowMain.width = GUI_CONFIG.cParamColor.width = width;
//    GUI_CONFIG.windowMain.height = GUI_CONFIG.cParamColor.height = height;
    mouseXpre = float(width)/2;
    mouseYpre = float(height)/2;

    auto *glfwwindow = new GLFWWindowContainer(std::move(name), width, height);

    if(!glfwwindow->window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        delete glfwwindow;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(glfwwindow->window);
    glfwSetKeyCallback(glfwwindow->window, key_callback);
    glfwSetMouseButtonCallback(glfwwindow->window, mouse_button_callback);
    glfwSetCursorPosCallback(glfwwindow->window, this->mouse_callback);
    glfwSetScrollCallback(glfwwindow->window, this->scroll_callback);
    glfwSetFramebufferSizeCallback(glfwwindow->window, framebuffer_size_callback);

    // Get runtime width and height. In Retina monitor, the resolution will change
    glfwGetFramebufferSize(glfwwindow->window, &glfwwindow->runtimeWidth, &glfwwindow->runtimeHeight);

    // Enable capture mouse
    glfwSetInputMode(glfwwindow->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        delete glfwwindow;
        return nullptr;
    }

    // Enable depth test for 3D
    glEnable(GL_DEPTH_TEST);
    // Enable Blend for transparent objects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return glfwwindow;
}

void GUI_base::process(GLFWWindowContainer* window){
    TICK("[GUI_base][process]1.glfwPollEvents");
    glfwPollEvents();
    TOCK("[GUI_base][process]1.glfwPollEvents");

    //TODO: UNCOMMAND ME
    TICK("[GUI_base][process]2.ProcessInput");
//        glfwGetFramebufferSize(window->window, &window->runtimeWidth, &window->runtimeHeight);
    currentFrameTime = glfwGetTime();
    deltaFrameTime = lastFrameTime - currentFrameTime;
    lastFrameTime = currentFrameTime;
    processInput(window->window);
    TOCK("[GUI_base][process]2.ProcessInput");

    process_impl(); // do timing inside.

    TICK("[GUI_base][process]4.glfwSwapBuffers");
    glfwSwapBuffers(window->window);
    TOCK("[GUI_base][process]4.glfwSwapBuffers");
}

void GUI_base::error_callback_impl(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

inline void GUI_base::mouse_button_callback_impl(GLFWwindow* window, int button, int action, int mods){
    //if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
}

void GUI_base::key_callback_impl(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void GUI_base::framebuffer_size_callback_impl(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

inline void GUI_base::scroll_callback_impl(GLFWwindow* window, double xoffset, double yoffset){
    glCam->ProcessMouseScroll(yoffset, deltaFrameTime);
}

inline void GUI_base::mouse_callback_impl(GLFWwindow* window, double xpos, double ypos){
    if(!mousePressedRight) return;
    if(!mouseInit)
    {
        mouseXpre = xpos;
        mouseYpre = ypos;
        mouseInit = !mouseInit;
        return;
    }

    float xoffset = xpos - mouseXpre;
    float yoffset = mouseYpre - ypos; //
    mouseXpre = xpos;
    mouseYpre = ypos;

    glCam->ProcessMouseMovement(xoffset, yoffset);
}

void GUI_base::processInput(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        glCam->ProcessKeyboard(glUtil::FORWARD, deltaFrameTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        glCam->ProcessKeyboard(glUtil::BACKWARD, deltaFrameTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        glCam->ProcessKeyboard(glUtil::RIGHT, deltaFrameTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        glCam->ProcessKeyboard(glUtil::LEFT, deltaFrameTime);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        mousePressedRight = true;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
        mousePressedRight = mouseInit = false;
}
