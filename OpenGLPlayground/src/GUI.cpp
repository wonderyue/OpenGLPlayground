#include "GUI.h"


nanogui::Screen* GUI::m_nanogui_screen = nullptr;
bool GUI::keys[1024];


GUI::~GUI()
{
}

void GUI::init(const std::function<Object*(model_type)> &sel_model, const std::function<void()> &reset)
{
    m_nanogui_screen = new nanogui::Screen();
    m_nanogui_screen->initialize(m_gl_window, true);
    m_model_sel_callback = sel_model;
    m_reset_callback = reset;
    glViewport(0, 0, m_camera->width, m_camera->height);

    // Create nanogui gui
    m_inspector_window = new nanogui::FormHelper(m_nanogui_screen);
    m_inspector_window->setFixedSize(Eigen::Vector2i(150,20));
    
    //Inspector
    m_inspector_window->addWindow(Eigen::Vector2i(0, 0), "Inspector");
    
    m_inspector_window->addGroup("Position");
    m_inspector_window->addVariable("x", m_camera->cameraPos.x, true)->setSpinnable(true);
    m_inspector_window->addVariable("y", m_camera->cameraPos.y, true)->setSpinnable(true);
    m_inspector_window->addVariable("z", m_camera->cameraPos.z, true)->setSpinnable(true);
    
    m_inspector_window->addGroup("Rotation");
    x_slider = new nanogui::Slider(m_inspector_window->addVariable("x", m_camera->cameraAngle.x, false));
    x_slider->setFixedSize(Eigen::Vector2i(120,20));
    x_slider->setRange(std::pair<float, float>(-180,180));
    x_slider->setValue(m_camera->cameraAngle.x);
    x_slider->setCallback([&](float value) {
        m_camera->cameraAngle.x = value;
    });
    y_slider = new nanogui::Slider(m_inspector_window->addVariable("y", m_camera->cameraAngle.y, false));
    y_slider->setFixedSize(Eigen::Vector2i(120,20));
    y_slider->setRange(std::pair<float, float>(-180,180));
    y_slider->setValue(m_camera->cameraAngle.y);
    y_slider->setCallback([&](float value) {
        m_camera->cameraAngle.y = value;
    });
    z_slider = new nanogui::Slider(m_inspector_window->addVariable("z", m_camera->cameraAngle.z, false));
    z_slider->setFixedSize(Eigen::Vector2i(120,20));
    z_slider->setRange(std::pair<float, float>(-180,180));
    z_slider->setValue(m_camera->cameraAngle.z);
    z_slider->setCallback([&](float value) {
        m_camera->cameraAngle.z = value;
    });
    
    m_inspector_window->addGroup("Configration");
    nanogui::ComboBox* modelComboBox = m_inspector_window->addVariable("Model", m_model_val, true);
    m_inspector_window->addVariable("near", m_camera->near, true)->setSpinnable(true);
    m_inspector_window->addVariable("far", m_camera->far, true)->setSpinnable(true);
    modelComboBox->setItems({ "Cube", "Rock", "Cyborg", "Two Cubes" });
    modelComboBox->setCallback([&](int value) {
        m_model_val = (model_type)value;
        m_object = m_model_sel_callback(m_model_val);
        update_object_params();
    });
    m_object = m_model_sel_callback(m_model_val);
    update_object_params();
    m_inspector_window->addVariable("Culling", m_culling_val)->setItems({ "CW", "CCW" });
    m_inspector_window->addVariable("Mode", m_render_val)->setItems({ "Point", "Line", "Trangle" });
    m_inspector_window->addVariable("Shading", m_shading_val)->setItems({ "Smooth", "Flat" });
    m_inspector_window->addVariable("Depth", m_depth_val)->setItems({ "Less", "Always" });
    m_inspector_window->addVariable("Auto Rotate", m_auto_rotate);
    
    m_inspector_window->addButton("Reset", [&]() {
        m_reset_callback();
    })->setTooltip("Reset camera position and rotation.");
    
    //Lighting
    m_lighting_window = new nanogui::FormHelper(m_nanogui_screen);
    m_lighting_window->setFixedSize(Eigen::Vector2i(100,20));
    m_lighting_window->addWindow(Eigen::Vector2i(260, 0), "Lighting");
    m_lighting_window->addVariable("Object Color", m_obj_col_val, true)->setCallback([&](const nanogui::Color &c) {
        m_obj_col_val = c;
        nanoColor2vec(c, m_object->obj_color);
    });
    m_lighting_window->addVariable("Object Shininess", m_shininess, true)->setCallback([&](const int& v) {
        m_shininess = v;
        m_object->shininess = v;
    });
    m_lighting_window->addVariable("Texture Mapping", m_texture_mapping_enable);
    m_lighting_window->addVariable("Normal Mapping", m_normal_mapping_enable);
    
    m_lighting_window->addVariable("Directional Light", m_direction_light_enable);
    m_lighting_window->addVariable("Directional Light x", m_lighting->direction_light.dir.x, true)->setSpinnable(true);
    m_lighting_window->addVariable("Directional Light y", m_lighting->direction_light.dir.y, true)->setSpinnable(true);
    m_lighting_window->addVariable("Directional Light z", m_lighting->direction_light.dir.z, true)->setSpinnable(true);
    m_lighting_window->addVariable("Ambient Strength", m_lighting->direction_light.ambient_strength, true)->setSpinnable(true);;
    m_lighting_window->addVariable("Ambient Color", m_direction_ambient_color, true)->setCallback([&](const nanogui::Color &c) {
        m_direction_ambient_color = c;
        nanoColor2vec(c, m_lighting->direction_light.ambient_color);
    });
    m_lighting_window->addVariable("Diffuse Color", m_direction_diffuse_color, true)->setCallback([&](const nanogui::Color &c) {
        m_direction_diffuse_color = c;
        nanoColor2vec(c, m_lighting->direction_light.diffuse_color);
    });
    m_lighting_window->addVariable("Specular Color", m_direction_specular_color, true)->setCallback([&](const nanogui::Color &c) {
        m_direction_specular_color = c;
        nanoColor2vec(c, m_lighting->direction_light.specular_color);
    });
    
    m_lighting_window->addVariable("Point Light", m_point_light_enable);
    m_lighting_window->addVariable("Ambient Strength", m_lighting->point_light.ambient_strength, true)->setSpinnable(true);;
    m_lighting_window->addVariable("Ambient Color", m_point_ambient_color, true)->setCallback([&](const nanogui::Color &c) {
        m_point_ambient_color = c;
        nanoColor2vec(c, m_lighting->point_light.ambient_color);
    });
    m_lighting_window->addVariable("Diffuse Color", m_point_diffuse_color, true)->setCallback([&](const nanogui::Color &c) {
        m_point_diffuse_color = c;
        nanoColor2vec(c, m_lighting->point_light.diffuse_color);
    });
    m_lighting_window->addVariable("Specular Color", m_point_specular_color, true)->setCallback([&](const nanogui::Color &c) {
        m_point_specular_color = c;
        nanoColor2vec(c, m_lighting->point_light.specular_color);
    });
    m_lighting_window->addVariable("Rotate on X", m_point_light_rotate_x);
    m_lighting_window->addVariable("Rotate on Y", m_point_light_rotate_y);
    m_lighting_window->addVariable("Rotate on Z", m_point_light_rotate_z);
    
    m_lighting_window->addButton("Reset", [&]() {
        m_lighting->reset(m_camera->cameraPos, m_camera->cameraFront);
    })->setTooltip("Reset point light position and rotation.");
    
    //Show UI
    m_nanogui_screen->setVisible(true);
    m_nanogui_screen->performLayout();

    glfwSetCursorPosCallback(m_gl_window,
        [](GLFWwindow* window, double x, double y) {
            m_nanogui_screen->cursorPosCallbackEvent(x, y);
        }
    );

    glfwSetMouseButtonCallback(m_gl_window,
        [](GLFWwindow*, int button, int action, int modifiers) {
            m_nanogui_screen->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(m_gl_window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
            if (key >= 0 && key < 1024)
            {
                if (action == GLFW_PRESS)
                    keys[key] = true;
                else if (action == GLFW_RELEASE)
                    keys[key] = false;
            }
        }
    );

    glfwSetCharCallback(m_gl_window,
        [](GLFWwindow*, unsigned int codepoint) {
            m_nanogui_screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(m_gl_window,
        [](GLFWwindow*, int count, const char** filenames) {
            m_nanogui_screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(m_gl_window,
        [](GLFWwindow*, double x, double y) {
            m_nanogui_screen->scrollCallbackEvent(x, y);
            //m_camera->ProcessMouseScroll(y);
        }
    );

    glfwSetFramebufferSizeCallback(m_gl_window,
        [](GLFWwindow*, int width, int height) {
            m_nanogui_screen->resizeCallbackEvent(width, height);
        }
    );
}

void GUI::update_object_params()
{
    vec2nanoColor(m_object->obj_color, m_obj_col_val);
    m_shininess = m_object->shininess;
}

void GUI::process_keyboard(float delta_time)
{
    m_camera->update_direction();
    if (keys[GLFW_KEY_W])
        m_camera->process_keyboard(FORWARD, delta_time);
    if (keys[GLFW_KEY_S])
        m_camera->process_keyboard(BACKWARD, delta_time);
    if (keys[GLFW_KEY_A])
        m_camera->process_keyboard(LEFT, delta_time);
    if (keys[GLFW_KEY_D])
        m_camera->process_keyboard(RIGHT, delta_time);
    if (keys[GLFW_KEY_Q])
        m_camera->process_keyboard(UP, delta_time);
    if (keys[GLFW_KEY_E])
        m_camera->process_keyboard(DOWN, delta_time);
    if (keys[GLFW_KEY_I])
        m_camera->process_keyboard(ROTATE_X_UP, delta_time);
    if (keys[GLFW_KEY_K])
        m_camera->process_keyboard(ROTATE_X_DOWN, delta_time);
    if (keys[GLFW_KEY_J])
        m_camera->process_keyboard(ROTATE_Y_UP, delta_time);
    if (keys[GLFW_KEY_L])
        m_camera->process_keyboard(ROTATE_Y_DOWN, delta_time);
    if (keys[GLFW_KEY_U])
        m_camera->process_keyboard(ROTATE_Z_UP, delta_time);
    if (keys[GLFW_KEY_O])
        m_camera->process_keyboard(ROTATE_Z_DOWN, delta_time);
}

void GUI::refresh()
{
    m_inspector_window->refresh();
    m_lighting_window->refresh();
    x_slider->setValue(m_camera->cameraAngle.x);
    y_slider->setValue(m_camera->cameraAngle.y);
    z_slider->setValue(m_camera->cameraAngle.z);
}

void GUI::draw()
{
    m_nanogui_screen->drawWidgets();
}
