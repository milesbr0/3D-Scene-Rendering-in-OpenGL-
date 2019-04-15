#include "Includes.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Camera settings
//							  width, heigh, near plane, far plane
Camera_settings camera_settings{ 1200, 1000, 0.1, 100.0 };

//Timer
Timer timer;

// Instantiate the camera object with basic data
Camera camera(camera_settings, glm::vec3(0.0, 4.0, 20.0));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

bool pause = false;


int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "17065178 Graphics Coursework", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Rendering settings
	glfwSwapInterval(1);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);	//Enables depth testing
	glEnable(GL_CULL_FACE);		//Enables face culling
	glFrontFace(GL_CCW);		//Specifies which winding order if front facing

	////	Shaders - Textures - Models	////

	GLuint basicShader;
	GLuint specTextureShader;

	// Texture container
	GLuint jadeTexture;
	GLuint snowTex;
	GLuint metalTexSpec;
	GLuint emeraldTexture;
	GLuint pilarTexture1;
	GLuint pilarTexture2;
	GLuint pilarTexture3;
	GLuint emeraldTex;
	GLuint goldTexture;
	GLuint bronzeTexture;
	GLuint pearlTexture;
	GLuint plasticRedTexture;
	GLuint rubberBlackTexture;

	// build and compile our shader program
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Basic_shader.vert"), 
		string("Resources\\Shaders\\Basic_shader.frag"),
		&basicShader);
	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\SpecTexture_Shader.vert"),
		string("Resources\\Shaders\\SpecTexture_Shader.frag"),
		&specTextureShader);
	
	Model emerald("Resources\\Models\\Emerald.obj");
	Model knife("Resources\\Models\\knife.obj");
	Model spoon("Resources\\Models\\spoon.obj");
	Model emerald3("Resources\\Models\\Emerald.obj");
	Model emerald4("Resources\\Models\\Emerald.obj");
	Model emerald5("Resources\\Models\\Emerald.obj");
	Model emerald6("Resources\\Models\\Emerald.obj");
	Model plane("Resources\\Models\\Plane.obj");
	Model pilar1("Resources\\Models\\pilar.obj");
	Model pilar2("Resources\\Models\\pilar.obj");
	Model pilar3("Resources\\Models\\pilar.obj");
	

	jadeTexture = TextureLoader::loadTexture("Resources\\Models\\jade_texture2.jpg");
	snowTex = TextureLoader::loadTexture("Resources\\Models\\snow_texture.jpg");
	metalTexSpec = TextureLoader::loadTexture("Resources\\Models\\metal_texture_specular.png");
	emeraldTexture = TextureLoader::loadTexture("Resources\\Models\\emerald.png");
	pilarTexture1 = TextureLoader::loadTexture("Resources\\Models\\blue.png");
	pilarTexture2 = TextureLoader::loadTexture("Resources\\Models\\brick_texture1.jpg");
	pilarTexture3 = TextureLoader::loadTexture("Resources\\Models\\brick_texture2.jpg");
	bronzeTexture = TextureLoader::loadTexture("Resources\\Models\\bronze_texture.jpg");
	pearlTexture = TextureLoader::loadTexture("Resources\\Models\\pearl_texture.png");
	plasticRedTexture = TextureLoader::loadTexture("Resources\\Models\\plastic_red.jpg");
	rubberBlackTexture = TextureLoader::loadTexture("Resources\\Models\\rubber_black.jpeg");





	pilar1.attachTexture(pilarTexture1);
	pilar2.attachTexture(pilarTexture2);
	pilar3.attachTexture(pilarTexture3);
	emerald.attachTexture(emeraldTexture);
	knife.attachTexture(pearlTexture);
	spoon.attachTexture(jadeTexture);
	emerald3.attachTexture(bronzeTexture);
	emerald4.attachTexture(pearlTexture);
	emerald5.attachTexture(plasticRedTexture);
	emerald6.attachTexture(rubberBlackTexture);

	plane.attachTexture(snowTex);

	//Light Data///////////////////////////////////////////////
	// Lights
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };	// Dim light 
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };	// White main light 
	GLfloat light_position[] = { 0.0, 3.0, 0.0, 1.0 };	// Point light (w=1.0)
	GLfloat attenuation[] = { 1.0, 0.10, 0.08 };



	// Materials
	GLfloat mat_amb_diff[] = { 1.0, 1.0, 1.0, 1.0 };	// Texture map will provide ambient and diffuse.
	GLfloat mat_specularCol[] = { 1.0, 1.0, 1.0, 1.0 }; // White highlight
	GLfloat mat_specularExp = 32.0;			// Shiny surface


	//Uniform Locations - Basic Shader////////////////////////////////////////////
	// Get unifom locations in shader
	GLuint uLightAmbient = glGetUniformLocation(basicShader, "lightAmbient");
	GLuint uLightDiffuse = glGetUniformLocation(basicShader, "lightDiffuse");
	GLuint uLightAttenuation = glGetUniformLocation(basicShader, "lightAttenuation");
	GLuint uLightPosition = glGetUniformLocation(basicShader, "lightPosition");
	GLuint uEyePos = glGetUniformLocation(basicShader, "eyePos");
	//Uniform Locations - Specular Texture Shader///////////////////////////////////
	GLuint uLightPositionSpec = glGetUniformLocation(specTextureShader, "lightPosition");
	GLuint uLightAmbientSpec = glGetUniformLocation(specTextureShader, "lightAmbient");
	GLuint uLightDiffuseSpec = glGetUniformLocation(specTextureShader, "lightDiffuse");
	GLuint uLightAttenuationSpec = glGetUniformLocation(specTextureShader, "lightAttenuation");
	GLuint uEyePosSpec = glGetUniformLocation(specTextureShader, "eyePos");



	// Get material unifom locations in shader
	GLuint uMatAmbient = glGetUniformLocation(basicShader, "matAmbient");
	GLuint uMatDiffuse = glGetUniformLocation(basicShader, "matDiffuse");
	GLuint uMatSpecularCol = glGetUniformLocation(basicShader, "matSpecularColour");
	GLuint uMatSpecularExp = glGetUniformLocation(basicShader, "matSpecularExponent");

	// Get material unifom locations in shader
	GLuint uMatAmbientSpec = glGetUniformLocation(specTextureShader, "matAmbient");
	GLuint uMatDiffuseSpec = glGetUniformLocation(specTextureShader, "matDiffuse");
	GLuint uMatSpecularColSpec = glGetUniformLocation(specTextureShader, "matSpecularColour");
	GLuint uMatSpecularExpSpec = glGetUniformLocation(specTextureShader, "matSpecularExponent");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		static float phase = 0.0f; // variable to store the incrementing value
		if(!pause)
			phase += timer.getDeltaTimeSeconds() * glm::pi<float>() * 4.0;

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 knifeModel = glm::mat4(1.0);
		glm::mat4 spoonModel = glm::mat4(1.0);
		glm::mat4 emeraldModel = glm::mat4(1.0);
		glm::mat4 bulletModel = glm::mat4(1.0);
		glm::mat4 boxModel = glm::mat4(1.0);
		glm::mat4 pillarModel = glm::mat4(1.0);
		glm::mat4 planeModel = glm::mat4(1.0);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();

		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(2, 2, 2));
		glm::vec3 eyePos = camera.getCameraPosition();


		glUseProgram(basicShader); //Use the Basic shader

		//Pass the uniform data to Basic shader///////////////////////////////////
		//Pass the light data
		glUniform4fv(uLightAmbient, 1, (GLfloat*)&light_ambient);
		glUniform4fv(uLightDiffuse, 1, (GLfloat*)&light_diffuse);
		glUniform4fv(uLightPosition, 1, (GLfloat*)&light_position);
		glUniform3fv(uLightAttenuation, 1, (GLfloat*)&attenuation);
		glUniform3fv(uEyePos, 1, (GLfloat*)&eyePos);


		//Pass material data
		glUniform4fv(uMatAmbient, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(uMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(uMatSpecularCol, 1, (GLfloat*)&mat_specularCol);
		glUniform1f(uMatSpecularExp, mat_specularExp);


		glUniformMatrix4fv(glGetUniformLocation(basicShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));
		plane.draw(basicShader); //Draw the plane

		emeraldModel = glm::translate(glm::mat4(1.0), glm::vec3(-10.0, 3.0, -5.0));
		emeraldModel = glm::rotate(emeraldModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(emeraldModel));
		emerald.draw(basicShader); //Draw emerald

		knifeModel = glm::translate(glm::mat4(1.0), glm::vec3(-7.0, 3.8, -2.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));
		knifeModel = glm::rotate(knifeModel, glm::radians(phase), glm::vec3(0.0, -1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(knifeModel));
		knife.draw(basicShader);

		spoonModel = glm::translate(glm::mat4(1.0), glm::vec3(-4.0, 3.8, -5.0))* glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));;
		spoonModel = glm::rotate(spoonModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(spoonModel));
		spoon.draw(basicShader);

		emeraldModel = glm::translate(glm::mat4(1.0), glm::vec3(-1.0, 3.0, -7.0));
		emeraldModel = glm::rotate(emeraldModel, glm::radians(phase), glm::vec3(0.0, -1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(emeraldModel));
		emerald3.draw(basicShader);

		boxModel = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 3.0, -5.0));
		boxModel = glm::rotate(boxModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(boxModel));
		emerald4.draw(basicShader);

		boxModel = glm::translate(glm::mat4(1.0), glm::vec3(5.0, 3.0, -2.0));
		boxModel = glm::rotate(boxModel, glm::radians(phase), glm::vec3(0.0, -1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(boxModel));
		emerald5.draw(basicShader);

		boxModel = glm::translate(glm::mat4(1.0), glm::vec3(8.0, 3.0, -5.0));
		boxModel = glm::rotate(boxModel, glm::radians(phase), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(boxModel));
		emerald6.draw(basicShader);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////DRAW PILARS/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		glUseProgram(basicShader);

		glUniform4fv(uLightAmbient, 1, (GLfloat*)&light_ambient);
		glUniform4fv(uLightDiffuse, 1, (GLfloat*)&light_diffuse);
		glUniform4fv(uLightPosition, 1, (GLfloat*)&light_position);
		glUniform3fv(uLightAttenuation, 1, (GLfloat*)&attenuation);
		glUniform3fv(uEyePos, 1, (GLfloat*)&eyePos);
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		pillarModel = glm::translate(glm::mat4(1.0), glm::vec3(-10.0, 0.0, -5.0)) * scaleMat;
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(pillarModel));
		pilar1.draw(basicShader); //Draw pilar1

		pillarModel = glm::translate(glm::mat4(1.0), glm::vec3(-7.0, 0.0, -2.0)) * scaleMat;
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(pillarModel));
		pilar2.draw(basicShader); //Draw pilar1

		pillarModel = glm::translate(glm::mat4(1.0), glm::vec3(-4.0, 0.0, -5.0)) * scaleMat;
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(pillarModel));
		pilar3.draw(basicShader); //Draw pilar1

		pillarModel = glm::translate(glm::mat4(1.0), glm::vec3(-1.0, 0.0, -7.0)) * scaleMat;
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(pillarModel));
		pilar1.draw(basicShader); //Draw pilar1

		pillarModel = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.0, -5.0)) * scaleMat;
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(pillarModel));
		pilar2.draw(basicShader); //Draw pilar1

		pillarModel = glm::translate(glm::mat4(1.0), glm::vec3(5.0, 0.0, -2.0)) * scaleMat;
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(pillarModel));
		pilar3.draw(basicShader); //Draw pilar1

		pillarModel = glm::translate(glm::mat4(1.0), glm::vec3(8.0, 0.0, -5.0)) * scaleMat;
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(pillarModel));
		pilar1.draw(basicShader); //Draw pilar1


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////DRAW PILARS/////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		pause = !pause;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		camera.processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}