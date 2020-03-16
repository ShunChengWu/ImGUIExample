#include <stdexcept>
#include "GUI.h"
#include <iostream>

using namespace SC;

GUI_base *GUI_base::ptrInstance;
GUI_base::GUI_base(){
    ptrInstance=this;
}
GUI_base::~GUI_base(){
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if(window_)delete window_;
    glfwTerminate();
}
//static void glfw_error_callback(int error, const char* description)
//{
//    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
//}

void GUI_base::init() {
    // Setup window
    glfwSetErrorCallback(SC::GUI_base::error_callback);
    if (!glfwInit()) throw std::runtime_error("Unable to init glfw!\n");

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130

    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
}

void GUI_base::initWindow(const std::string& name, int width, int height) {
    window_ = new GLFWWindowContainer(name, width, height);
    if(!window_->window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        delete window_;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window_->window);
    glfwSetKeyCallback(window_->window, key_callback);
    glfwSetMouseButtonCallback(window_->window, mouse_button_callback);
    glfwSetCursorPosCallback(window_->window, SC::GUI_base::mouse_callback);
    glfwSetScrollCallback(window_->window, SC::GUI_base::scroll_callback);
    glfwSetFramebufferSizeCallback(window_->window, framebuffer_size_callback);

    // Get runtime width and height. In Retina monitor, the resolution will change
    glfwGetFramebufferSize(window_->window, &window_->runtimeWidth, &window_->runtimeHeight);

    // Enable capture mouse
    glfwSetInputMode(window_->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        delete window_;
        return nullptr;
    }
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err) throw std::runtime_error("Failed to initialize OpenGL loader!\n");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
//    ImGui::StyleColorsLight();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window_->window, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());


    // Enable depth test for 3D
    glEnable(GL_DEPTH_TEST);
    // Enable Blend for transparent objects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void GUI_base::run() {
    while(!glfwWindowShouldClose(window_->window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        draw_ui();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window_->window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_gl();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_->window);
    }
}

void GUI_base::draw_gl() {}

void GUI_base::draw_ui() {
    ImGui::ShowDemoWindow();
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
//    glCam->ProcessMouseScroll(yoffset, deltaFrameTime);
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

//    glCam->ProcessMouseMovement(xoffset, yoffset);
}