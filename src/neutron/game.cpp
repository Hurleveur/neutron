#include "import.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <iostream>
#include <map>
#include <span>
#include <istream>
#include <fstream>

// utility functions
void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
GLFWwindow* init();
void Step(double time);

// TODO: put all of this inside a GameState class
// window settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

constexpr int EARTH_MOON_MASS = 100;
constexpr int SUN_MASS = 100000000;
// camera
Camera camera(glm::vec3(50.0f, 0.0f,50.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// stops time
bool stop = false;
float stopTimeout = 0.0f;

class Window {
    GLFWwindow* glfw_window;

public:
    Window() {
        // setup all GLFW and enable opengl
        glfwInit();
        // use version 3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        // setup profile
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        glfw_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "neutron", nullptr, nullptr);
        if (!glfw_window)
            throw std::runtime_error("Failed to create GLFW window.");

        glfwMakeContextCurrent(glfw_window);
        glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
        glfwSetCursorPosCallback(glfw_window, mouse_callback);
        glfwSetScrollCallback(glfw_window, scroll_callback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize GLAD.");

        // configure global opengl state
        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_CULL_FACE);
//        glFrontFace(GL_CCW);
    }

    ~Window() {
        glfwTerminate();
    }

    bool ShouldClose() const {
        return glfwWindowShouldClose(glfw_window);
    }

    void PaintAndPoll() const {
        glfwSwapBuffers(glfw_window);
        glfwPollEvents();
    }

    // TODO: this breaks encapsulation, get rid of this
    GLFWwindow* GetGLFWWindow() const {
        return glfw_window;
    }
};

// the objects and if they should be rendered
std::map<Planet*, bool> objectList;

struct VertexFormat {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;
};

class Model {
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    uint32_t vertex_index_count;

public:
    Model(const std::span<const VertexFormat>& vertices, const std::span<const uint16_t>& vertex_indices):
        VAO(0), VBO(0), EBO(0), vertex_index_count(vertex_indices.size()) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // fill vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (uint32_t)vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);

        // fill index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (uint32_t)vertex_indices.size_bytes(), vertex_indices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, tangent));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, texcoord));

        glBindVertexArray(0);
    }

    ~Model() {
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void Set() {
        glBindVertexArray(VAO);
    }

    void Draw() {
        glDrawElements(GL_TRIANGLES, vertex_index_count, GL_UNSIGNED_SHORT, (void*)0);
    }

    void Unset() {
        glBindVertexArray(0);
    }
};

// factory function that creates a Model given a file path to a MDL file
Model CreateModelFromFile(const std::string_view& file_path) {
    std::ifstream mdl_file(file_path.data());
    if (!mdl_file.is_open())
        throw std::runtime_error("Failed to open an MDL file for reading.");

    // read vertices
    uint32_t vertex_count;
    mdl_file.read((char*)&vertex_count, sizeof(uint32_t));
    std::vector<VertexFormat> vertices;
    vertices.resize(vertex_count);
    mdl_file.read((char*)vertices.data(), vertex_count * sizeof(VertexFormat));

    // read corner indices
    uint32_t vertex_index_count;
    mdl_file.read((char*)&vertex_index_count, sizeof(uint32_t));
    std::vector<uint16_t> vertex_indices;
    vertex_indices.resize(vertex_index_count);
    mdl_file.read((char*)vertex_indices.data(), vertex_index_count * sizeof(uint16_t));

    return {vertices, vertex_indices};
}

int main()
{
    Window window;
    
    std::cout << "Welcome to the solar system simulator, featuring particles within the Sun, a physics simulator including gravity and collisions with devastating effects (that is doing the simili orbits), realistic planets with Phong shaders and even normal mapping (each planet featuring their own image, normal and specular maps)." << std::endl;
    std::cout << "All of this would not be complete, however, without the skybox to surround all of it, and the ability to stop time." << std::endl;
    std::cout << "In order to move use Z (up), S(down), Q(left) and D(right) - camera speed can be changed in the code at line 59 of game.cpp - in order to stop/resume time, press T." << std::endl;
    
    // to move faster
    camera.MovementSpeed *= 6;

    // make shaders and objects, with VBO and VAO
    // skybox
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");
    makeSkybox(skyboxShader);
    // particles
    Shader particleShader("shaders/particle.vs", "shaders/particle.fs");
    makeParticles(particleShader);

    // shader for all planets
    Shader shader("shaders/shader.vs", "shaders/shader.fs");

    // crate the sphere model once, and reuse it for every planet
    Model planet_model = CreateModelFromFile("models/sphere.mdl");

    // make all planets, starting with the sun
    Planet sun(SUN_MASS, 5, 0, 0, 0, 0, 0, 0, shader, Planet::Type::Sun);
    objectList[&sun] = true;
    Planet mercury(90, .5, 1.5, -30, 0, 0.0004, 0.00015, 0, shader, Planet::Type::Mercury);
    objectList[&mercury] = true;
    Planet earth(100, 1, 50, 0, 0, 0.0001, 0.0003, 0, shader, Planet::Type::Earth);
    objectList[&earth] = true;
    Planet moon(1, .2, 51.5, -1.5, 0, 0.0001, 0.0003 + 0.00008, 0, shader, Planet::Type::Moon);
    objectList[&moon] = true;
    Planet mars(60, .8, 1.5, -80, 0, -0.0005, 0.00004, 0, shader, Planet::Type::Mars);
    objectList[&mars] = true;

    //glfwSwapInterval(0); // to remove the 60 fps limit
    float currentFrame;
    while (!window.ShouldClose())
    {
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // to prevent double stopping time in two consecutive frames (which results in not doing anything)
        if(stopTimeout > 0.f)
            stopTimeout -= deltaTime;

        processInput(window.GetGLFWWindow());

        // steps gravity and movement with physics, and do collision detection + handling
        Step(deltaTime);

        // we don't need to clear GL_COLOR_BUFFER_BIT due to the skybox being in the background
        glClear(GL_DEPTH_BUFFER_BIT);

        // Shader properties for all planets and the sun
        shader.use();
        shader.setVec3("light.position", 0.f, 0.f, 0.f);
        shader.setVec3("viewPos", camera.Position);
        // full on sunlight - will be reduced after the sun is drawn
        shader.setVec3("light.ambient", 1.f, 1.f, 1.f);
        shader.setVec3("light.diffuse", .7f, .7f, .7f);
        shader.setVec3("light.specular", .5f, .5f, .5f);

        // view and projection matrix, with a depth sufficient so it doesn't crop objects - both are also multiplied here in advanced for optimisation
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 viewProj = projection * view;
        shader.setMat4("view_projection_matrix", viewProj);

        // render all objects (model is calculated in the draw function itself)
        planet_model.Set(); // TODO: encapsulate inside a PlanetRenderer class
        for (auto object : objectList)
        {
            if(object.second)
                object.first->SetShaderVariables(shader, stop ? 0. : deltaTime);
            planet_model.Draw();

            // only the sun needs to be super bright, and its drawn first
            shader.setVec3("light.ambient", .2f, .2f, .2f);
        }
        planet_model.Unset(); // TODO: encapsulate inside a PlanetRenderer class

        glDisable(GL_CULL_FACE);
        particleShader.use();
        particleShader.setMat4("view_projection_matrix", viewProj);
        drawParticles(particleShader, stop ? 0 : deltaTime);
        glEnable(GL_CULL_FACE);

        // draw skybox as last (optimisation, all depth buffer have been +- filled now)
        drawSkybox(skyboxShader, view, projection);

        window.PaintAndPoll();
    }

    return 0;
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

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && stopTimeout <= 0.f)
    {
        stopTimeout = .2f;
        stop = !stop;
    }
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


void Step(double time)
{
    if (stop)
        return;

    // gravitational constant
    static const double gravitational = 6.674 / 100000000000;
    // apply gravity
    // auto is Planet, bool, where the bool states if the objects are active -
    // they get turned off when they have collided with the sun, in which case they are to be ignored
    for (auto object : objectList)
    {
        // all objects get pull from other objects - except themselves and disabled objects
        for (auto otherObject : objectList)
        {
            // the sun and other bigger objects wont be affected by the earth and the earth wont be by the moon or by other same weight planets - for simplicity, and optimisation
            if (!object.second || !otherObject.second || otherObject.first->mass <= object.first->mass)
                continue;
            double distance = object.first->DistanceFrom(*otherObject.first);
            // hack: the earth should have a pull much stronger on the moon, because its supposed to be much closer (but we wouldn't see anything if it was real scale)
            if (object.first->mass * otherObject.first->mass == EARTH_MOON_MASS)
                distance /= 100;

            double pull = otherObject.first->mass * gravitational / (distance * distance) * time;
            object.first->vX += pull * ((otherObject.first->x > object.first->x) ? 1 : -1);
            object.first->vY += pull * ((otherObject.first->y > object.first->y) ? 1 : -1);
            object.first->vZ += pull * ((otherObject.first->z > object.first->z) ? 1 : -1);
        }
    }

    // check for collisions and handle them
    for (auto object : objectList)
    {
        if (!object.second)
            continue;
        for (auto otherObject : objectList)
            if (!object.second || otherObject == object)
                continue;
            else if (object.first->DistanceFrom(*otherObject.first) < (object.first->radius + otherObject.first->radius))
            {
                // a collision with the sun is fatal
                if (object.first->mass == SUN_MASS)
                    objectList[otherObject.first] = false;
                else if (otherObject.first->mass == SUN_MASS)
                    objectList[object.first] = false;
                // otherwise both objects move away
                else
                {
                    // determine the strength of the collision
                    double factor = otherObject.first->mass * object.first->mass * std::sqrt(
                        (object.first->vX - otherObject.first->vX) * (object.first->vX - otherObject.first->vX) +
                        (object.first->vY - otherObject.first->vY) * (object.first->vY - otherObject.first->vY) +
                        (object.first->vZ - otherObject.first->vZ) * (object.first->vZ - otherObject.first->vZ)
                    );
                    // make them move away from each other with the strength of the impact
                    otherObject.first->vX += factor / otherObject.first->mass * (otherObject.first->vX > 0 ? -1 : 1);
                    otherObject.first->vY += factor / otherObject.first->mass * (otherObject.first->vY > 0 ? -1 : 1);
                    otherObject.first->vZ += factor / otherObject.first->mass * (otherObject.first->vZ > 0 ? -1 : 1);
                    object.first->vX += factor / object.first->mass * (object.first->vX > 0 ? -1 : 1);
                    object.first->vY += factor / object.first->mass * (object.first->vY > 0 ? -1 : 1);
                    object.first->vZ += factor / object.first->mass * (object.first->vZ > 0 ? -1 : 1);
                    // move them away enough so they're no longer in collision
                    object.first->Tick(time);
                    otherObject.first->Tick(time);
                }
            }
    }

    // move all objects
    for (auto object : objectList)
        object.first->Tick(time);
}
