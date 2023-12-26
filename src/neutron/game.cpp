#include "globals.h"
#include "init_helper.h"
#include "skybox/skybox.h"
#include <learnopengl/shader.h>

int main() {
    GLFWwindow *window = init();
    if (!window)
        return -1;

    Shader skyboxShader("skybox.vs", "skybox.fs");
    makeSkybox(skyboxShader);

    float currentFrame;
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        

        // skybox
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        drawSkybox(skyboxShader, view, projection);


        // clear for next frame
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
