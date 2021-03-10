#pragma once

#include <nanogui/nanogui.h>
#include "Enums.h"
#include "Camera.h"
#include "Lighting.h"
#include "Object.h"

class GUI
{
public:
    GUI(GLFWwindow* window, Camera* camera, Lighting* lighting):
    m_gl_window(window),
    m_camera(camera),
    m_lighting(lighting),
    m_object(NULL),
    m_lighting_window(NULL)
    {};
    ~GUI();

    void init(const std::function<Object*(model_type)> &sel_model, const std::function<void()> &reset);
    void setObject();
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
    int get_depth_val() {
        return depth_type_2_gl_type[m_depth_val];
    }
    model_type get_model_val() {
        return m_model_val;
    }
    bool get_auto_rotate() {
        return m_auto_rotate;
    }
    nanogui::Color& get_obj_color_val() {
        return m_obj_col_val;
    }
    nanogui::Color& get_direction_ambient_color() {
        return m_direction_ambient_color;
    }
    nanogui::Color& get_direction_diffuse_color() {
        return m_direction_diffuse_color;
    }
    nanogui::Color& get_direction_specular_color() {
        return m_direction_specular_color;
    }
    bool get_direction_light_enable() {
        return m_direction_light_enable;
    }
    nanogui::Color& get_point_ambient_color() {
        return m_point_ambient_color;
    }
    nanogui::Color& get_point_diffuse_color() {
        return m_point_diffuse_color;
    }
    nanogui::Color& get_point_specular_color() {
        return m_point_specular_color;
    }
    bool get_point_light_enable() {
        return m_point_light_enable;
    }
    bool get_point_light_rotate_x() {
        return m_point_light_rotate_x;
    }
    bool get_point_light_rotate_y() {
        return m_point_light_rotate_y;
    }
    bool get_point_light_rotate_z() {
        return m_point_light_rotate_z;
    }
    
private:
    static nanogui::Screen* m_nanogui_screen;
    GLFWwindow* m_gl_window;
    Camera* m_camera;
    Lighting* m_lighting;
    Object* m_object;
    nanogui::FormHelper* m_inspector_window;
    nanogui::FormHelper* m_lighting_window;
    model_type m_model_val = CYBORG;
    render_type m_render_val = TRIANGLE;
    culling_type m_culling_val = CCW;
    shading_type m_shading_val = SMOOTH;
    depth_type m_depth_val = LESS;
    nanogui::Color m_obj_col_val = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    nanogui::Color m_direction_ambient_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    nanogui::Color m_direction_diffuse_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    nanogui::Color m_direction_specular_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    nanogui::Color m_point_ambient_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    nanogui::Color m_point_diffuse_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    nanogui::Color m_point_specular_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    uint m_shininess = 32;
    std::function<Object*(model_type)> m_model_sel_callback;
    std::function<void()> m_reset_callback;
    bool m_auto_rotate = false;
    nanogui::Slider *x_slider;
    nanogui::Slider *y_slider;
    nanogui::Slider *z_slider;
    bool m_direction_light_enable = true;
    bool m_point_light_enable = true;
    bool m_point_light_rotate_x = false;
    bool m_point_light_rotate_y = false;
    bool m_point_light_rotate_z = false;
    
    void update_object_params();
    void nanoColor2vec(const nanogui::Color& c, glm::vec4& v) {
        v.x = c.x();
        v.y = c.y();
        v.z = c.z();
        v.w = c.w();
    }
    
    void vec2nanoColor(const glm::vec4& v, nanogui::Color& c) {
        c[0] = v.x;
        c[1] = v.y;
        c[2] = v.z;
        c[3] = v.w;
    }
};
