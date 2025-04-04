#include <stdio.h>
#include <math.h>
#include <vector>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/fwd.hpp>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>
#include <ew/procGen.h>

#include "rivLib/forwardK.h"

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

GLuint depthMap;

riv::Skeleton skeleton;
riv::Joint torso, shoulder_R, elbow_R, wrist_R, shoulder_L, elbow_L, wrist_L;

int main() {
	GLFWwindow* window = initWindow("Assignment 6", screenWidth, screenHeight);
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
	
	// left side
	wrist_L.parent = new riv::Joint;
	*wrist_L.parent = elbow_L;
	wrist_L.parentIndex = 3;

	elbow_L.parent = new riv::Joint;
	*elbow_L.parent = shoulder_L;
	elbow_L.children = new riv::Joint*[1];
	elbow_L.children[0] = new riv::Joint;
	*elbow_L.children[0] = wrist_L;
	elbow_L.numChildren = 1;
	elbow_L.parentIndex = 1;

	shoulder_L.parent = new riv::Joint;
	*shoulder_L.parent = torso;
	shoulder_L.children = new riv::Joint*[1];
	shoulder_L.children[0] = new riv::Joint;
	*shoulder_L.children[0] = elbow_L;
	shoulder_L.numChildren = 1;
	shoulder_L.parentIndex = 0;

	// right side
	wrist_R.parent = new riv::Joint;
	*wrist_R.parent = elbow_R;
	wrist_R.parentIndex = 4;

	elbow_R.parent = new riv::Joint;
	*elbow_R.parent = shoulder_R;
	elbow_R.children = new riv::Joint * [1];
	elbow_R.children[0] = new riv::Joint;
	*elbow_R.children[0] = wrist_R;
	elbow_R.numChildren = 1;
	elbow_R.parentIndex = 2;

	shoulder_R.parent = new riv::Joint;
	*shoulder_R.parent = torso;
	shoulder_R.children = new riv::Joint * [1];
	shoulder_R.children[0] = new riv::Joint;
	*shoulder_R.children[0] = elbow_R;
	shoulder_R.numChildren = 1;
	shoulder_R.parentIndex = 0;

	// torso
	torso.children = new riv::Joint*[2];
	torso.children[0] = new riv::Joint;
	*torso.children[0] = shoulder_L;
	torso.children[1] = new riv::Joint;
	*torso.children[1] = shoulder_R;
	torso.numChildren = 2;

	// skeleton
	skeleton.joints = new riv::Joint*[7];
	skeleton.nodeCount = 7;
	skeleton.joints[0] = new riv::Joint;
	*skeleton.joints[0] = torso;
	skeleton.joints[1] = new riv::Joint;
	*skeleton.joints[1] = shoulder_L;
	skeleton.joints[2] = new riv::Joint;
	*skeleton.joints[2] = shoulder_R;
	skeleton.joints[3] = new riv::Joint;
	*skeleton.joints[3] = elbow_L;
	skeleton.joints[4] = new riv::Joint;
	*skeleton.joints[4] = elbow_R;
	skeleton.joints[5] = new riv::Joint;
	*skeleton.joints[5] = wrist_L;
	skeleton.joints[6] = new riv::Joint;
	*skeleton.joints[6] = wrist_R;

	// set transforms
	torso.globalTransform = monkeyTransform.modelMatrix();
	shoulder_L.globalTransform = glm::translate(torso.globalTransform, glm::vec3(-3, 0, 0));
	shoulder_R.globalTransform = glm::translate(torso.globalTransform, glm::vec3(3, 0, 0));
	elbow_L.globalTransform = glm::translate(shoulder_L.globalTransform, glm::vec3(-3, 0, 0));
	elbow_R.globalTransform = glm::translate(shoulder_R.globalTransform, glm::vec3(3, 0, 0));
	wrist_L.globalTransform = glm::translate(elbow_L.globalTransform, glm::vec3(-3, 0, 0));
	wrist_R.globalTransform = glm::translate(elbow_R.globalTransform, glm::vec3(3, 0, 0));

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

		//riv::SolveFK(skeleton);
		for (int i = 0; i < skeleton.nodeCount; i++)
		{
			depthShader.setMat4("model", skeleton.joints[i]->globalTransform);
			monkeyModel.draw();
		}

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

		//riv::SolveFK(skeleton);
		for (int i = 0; i < skeleton.nodeCount; i++)
		{
			depthShader.setMat4("model", skeleton.joints[i]->globalTransform);
			monkeyModel.draw();
		}

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

	ImGui::Begin("Hierarchy");
	for (int i = 0; i < skeleton.nodeCount; i++)
	{
		if (skeleton.joints[i]->parentIndex = -1)
		{
			if (ImGui::CollapsingHeader("Torso"))
			{
				for (int j = 0; j < skeleton.joints[i]->numChildren; j++)
				{
					if (ImGui::CollapsingHeader("Shoulder" + i))
					{
						for (int k = 0; k < skeleton.joints[j]->numChildren; k++)
						{
							if (ImGui::CollapsingHeader("Elbow" + j))
							{
								for (int l = 0; l < skeleton.joints[k]->numChildren; l++)
								{
									if (ImGui::CollapsingHeader("Wrist" + k))
									{

									}
								}
							}
						}
					}
				}
			}
			break;
		}
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