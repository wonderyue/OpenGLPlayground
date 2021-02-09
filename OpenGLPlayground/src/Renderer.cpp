#include "Renderer.h"

Camera* Renderer::m_camera = new Camera();

nanogui::Screen* Renderer::m_nanogui_screen = nullptr;


/*
 * TODO: Deprecate these
 * 		and create global variables for your GUI
 */
bool bvar = true;
int ivar = 12345678;
double dvar = 3.1415926;
float fvar = (float)dvar;
std::string strval = "A string";
nanogui::Color col_val(1.0f, 1.0f, 1.0f, 1.0f);
bool reload_model = false;


// Pre-defined
bool Renderer::keys[1024];

enum render_type {
	POINT = 0,
	LINE = 1,
	TRIANGLE = 2
};
enum culling_type {
	CW = 0,
	CCW = 1
};
enum shading_type {
	SMOOTH = 0,
	FLAT = 1,
};
enum depth_type {
	LESS = 0,
	ALWAYS = 1,
};

std::unordered_map<render_type, int> render_type_2_gl_type = {{POINT, GL_POINT}, {LINE, GL_LINE}, {TRIANGLE, GL_FILL}};

render_type render_val = LINE;
culling_type culling_val = CW;
shading_type shading_val = SMOOTH;
depth_type depth_val = LESS;

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

	nanogui_init(this->m_window);
}

void Renderer::nanogui_init(GLFWwindow* window)
{
	m_nanogui_screen = new nanogui::Screen();
	m_nanogui_screen->initialize(window, true);

	glViewport(0, 0, m_camera->width, m_camera->height);

	// Create nanogui gui
	nanogui::FormHelper* gui = new nanogui::FormHelper(m_nanogui_screen);
	nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(0, 0), "Nanogui control bar_1");

	/*
	 *	TODO: Define your GUI components here 
	 */ 
    bool enabled = true;
    gui->addGroup("Basic types");
    gui->addVariable("bool", bvar)->setTooltip("Test tooltip.");
    gui->addVariable("string", strval);

    gui->addGroup("Validating fields");
    gui->addVariable("int", ivar)->setSpinnable(true);
    gui->addVariable("float", fvar)->setTooltip("Test.");
    gui->addVariable("double", dvar)->setSpinnable(true);

    gui->addGroup("Complex types");
    gui->addVariable("Render Mode", render_val, enabled)->setItems({ "Point", "Line", "Trangle" });
    gui->addVariable("Color", col_val)
       ->setFinalCallback([](const nanogui::Color &c) {
             std::cout << "ColorPicker Final Callback: ["
                       << c.r() << ", "
                       << c.g() << ", "
                       << c.b() << ", "
                       << c.w() << "]" << std::endl;
         });

    gui->addGroup("Other widgets");
    gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; })->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");
    
	m_nanogui_screen->setVisible(true);
	m_nanogui_screen->performLayout();

	glfwSetCursorPosCallback(window,
		[](GLFWwindow* window, double x, double y) {
			m_nanogui_screen->cursorPosCallbackEvent(x, y);
		}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow*, int button, int action, int modifiers) {
			m_nanogui_screen->mouseButtonCallbackEvent(button, action, modifiers);
		}
	);

	glfwSetKeyCallback(window,
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

	glfwSetCharCallback(window,
		[](GLFWwindow*, unsigned int codepoint) {
			m_nanogui_screen->charCallbackEvent(codepoint);
		}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow*, int count, const char** filenames) {
			m_nanogui_screen->dropCallbackEvent(count, filenames);
		}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow*, double x, double y) {
			m_nanogui_screen->scrollCallbackEvent(x, y);
			//m_camera->ProcessMouseScroll(y);
		}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow*, int width, int height) {
			m_nanogui_screen->resizeCallbackEvent(width, height);
		}
	);
}

void Renderer::display(GLFWwindow* window)
{
	Shader m_shader = Shader("shader/mvp.vert", "shader/basic.frag");
    Object* obj = load_models();
    glEnable(GL_DEPTH_TEST | GL_CULL_FACE);
	// Main frame while loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		camera_move();
		
		m_shader.use();
		
		setup_uniform_values(m_shader);

		draw_scene(m_shader);
        
        glPolygonMode(GL_FRONT_AND_BACK, render_type_2_gl_type[render_val]);
        draw_object(m_shader, *obj);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_nanogui_screen->drawWidgets();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
    delete obj;
	return;
}

void Renderer::scene_reset()
{
	m_camera->reset();
}

Object* Renderer::load_models()
{
    Object* obj = new Object("model/cube.obj");
    glGenVertexArrays(1, &obj->vao);
    glGenBuffers(1, &obj->vbo);
    glGenBuffers(1, &obj->ebo);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(obj->vao);
    // Bind vertices to Array Buffer
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Object::Vertex) * obj->vao_vertices.size(), &obj->vao_vertices[0], GL_STATIC_DRAW);

    // Bind indices to Element Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->veo_indices.size(), &obj->veo_indices[0], GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, Normal));
    glEnableVertexAttribArray(1);
    // Texture Coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO
	/*
	* TODO: You can also set Camera parameters here
	*/
    return obj;
}

void Renderer::draw_scene(Shader& shader)
{
	// Set up some basic parameters
	glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Renderer::draw_object(Shader& shader, Object& object)
{
	glBindVertexArray(object.vao);
    glDrawElements(GL_TRIANGLES, object.veo_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Renderer::setup_uniform_values(Shader& shader)
{
    // Model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, 3.14f* sinf(glfwGetTime()), glm::vec3(1.0f, 1.0f, 0.0f));
    // Camera/View transformation
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(m_camera->cameraPos, m_camera->cameraPos + m_camera->cameraFront, m_camera->cameraUp);
    // Projection
    glm::mat4 projection;
    projection = glm::perspective(45.0f, (GLfloat)m_camera->width / (GLfloat)m_camera->height, 0.1f, 100.0f);

    // Get the uniform locations
    GLint modelLoc = glGetUniformLocation(shader.program, "model");
    GLint viewLoc = glGetUniformLocation(shader.program, "view");
    GLint projLoc = glGetUniformLocation(shader.program, "projection");
    // Pass the matrices to the shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Renderer::camera_move()
{
	GLfloat current_frame = glfwGetTime();
	delta_time = current_frame - last_frame;
	last_frame = current_frame;
	// Camera controls
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
