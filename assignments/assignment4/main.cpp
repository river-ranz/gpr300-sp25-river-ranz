#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>
#include <ew/procGen.h>

#include "assets/animation.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();
void resetCamera(ew::Camera* camera, ew::CameraController* controller);

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

struct Material
{
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

ew::Camera camera;
ew::CameraController cameraController;

glm::vec3 lightPos(-0.25f, -1.0f, -0.5f);

float bias;

bool isPlaying = false, isLoop = false;
float playbackSpeed = 1.0f, playbackTime = 0.0f, duration = 0.0f;
int numPosFrames, numRotFrames, numScaleFrames;

AnimationClip clip;

GLuint depthMap;

int main() {
	GLFWwindow* window = initWindow("Assignment 4", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	// look at the center of the screen
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	// vertical field of view in degrees
	camera.fov = 60.0f;

	// depth testing
	glEnable(GL_DEPTH_TEST);

	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Transform planeTransform;
	planeTransform.position = glm::vec3(0, -2.0, 0);

	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader depthShader = ew::Shader("assets/depthShader.vert", "assets/depthShader.frag");

	ew::Model monkeyModel = ew::Model("assets/suzanne.fbx");
	ew::Transform monkeyTransform;

	GLuint texture = ew::loadTexture("assets/PavingStones.png");
	GLuint normalMap = ew::loadTexture("assets/PavingStones_normal.png");

	// depth map
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// 2D texture (using depth buffer)
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture to depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	// no color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// orthographic projection light source
	float nearPlane = 5.0f, farPlane = -2.0f;
	glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//RENDER

		cameraController.move(window, &camera, deltaTime);

		glBindTextureUnit(0, texture);
		glBindTextureUnit(1, normalMap);
		glBindTextureUnit(2, depthMap);

		// view matrix
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		// light space transformation matrix
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		// first pass
		// render to depth map
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		depthShader.setMat4("model", planeTransform.modelMatrix());
		planeMesh.draw();

		//monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

		depthShader.setMat4("model", monkeyTransform.modelMatrix());

		monkeyModel.draw();

		// render with shadow mapping
		glViewport(0, 0, screenWidth, screenHeight); // reset viewport
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f); // background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);

		shader.use();
		shader.setInt("_MainTex", 0);
		shader.setInt("normalMap", 1);
		shader.setInt("shadowMap", 2);
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setVec3("viewPos", camera.position);
		shader.setVec3("lightPos", lightPos);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);
		shader.setFloat("_Bias", bias);

		shader.setMat4("_Model", planeTransform.modelMatrix());
		planeMesh.draw();

		//monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

		shader.setMat4("_Model", monkeyTransform.modelMatrix());

		monkeyModel.draw();

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller)
{
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

void addFrame(int& frames) { frames++; }
void removeFrame(int& frames) { if (frames > 0) { frames--; } }

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
	if (ImGui::CollapsingHeader("Light Position"))
	{
		ImGui::SliderFloat("X", &lightPos.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Y", &lightPos.y, -1.0f, 1.0f);
		ImGui::SliderFloat("Z", &lightPos.z, -1.0f, 1.0f);
	}
	ImGui::SliderFloat("bias", &bias, 0.0, 0.5);
	if (ImGui::Button("Reset Camera"))
	{
		resetCamera(&camera, &cameraController);
	}
	ImGui::End();

	ImGui::Begin("Shadow Map");
	//child allows to fill all of the window
	ImGui::BeginChild("Shadow Map");
	// stretch image
	ImVec2 windowSize = ImGui::GetWindowSize();
	// invert 0-1 V to flip vertically
	ImGui::Image((ImTextureID)depthMap, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();

	ImGui::Begin("Animation");
	ImGui::Checkbox("Playing", &isPlaying);
	ImGui::Checkbox("Loop", &isLoop);
	ImGui::SliderFloat("Playback Speed", &playbackSpeed, -1.0f, 1.0f);
	ImGui::SliderFloat("Playback Time", &playbackTime, 0.0f, duration);
	ImGui::SliderFloat("Duration", &duration, 0.0f, 100.0f);
	ImGui::CollapsingHeader("Position Keys");
	{
		for (int i = 0; i < numPosFrames; i++)
		{
			ImGui::PushID(i);
			ImGui::SliderFloat("Time", &clip.positionKeys[i].time, 0.0f, duration);
			ImGui::SliderFloat("Value", &clip.positionKeys[i].value.x, -10.0f, 10.0f);
			ImGui::SliderFloat2("Value", &clip.positionKeys[i].value.y, -10.0f, 10.0f);
			ImGui::SliderFloat3("Value", &clip.positionKeys[i].value.z, -10.0f, 10.0f);
			ImGui::PopID();
		}
		ImGui::Button("Add Keyframe"); { addFrame(numPosFrames); }
		ImGui::Button("Remove Keyframe"); { removeFrame(numPosFrames); }
	}
	ImGui::CollapsingHeader("Rotation Keys");
	{
		for (int i = 0; i < numRotFrames; i++)
		{
			ImGui::PushID(i);
			ImGui::SliderFloat("Time", &clip.rotationKeys[i].time, 0.0f, duration);
			ImGui::SliderFloat("Value", &clip.rotationKeys[i].value.x, -10.0f, 10.0f);
			ImGui::SliderFloat2("Value", &clip.rotationKeys[i].value.y, -10.0f, 10.0f);
			ImGui::SliderFloat3("Value", &clip.rotationKeys[i].value.z, -10.0f, 10.0f);
			ImGui::PopID();
		}
		ImGui::Button("Add Keyframe"); { addFrame(numRotFrames); }
		ImGui::Button("Remove Keyframe"); { removeFrame(numRotFrames); }
	}
	ImGui::CollapsingHeader("Scale Keys");
	{
		for (int i = 0; i < numScaleFrames; i++)
		{
			ImGui::PushID(i);
			ImGui::SliderFloat("Time", &clip.scaleKeys[i].time, 0.0f, duration);
			ImGui::SliderFloat("Value", &clip.scaleKeys[i].value.x, -10.0f, 10.0f);
			ImGui::SliderFloat2("Value", &clip.scaleKeys[i].value.y, -10.0f, 10.0f);
			ImGui::SliderFloat3("Value", &clip.scaleKeys[i].value.z, -10.0f, 10.0f);
			ImGui::PopID();
		}
		ImGui::Button("Add Keyframe"); { addFrame(numScaleFrames); }
		ImGui::Button("Remove Keyframe"); { removeFrame(numScaleFrames); }
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}