#pragma once

#include <nanogui/nanogui.h>
#include "Enums.h"

class Camera;

class GUI
{
public:
    GUI(GLFWwindow* window, Camera* camera):
    m_gl_window(window),
    m_camera(camera)
    {};
    ~GUI();

    void init(const std::function<void(model_type)> &callback);
    void process_keyboard(float delta_time);
    void refresh();
    void draw();
    
    static bool keys[1024];
    int get_render_mode() {
        return render_type_2_gl_type[m_render_val];
    }
    int get_culling_val() {
        return culling_type_2_gl_type[m_culling_val];
    }
    shading_type get_shading_val() {
        return m_shading_val;
    }
    depth_type get_depth_val() {
        return m_depth_val;
    }
    nanogui::Color get_color_val() {
        return m_col_val;
    }
    
    
private:
    static nanogui::Screen* m_nanogui_screen;
    GLFWwindow* m_gl_window;
    Camera* m_camera;
    nanogui::FormHelper* m_gui;
    model_type m_model_val = CYBORG;
    render_type m_render_val = LINE;
    culling_type m_culling_val = CCW;
    shading_type m_shading_val = SMOOTH;
    depth_type m_depth_val = LESS;
    nanogui::Color m_col_val = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    std::function<void(model_type)> m_model_sel_callback;
};
