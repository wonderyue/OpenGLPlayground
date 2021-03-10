#include "Renderer.h"

Camera* Renderer::m_camera = new Camera();
ResourceManager* Renderer::m_res_manager = new ResourceManager();
Lighting* Renderer::m_lighting = new Lighting();

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    delete m_gui;
}

void Renderer::run()
{
	init();
	display(this->m_window);
}

void Renderer::init()
{
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#if defined(__APPLE__)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    m_camera->init();
    m_lighting->init(m_camera->cameraPos, m_camera->cameraFront);

	this->m_window = glfwCreateWindow(m_camera->width, m_camera->height, "OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(this->m_window);

	glewExperimental = GL_TRUE;
	glewInit();
    m_gui = new GUI(this->m_window, m_camera, m_lighting);
    m_gui->init(std::bind(&Renderer::on_model_change, this, std::placeholders::_1),
                std::bind(&Renderer::scene_reset, this));
    
    m_shaders.emplace_back("shader/mvp.vert", "shader/phong.frag");
    m_shaders.emplace_back("shader/mvp_flat.vert", "shader/phong_flat.frag");
}

void Renderer::display(GLFWwindow* window)
{
	// Main frame while loop
	while (!glfwWindowShouldClose(window))
	{
        Shader m_shader = m_shaders[m_gui->get_shading_val()];
        // update delta time
        GLfloat current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

        m_gui->process_keyboard(delta_time);
		
		m_shader.use();
        
        update();
        
		setup_uniform_values(m_shader);

		draw_scene(m_shader);
        
        m_gui->refresh();
        m_gui->draw();

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return;
}

void Renderer::scene_reset()
{
    const ResourceManager::ModelData& data = m_res_manager->get_model_data(m_gui->get_model_val());
    m_camera->reset(data.position, data.rotation);
    m_lighting->reset(m_camera->cameraPos, m_camera->cameraFront);
}

void Renderer::draw_scene(Shader& shader)
{
	// Set up some basic parameters
	glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(m_gui->get_depth_val());
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    draw_object(shader, *m_rendering_model);
}

void Renderer::draw_object(Shader& shader, Object& object)
{
    glFrontFace(m_gui->get_culling_val());
    glPolygonMode(GL_FRONT_AND_BACK, m_gui->get_render_mode());
	glBindVertexArray(object.vao);
    glDrawElements(GL_TRIANGLES, object.veo_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::setup_uniform_values(Shader& shader)
{
    // Model
    glm::mat4 model = glm::mat4(1.0f);
    if (m_gui->get_auto_rotate())
        model = glm::rotate(model, 3.14f* sinf(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    // Camera/View transformation
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(m_camera->cameraPos, m_camera->cameraPos + m_camera->cameraFront, m_camera->cameraUp);
    // Projection
    glm::mat4 projection;
    projection = glm::perspective(m_camera->fov, (GLfloat)m_camera->width / (GLfloat)m_camera->height, m_camera->near, m_camera->far);
    // Get the uniform locations
    GLint modelLoc = glGetUniformLocation(shader.program, "model");
    GLint viewLoc = glGetUniformLocation(shader.program, "view");
    GLint projLoc = glGetUniformLocation(shader.program, "projection");
    GLint colorLoc = glGetUniformLocation(shader.program, "color");
    //directional light
    GLint directionLightDirLoc = glGetUniformLocation(shader.program, "directionLightDir");
    GLint directionAmbientStrengthLoc = glGetUniformLocation(shader.program, "directionAmbientStrength");
    GLint directionAmbientColorLoc = glGetUniformLocation(shader.program, "directionAmbientColor");
    GLint directionDiffuseColorLoc = glGetUniformLocation(shader.program, "directionDiffuseColor");
    GLint directionSpecularColorLoc = glGetUniformLocation(shader.program, "directionSpecularColor");
    //point light
    GLint lightPosLoc = glGetUniformLocation(shader.program, "pointLightPos");
    GLint pointAmbientStrengthLoc = glGetUniformLocation(shader.program, "pointAmbientStrength");
    GLint ambientColorLoc = glGetUniformLocation(shader.program, "pointAmbientColor");
    GLint diffuseColorLoc = glGetUniformLocation(shader.program, "pointDiffuseColor");
    GLint specularColorLoc = glGetUniformLocation(shader.program, "pointSpecularColor");
    GLint viewPosLoc = glGetUniformLocation(shader.program, "viewPos");
    GLint shininessLoc = glGetUniformLocation(shader.program, "shininess");
    
    // Pass the matrices to the shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(colorLoc, m_rendering_model->obj_color.x, m_rendering_model->obj_color.y, m_rendering_model->obj_color.z);
    glUniform3f(lightPosLoc, m_lighting->point_light.pos.x, m_lighting->point_light.pos.y, m_lighting->point_light.pos.z);
    glUniform3f(directionLightDirLoc, m_lighting->direction_light.dir.x, m_lighting->direction_light.dir.y, m_lighting->direction_light.dir.z);
    glUniform3f(viewPosLoc, m_camera->cameraPos.x, m_camera->cameraPos.y, m_camera->cameraPos.z);
    glUniform1i(shininessLoc, m_rendering_model->shininess);
    glUniform1f(directionAmbientStrengthLoc, m_lighting->direction_light.ambient_strength);
    glUniform1f(pointAmbientStrengthLoc, m_lighting->point_light.ambient_strength);
    if (m_gui->get_direction_light_enable()) {
        glUniform3f(directionAmbientColorLoc, m_lighting->direction_light.ambient_color.x, m_lighting->direction_light.ambient_color.y, m_lighting->direction_light.ambient_color.z);
        glUniform3f(directionDiffuseColorLoc, m_lighting->direction_light.diffuse_color.x, m_lighting->direction_light.diffuse_color.y, m_lighting->direction_light.diffuse_color.z);
        glUniform3f(directionSpecularColorLoc, m_lighting->direction_light.specular_color.x, m_lighting->direction_light.specular_color.y, m_lighting->direction_light.specular_color.z);
    } else {
        glUniform3f(directionAmbientColorLoc, 0.0f, 0.0f, 0.0f);
        glUniform3f(directionDiffuseColorLoc, 0.0f, 0.0f, 0.0f);
        glUniform3f(directionSpecularColorLoc, 0.0f, 0.0f, 0.0f);
    }
    if (m_gui->get_point_light_enable()) {
        glUniform3f(ambientColorLoc, m_lighting->point_light.ambient_color.x, m_lighting->point_light.ambient_color.y, m_lighting->point_light.ambient_color.z);
        glUniform3f(diffuseColorLoc, m_lighting->point_light.diffuse_color.x, m_lighting->point_light.diffuse_color.y, m_lighting->point_light.diffuse_color.z);
        glUniform3f(specularColorLoc, m_lighting->point_light.specular_color.x, m_lighting->point_light.specular_color.y, m_lighting->point_light.specular_color.z);
    } else {
        glUniform3f(ambientColorLoc, 0.0f, 0.0f, 0.0f);
        glUniform3f(diffuseColorLoc, 0.0f, 0.0f, 0.0f);
        glUniform3f(specularColorLoc, 0.0f, 0.0f, 0.0f);
    }
}

Object* Renderer::on_model_change(model_type type)
{
    m_rendering_model = m_res_manager->get_model(type);
    scene_reset();
    return m_rendering_model;
}

void Renderer::update()
{
    if (m_gui->get_point_light_enable() && (m_gui->get_point_light_rotate_x() || m_gui->get_point_light_rotate_y() || m_gui->get_point_light_rotate_z())) {
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, 0.0314f* sinf(glfwGetTime()), glm::vec3(m_gui->get_point_light_rotate_x(), m_gui->get_point_light_rotate_y(), m_gui->get_point_light_rotate_z()));
        m_lighting->point_light.pos = rotation * glm::vec4(m_lighting->point_light.pos, 1.0f);
    }
}
