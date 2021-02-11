#include "GUI.h"
#include "Camera.h"


nanogui::Screen* GUI::m_nanogui_screen = nullptr;
bool GUI::keys[1024];


GUI::~GUI()
{
}

void GUI::init(const std::function<void(model_type)> &callback)
{
    m_nanogui_screen = new nanogui::Screen();
    m_nanogui_screen->initialize(m_gl_window, true);
    m_model_sel_callback = callback;
    glViewport(0, 0, m_camera->width, m_camera->height);

    // Create nanogui gui
    m_gui = new nanogui::FormHelper(m_nanogui_screen);
    m_gui->setFixedSize(Eigen::Vector2i(150,20));
    nanogui::ref<nanogui::Window> nanoguiWindow = m_gui->addWindow(Eigen::Vector2i(0, 0), "Inspector");
    
    m_gui->addGroup("Position");
    m_gui->addVariable("x", m_camera->cameraPos.x, true)->setSpinnable(true);
    m_gui->addVariable("y", m_camera->cameraPos.y, true)->setSpinnable(true);
    m_gui->addVariable("z", m_camera->cameraPos.z, true)->setSpinnable(true);
    
    m_gui->addGroup("Rotation");
    nanogui::Slider *x_slider = new nanogui::Slider(m_gui->addVariable("x", m_camera->pitch, false));
    x_slider->setFixedSize(Eigen::Vector2i(120,20));
    x_slider->setRange(std::pair<float, float>(-180,180));
    x_slider->setValue(m_camera->pitch);
    x_slider->setCallback([&](float value) {
        m_camera->pitch = value;
    });
    nanogui::Slider *y_slider = new nanogui::Slider(m_gui->addVariable("y", m_camera->yaw, false));
    y_slider->setFixedSize(Eigen::Vector2i(120,20));
    y_slider->setRange(std::pair<float, float>(-180,180));
    y_slider->setValue(m_camera->yaw);
    y_slider->setCallback([&](float value) {
        m_camera->yaw = value;
    });
    nanogui::Slider *z_slider = new nanogui::Slider(m_gui->addVariable("z", m_camera->roll, false));
    z_slider->setFixedSize(Eigen::Vector2i(120,20));
    z_slider->setRange(std::pair<float, float>(-180,180));
    z_slider->setValue(m_camera->roll);
    z_slider->setCallback([&](float value) {
        m_camera->roll = value;
    });
    
    m_gui->addGroup("Configration");
    nanogui::ComboBox* modelComboBox = m_gui->addVariable("Model", m_model_val, true);
    modelComboBox->setItems({ "Cube", "Rock", "Cyborg", "Two Cubes" });
    modelComboBox->setCallback([&](int value) {
        m_model_val = (model_type)value;
        m_model_sel_callback(m_model_val);
    });
    m_model_sel_callback(m_model_val);
    m_gui->addVariable("Mode", m_render_val, true)->setItems({ "Point", "Line", "Trangle" });
    m_gui->addVariable("Color", m_col_val);
    
    m_gui->addButton("Reset", [&]() {
        m_camera->reset();
    })->setTooltip("Reset camera position and rotation.");
    
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
    m_gui->refresh();
}

void GUI::draw()
{
    m_nanogui_screen->drawWidgets();
}

int GUI::get_render_mode()
{
    return render_type_2_gl_type[m_render_val];
}
