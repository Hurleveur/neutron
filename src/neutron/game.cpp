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


void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
GLFWwindow* init();
SpaceObject *Step(double time);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, -10.0f, 50.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

vector<Planet*> objectList;

int main()
{
    GLFWwindow* window = init();
    if (!window)
        return -1;

    // build and compile shaders
    // -------------------------
    Shader skyboxShader("skybox.vs", "skybox.fs");
    makeSkybox(skyboxShader);

    Shader sunShader("planet.vs", "planet.fs");
    Planet sun(100000000, 5, 0, 0, 0, 0, 0, 0, sunShader, "resources/textures/th.jpeg");
    objectList.emplace_back(&sun);

    Shader planetShader("planet.vs", "planet.fs");
    Planet earth(100, 1, 0, -30, 0, 0.0004, 0, 0, planetShader, "resources/textures/planet.jpg");
    objectList.emplace_back(&earth);

    Shader moonShader("planet.vs", "planet.fs");
    Planet moon(1, .2, -1.3, -30, 0, 0.0003, 0.00004, 0, moonShader, "resources/textures/moon.bmp");
    objectList.emplace_back(&moon);

    // render loop
    // -----------
    float currentFrame;
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // doesnt delete on collision detect yet
        Step(deltaTime);

        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        sun.draw(sunShader, view, projection);

        earth.draw(planetShader, view, projection);

        moon.draw(moonShader, view, projection);

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


SpaceObject *Step(double time)
{
    SpaceObject* sun = SpaceObject::biggestMass;
    if (!sun)
        return nullptr;
    static const double gravitational = sun->mass * 6.674 / 1000000000000;
    for (SpaceObject* object : objectList)
    {
        for (SpaceObject* otherObject : objectList)
        {
            // the sun wont be affected by the earth and the earth wont be by the moon
            if (otherObject->mass <= object->mass)
                continue;
            // only consider the sun (or biggest mass) as it is the object everything gravitates around anyway
            double pull = gravitational * object->mass / object->DistanceFrom(*otherObject) * time;
            object->vX += pull * ((sun->x < object->x) ? -0.01 : 0.01);
            object->vY += pull * ((sun->y < object->y) ? -0.01 : 0.01);
            object->vZ += pull * ((sun->z < object->z) ? -0.01 : 0.01);
        }
    }
    for (SpaceObject* object : objectList)
        object->Tick(time);
    // we assume at most one object destruction per tick
    SpaceObject* objectToRemove = nullptr;
    for (SpaceObject* object : objectList)
    {
        for (SpaceObject* otherObject : objectList)
            if (otherObject != object && object->DistanceFrom(*otherObject) < (object->radius + otherObject->radius))
            {
                objectToRemove = object->mass > otherObject->mass ? otherObject : object;
                return objectToRemove;
            }
    }
    return objectToRemove;
}
