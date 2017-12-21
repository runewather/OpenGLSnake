#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WIDTH 800
#define HEIGHT 600

//Vertex Shader
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 ourColor1;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"	//ourColor1 = aPos;\n"
"	TexCoord = aTexCoord;\n"
"}\0";

//Fragment Shader - Orange Color
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform float visible_param;\n"
"uniform sampler2D ourTexture;\n"
"uniform sampler2D ourTexture1;\n"
"void main()\n"
"{\n"
"   FragColor = mix(texture(ourTexture, TexCoord * 2).rgba, texture(ourTexture1, TexCoord * 2).rgba, vec4(texture(ourTexture, TexCoord)).a * visible_param);\n"
"   //FragColor = texture(ourTexture1, TexCoord);\n"
"}\n\0";

//Fragment Shader - Yellow Color
const char *fragmentShaderSource1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor1;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor1, 1.0);\n"
"}\n\0";

//Visible var
float visibleParam = 0.0f;

//Delta Time
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

//Camera walk around
float cameraFov = 45.0f;
float cameraSpeed = 0.5f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Mouse variables
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

//Call this function when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Clamp function
float Clamp(float min, float max, float value)
{
	if (value < min)
	{
		return min;
	}
	if (value > max)
	{
		return max;
	}
	return value;
}

//Process Inputs
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		visibleParam += 0.0001f;
		visibleParam = Clamp(0.0f, 1.0f, visibleParam);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		visibleParam -= 0.0001f;
		visibleParam = Clamp(0.0f, 1.0f, visibleParam);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraFront * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraFront * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
	cameraPos.y = 0.0f;
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos) 
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xoffset = xPos - lastX;
	float yoffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	float sensitivity = 0.5f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}
	
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (cameraFov >= 1.0f && cameraFov <= 45.0f)
	{
		cameraFov -= yoffset;
		std::cout << cameraFov << std::endl;
		std::cout << yoffset << std::endl;
	}
	if (cameraFov <= 1.0f)
	{
		cameraFov = 1.0f;
	}
	if (cameraFov >= 45.0f)
	{
		cameraFov = 45.0f;
	}	
}

int main()
{
	//Init GLFW
	glfwInit();
	//Set OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
	//Create Window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	//Set the window to context
	glfwMakeContextCurrent(window);

	//Enable mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	//Set mouse callback function
	glfwSetCursorPosCallback(window, mouse_callback);

	//Set scroll callback function
	glfwSetScrollCallback(window, scroll_callback);
			
	//Init GLAD
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	//Set Viewport
	glViewport(0, 0, WIDTH, HEIGHT);

	//Enable z-buffer
	glEnable(GL_DEPTH_TEST);

	//Set callback function to adjust screen when resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Compiling vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//Check if vertex shader is ok
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR: VERTEX SHADER COMPILATION FAILED\n" << infoLog << std::endl;
	}

	//Compiling fragment shader 0 
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Compiling fragment shader 1 
	//unsigned int fragmentShader1;
	//fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
	//glCompileShader(fragmentShader1);

	//Creating program object 0
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	//Creating program object 1
	unsigned int shaderProgram1;
	shaderProgram1 = glCreateProgram();

	//Setting shaders to program 0
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//Setting shaders to program 1
	//glAttachShader(shaderProgram1, vertexShader);
	//glAttachShader(shaderProgram1, fragmentShader1);
	//glLinkProgram(shaderProgram1);

	//Check if liking shaders to program is ok
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR: FRAGMENT SHADER COMPILATION FAILED!" << infoLog << std::endl;
	}

	//Deleting shaders objects after liking them with the program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//glDeleteShader(fragmentShader1);

	//Load texture 0 
	int textureWidth;
	int textureHeight;
	int nrChannels;
	unsigned char *data = stbi_load("Textures/wall.jpg", &textureWidth, &textureHeight, &nrChannels, 0);
	if (!data)
	{
		std::cout << "ERROR: FAILED TO LOAD TEXTURE" << std::endl;
		return 0;
	}

	//Generate Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	//Load texture 1
	int texture1Width;
	int texture1Height;
	int nrChannels1;
	unsigned char *data1 = stbi_load("Textures/awesomeface.png", &texture1Width, &texture1Height, &nrChannels1, 0);
	if (!data)
	{
		std::cout << "ERROR: FAILED TO LOAD TEXTURE" << std::endl;
		return 0;
	}

	//Generate Texture 2
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture1Width, texture1Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data1);

	//Set up vertex data

	//Cube Vertices
	float cube[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	//Cube translations
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

	//Triangle Vertices
	float triangle1[] = {
		-1.0f, -0.5f, 0.0f,
		0.0f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
	};

	float triangle2[] = {
		0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};	

	//Rectangle Vertices
	float rectangle[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f
	};

	//Texture coords
	float texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f
	};

	//Rectangle Indicies
	unsigned int indicies[] = {
		0, 1, 2,
		1, 2, 3
	};

	unsigned int VBO, VAO, EBO;
	unsigned int VBO1, VAO1;

	//VAO 0
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	//Send data to VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	//Send data to EBO buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	//Set data format and location
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	//Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//VAO 1
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);

	//Send data to VBO1
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);

	//Set data format and location
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	//Transformations
	//Model
	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.6f, 0.2f));

	//View
	glm::mat4 view;
	
	//Projection
	glm::mat4 projection;
	

	//Main loop
	while (!glfwWindowShouldClose(window))
	{
		//Calculate delta time
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		//Input
		processInput(window);


		//Redering
		//Clear Screen
		glClearColor(0.19f, 0.3f, 0.47f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Bind Textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		//Transformations
		float time = glfwGetTime();		
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		int modelLocation = glGetUniformLocation(shaderProgram, "model");
		int viewLocation = glGetUniformLocation(shaderProgram, "view");
		int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
		model = glm::rotate(model, glm::radians(0.01f), glm::vec3(1.0f, 0.0f, 0.0f));
		projection = glm::perspective(glm::radians(cameraFov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);

		int textureLocation = glGetUniformLocation(shaderProgram, "ourTexture");
		int texture1Location = glGetUniformLocation(shaderProgram, "ourTexture1");
		int visibleParamLocation = glGetUniformLocation(shaderProgram, "visible_param");				
		glUniform1i(textureLocation, 0);
		glUniform1i(texture1Location, 1);
		glUniform1f(visibleParamLocation, visibleParam);

		//Set shader program
		glUseProgram(shaderProgram);	
		unsigned int transformMatLocation = glGetUniformLocation(shaderProgram, "trans");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));		
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

		//Setting fragment shader parameters		
		float greenValue = (sin(time) / 2.0f);		
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		//int vertexOffsetLocation = glGetUniformLocation(shaderProgram, "offset");
		//glUniform4f(vertexOffsetLocation, -0.25f, 0.0f, 0.0f, 1.0f);		
				
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glUseProgram(shaderProgram1);
		//glBindVertexArray(VAO1);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 0.0f;
			if (i % 3 == 0)
			{
				angle = 20.0f * i * glfwGetTime();
			}
			else
			{
				angle = 25.0f * i;
			}			
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Swap buffers
		glfwSwapBuffers(window);
		//Check and call events
		glfwPollEvents();
	}

	//Delete all resources that were allocated
	glfwTerminate();
	return 0;
}