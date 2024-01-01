#include "import.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>
#include <vector>
#include <map>


void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
GLFWwindow* init();
Planet *Step(double time);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, -60.0f, 80.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool stop = false;

std::vector<Planet*> objectList;

int main()
{
    GLFWwindow* window = init();
    if (!window)
        return -1;
    camera.MovementSpeed *= 6;

    // build and compile shaders
    // -------------------------
    Shader skyboxShader("skybox.vs", "skybox.fs");
    makeSkybox(skyboxShader);

    Shader shader("sun.vs", "sun.fs");
    Planet sun(100000000, 5, 0, 0, 0, 0, 0, 0, shader, Sun);
    objectList.emplace_back(&sun);
    Planet earth(100, 1, 0, -50, 0, 0.0004, 0, 0, shader, Earth);
    objectList.emplace_back(&earth);
    Planet moon(1, .2, 1.5, -51.5, 0, 0.0004 + 0.00008, 0.00004, 0, shader, Moon);
    objectList.emplace_back(&moon);
    Planet mars(100, .8, 1.5, -80, 0, 0.0004 + 0.00008, 0.00004, 0, shader, Mars);
    objectList.emplace_back(&mars);
    Planet mercury(100, .5, 1.5, -30, 0, 0.0004 + 0.00008, 0.00004, 0, shader, Mercury);
    objectList.emplace_back(&mercury);
    // render loop
    // -----------
    float currentFrame;
    Planet *planetToDestroy = nullptr;
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        processInput(window);

        planetToDestroy = Step(deltaTime);
        /*
        if (planetToDestroy)
            // TODO: make it go boom
            objectList[planetToDestroy] = nullptr;
        */

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.use();
        shader.setVec3("light.position", 0.f, 0.f, 0.f);
        shader.setVec3("viewPos", camera.Position);
        // light properties
        shader.setVec3("light.ambient", 1.f, 1.f, 1.f);
        shader.setVec3("light.diffuse", .7f, .7f, .7f);
        shader.setVec3("light.specular", .5f, .5f, .5f);

        // material properties
        shader.setFloat("material.shininess", 2.f);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        for (auto object : objectList)
        {
            if(object)
                object->draw(shader);
            // only the sun needs to be super bright
            shader.setVec3("light.ambient", .2f, .2f, .2f);
        }

        drawSkybox(skyboxShader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



GLFWwindow* init() {
    glfwInit();
    // use version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // setup profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "neutron", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    return window;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        stop = !stop;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


Planet *Step(double time)
{
    SpaceObject* sun = SpaceObject::biggestMass;
    if (!sun || stop)
        return nullptr;
    static const double gravitational = 6.674 / 100000000000;
    for (auto object : objectList)
    {
        for (auto otherObject : objectList)
        {
            // the sun wont be affected by the earth and the earth wont be by the moon - for simplicity
            if (otherObject->mass <= object->mass)
                continue;
            double distance = object->DistanceFrom(*otherObject);
            // the earth should have a pull much stronger on the moon, because its supposed to be much closer
            if (object->mass * otherObject->mass == 100)
                distance /= 100;
            double pull = otherObject->mass * gravitational / (distance * distance) * time;
            object->vX += pull * ((otherObject->x > object->x) ? 1 : -1);
            object->vY += pull * ((otherObject->y > object->y) ? 1 : -1);
            object->vZ += pull * ((otherObject->z > object->z) ? 1 : -1);
        }
    }
    for (auto object : objectList)
        object->Tick(time);
    // we assume at most one object destruction per tick
    Planet* objectToRemove = nullptr;
    for (auto object : objectList)
    {
        for (auto otherObject : objectList)
            if (otherObject != object && object->DistanceFrom(*otherObject) < (object->radius + otherObject->radius))
            {
                objectToRemove = object->mass > otherObject->mass ? otherObject : object;
                return objectToRemove;
            }
    }
    return objectToRemove;
}
