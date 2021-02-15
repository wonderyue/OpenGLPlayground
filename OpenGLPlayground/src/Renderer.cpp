#include "Renderer.h"

Camera* Renderer::m_camera = new Camera();
ResourceManager* Renderer::m_res_manager = new ResourceManager();

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
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

	this->m_window = glfwCreateWindow(m_camera->width, m_camera->height, "OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(this->m_window);

	glewExperimental = GL_TRUE;
	glewInit();
    m_gui = new GUI(this->m_window, m_camera);
    m_gui->init(std::bind(&Renderer::on_model_change, this, std::placeholders::_1),
                std::bind(&Renderer::scene_reset, this));
}

void Renderer::display(GLFWwindow* window)
{
	Shader m_shader = Shader("shader/mvp.vert", "shader/basic.frag");
	// Main frame while loop
	while (!glfwWindowShouldClose(window))
	{
        // update delta time
        GLfloat current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

        m_gui->process_keyboard(delta_time);
		
		m_shader.use();
		
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
}

void Renderer::draw_scene(Shader& shader)
{
	// Set up some basic parameters
	glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
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
    // Pass the matrices to the shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(colorLoc, (GLfloat)m_gui->get_color_val().r(), (GLfloat)m_gui->get_color_val().g(), (GLfloat)m_gui->get_color_val().b());
}

void Renderer::on_model_change(model_type type)
{
    m_rendering_model = m_res_manager->get_model(type);
    scene_reset();
}
