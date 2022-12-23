#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h" included in model.h

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera globals
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); //relative to pos
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true; // So the camera doesn't jump suddenly when window receives focus
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0; //previous frame's mouse position
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// stores how much we're seeing of either texture
float mixValue = 0.2f;

int main()
{
	// GLFW: init, configure, and create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //for Mac OS X

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL); //create window
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Window resize callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Window mouse and scroll callbacks
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD: initialize GLAD before calling any OpenGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	//unsigned int vertexShader; // ID of the shader object
	//vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//// Attach shader sourcecode to the shader object, then compile it (during runtime).
	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glCompileShader(vertexShader);

	//glUseProgram(shaderProgram); //After this call, all rendering calls will use this program object.

	//// Delete the individual shader objects as they're now linked to the Program object.
	//glDeleteShader(vertexShader);


	//// Hello SQUARE!!!!!!!!!!!

	////float vertices[] = {
	////     0.5f,  0.5f, 0.0f,  // top right
	////     0.5f, -0.5f, 0.0f,  // bottom right
	////    -0.5f, -0.5f, 0.0f,  // bottom left
	////    -0.5f,  0.5f, 0.0f   // top left 
	////};
	// // for indexed drawing:
	//unsigned int indices[] = {  // note that we start from 0!
	//    0, 1, 3,   // first triangle
	//    1, 2, 3    // second triangle
	//};

	//// Two triangles -- render 6 vertices, remember to change parameter in glDrawArrays
	//float vertices[] = {
	//    // first triangle
	//    -0.9f, -0.5f, 0.0f,  // left 
	//    -0.0f, -0.5f, 0.0f,  // right
	//    -0.45f, 0.5f, 0.0f,  // top 
	//    // second triangle
	//     0.0f, -0.5f, 0.0f,  // left
	//     0.9f, -0.5f, 0.0f,  // right
	//     0.45f, 0.5f, 0.0f   // top 
	//};

	// Shader program setup
	// ---------------------------------------

	Shader lightingShader("3_3_shader.vs", "3_3_shader.fs"); // Shader class replaces shader program setup above. (Compiles the shaders, create shader program object, etc)

	Shader lightCubeShader("light_cube.vs", "light_cube.fs"); //For drawing a cube at the light source, independent of the other objects to draw

	// load models
	// -----------
	Model ourModel("assets/backpack/backpack.obj");

	//float vertices[] = {
	//    // positions          // colors           // texture coords
	//     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	//     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	//    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	//    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	//};



	// Cube: 36 vertices 12 triangles. Also can't use EBO (something to do with texture coords not being unique on a vertex)
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	float floorVertices[] = { //lol its literally just a flat square. could literally just reuse the cube except only draw one face
		// positions          // normals           // texture coords
		-0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //topleft triangle
		-0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 10.0f,
		 0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  10.0f,10.0f,
		 0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  10.0f,10.0f, //botright triangle
		 0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  10.0f, 0.0f,
		-0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,

	};



	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f), //this one is moving
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO; //vertex buffer object
	glGenBuffers(1, &VBO);

	unsigned int EBO; //Element array buffer, for indexed drawing!!!
	glGenBuffers(1, &EBO);

	// use VAO to save configuration settings, allowing drawing multiple objects without having to bind each
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// 1. bind vao. Need to do this before VBO and attribute setup stuff so that those settings are stored in the VAO.
	glBindVertexArray(VAO);

	// 2. bind vbo. copy our vertices[] array in the vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices[] to VBO (to RAM). Static b/c it won't move and it will be used a lot

	// 3. bind ebo. copy index array to element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); //copy incides[] to EBO

	// 4. then set our vertex attributes pointers. index, size, type, normalized, stride, pointer
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// (unused) color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);


	// Normal vector attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind the VBO as it's already stored in VAO thru the glVertexattribpointer call.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// However, do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound. (?)
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	// Wireframe: uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	////OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image, but images usually have 0.0 at the top of the y-axis.
	////So it should be flipped vertically:
	//stbi_set_flip_vertically_on_load(true);


	// Light cube VAO
	// ------------------------------
	// 
	// configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the previously used VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //stride of 8, to ignore the texture data. we only need position data.
	glEnableVertexAttribArray(0);



	// xd Floor VAO and VBO
	// --------------------------------
	unsigned int floorVAO;
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	unsigned int floorVBO;
	glGenBuffers(1, &floorVBO);

	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//unbind vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::cout << "YES";


	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------

	unsigned int diffuseMap = loadTexture("assets/container2.png");
	unsigned int specularMap = loadTexture("assets/container2_specular.png");
	//unsigned int specularMap = loadTexture("lighting_maps_specular_color.png"); //choppy specular highlight
	unsigned int emissionMap = loadTexture("assets/matrix.jpg");
	unsigned int floorDiffuseMap = loadTexture("assets/GRASS.jpg");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0); //diffuse map sampler
	lightingShader.setInt("material.specular", 1); //specular map sampler
	lightingShader.setInt("material.emission", 2); //emission map sampler


	// Render loop

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw our first triangle
		//glUseProgram(shaderProgram);

		//move one of the point lights in a circle
		pointLightPositions[1].x = 5.0f * sin(glfwGetTime());
		pointLightPositions[1].z = 10.0f * cos(glfwGetTime());


		// Set uniforms for lighting calculations
		lightingShader.use();
		//ourShader.setFloat("mixValue", mixValue);         // set the texture mix value in the shader
		lightingShader.setVec3("viewPos", cameraPos); //if fs in view space, dont need this
		lightingShader.setFloat("material.shininess", 32.0f);

		  /*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.1f, 0.1f, 0.1f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		//lightingShader.setVec3("spotLight.position", cameraPos);
		//lightingShader.setVec3("spotLight.direction", cameraFront);
		lightingShader.setVec3("spotLight.position", glm::vec3(10.0f, -4.0f, 3.0f));
		lightingShader.setVec3("spotLight.direction", glm::vec3(1, 0, 0));
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 0.0f, -3.0f, 10.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cosCutoff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.cosOuterCutoff", glm::cos(glm::radians(15.0f)));


		// create transformations for perspective drawing
		//glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		//glm::mat4 view = glm::mat4(1.0f);
		//glm::mat4 projection = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate model -55 deg around x axis. and keep pos at origin
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f)); //Rotate over time
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); //move scene 3 units away.
		//projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); //45 deg FOV


		// retrieve the matrix uniform locations, unneeded since using shader class.
 /*     unsigned int modelLoc = glGetUniformLocation(lightingShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");*/

		// pass them to the shaders (3 different ways)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		//glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

		// camera/view: dependent on cameraPos, which is affected by WASD input
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		lightingShader.setMat4("view", view);

		// pass projection matrix to shader (note that in this case it could change every frame, b/c we're supporting zooming in/out.)
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		lightingShader.setMat4("projection", projection);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		// bind emission map
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, emissionMap);

		// Render container
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3); //draws VBO. params are (first, count)
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //draws using EBO and VBO
		//glDrawArrays(GL_TRIANGLES, 0, 36); //Draw cube.

		glm::mat4 model = glm::mat4(1.0f);
		// Draw 10 cubes in different positions/orientation. Accomplished by varying the model matrices among them.
		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0f); // init to identity matrix
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i + 20 * glfwGetTime();
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Draw cube with no rotation or translation from its default pos
		//glm::mat4 model = glm::mat4(1.0f);
		//lightingShader.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//xd draw floor
		glBindVertexArray(floorVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);
		glActiveTexture(GL_TEXTURE1);//unbind specular
		glBindTexture(GL_TEXTURE_2D, 0);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, -5, 0));
		model = glm::scale(model, glm::vec3(100));
		model = glm::rotate(model, (float)glfwGetTime() * 0.1f, glm::vec3(1, 0, 0));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// render the loaded model
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, (float)glfwGetTime() * 5.0f, glm::vec3(0, 1, 0)); //change of basis
		lightingShader.setMat4("model", model);
		ourModel.Draw(lightingShader);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		lightingShader.setMat4("model", model);
		ourModel.Draw(lightingShader);
		model = glm::translate(model, glm::vec3(-3.0f, -4.0f, 5.0f));
		lightingShader.setMat4("model", model);
		ourModel.Draw(lightingShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, (float)sin(glfwGetTime()) * 5.0f));
		lightingShader.setMat4("model", model);
		ourModel.Draw(lightingShader);

		cout << glfwGetTime();


		//Use the lightCubeShader to draw the point light sources
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}





		//// (before we did model*view*proj, just transformation fun)
		// second transformation: draw the same vertices but translated in another direction. (and with cool sine-wave scaling.)
		//transform = glm::mat4(1.0f); // reset it to identity matrix
		//transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
		//float scaleAmount = sin(glfwGetTime());
		//transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]); // this time take the matrix value array's first element as its memory pointer value (?WHY??? just another option)
		//// now with the uniform matrix being replaced with new transformations, draw it again.
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//glBindVertexArray(0); // no need to unbind it every time 



		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Free resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	//glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}

	// WASD camera
	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Calculate the final direction the camera points toward
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
