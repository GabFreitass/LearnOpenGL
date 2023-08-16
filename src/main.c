#include "configs.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "cube.h"
#include "vao.h"
#include "vbo.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// camera
Camera camera;
float lastX;
float lastY;
bool firstMouse;

// timing
float deltaTime;
float lastFrame;

// lighting
vec3 lightPos = {0.0f, 5.0f, 0.0f};
extern GLfloat cubeVertices[288];

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window;
    GLint VIEW_WIDTH, VIEW_HEIGHT;
    if (FULLSCREEN_MODE)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        VIEW_WIDTH = mode->width;
        VIEW_HEIGHT = mode->height;
        window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, GAME_NAME, monitor, NULL);
    }
    else
    {
        VIEW_WIDTH = WINDOW_WIDTH;
        VIEW_HEIGHT = WINDOW_HEIGHT;
        window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, GAME_NAME, NULL, NULL);
    }

    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 0, 0);
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
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    camera = initCamera((vec3){0.0f, 0.0f, 3.0f});
    lastX = VIEW_WIDTH / 2.0f;
    lastY = VIEW_HEIGHT / 2.0f;
    firstMouse = true;
    deltaTime = 0.0f;
    lastFrame = 0.0f;

    // build and compile our shader program
    // ------------------------------------
    Shader containerCubeShader = initShader("shaders/basicLighting.vs", "shaders/basicLighting.fs");
    Shader lightCubeShader = initShader("shaders/lightCube.vs", "shaders/lightCube.fs");

    stbi_set_flip_vertically_on_load(true);
    Image textureIMG = initImage("img/textures/grass_block.png");
    Texture texture = initTexture(&textureIMG);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    Cube containerCube = initCube(1, (vec3){0.0f, 0.0f, 0.0f}, &texture, &containerCubeShader);

    // first, configure the cube's VAO (and VBO)
    VAO cubeVAO = initVAO();
    VBO cubeVBO = initVBO();

    // store vertices information in GPU memory
    cubeVBO.buffer(&cubeVBO, cubeVertices, sizeof(cubeVertices), GL_FALSE);

    // position attribute
    cubeVAO.configVertex(&cubeVAO, 0, 3, GL_FLOAT, 8, 0);

    // normal attribute
    cubeVAO.configVertex(&cubeVAO, 1, 3, GL_FLOAT, 8, 3);

    // texcoord attribute
    cubeVAO.configVertex(&cubeVAO, 2, 2, GL_FLOAT, 8, 6);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = (float)glfwGetTime(); // get current time in seconds
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        printf("FPS: %.2f\n", 1 / (deltaTime));

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        containerCubeShader.use(&containerCubeShader);
        containerCubeShader.setVec3f(&containerCubeShader, "lightColor", 1.0f, 1.0f, 1.0f);
        containerCubeShader.setVec3fv(&containerCubeShader, "lightPos", lightPos);
        containerCubeShader.setVec3fv(&containerCubeShader, "viewPos", camera.Position);

        // view/projection transformations
        mat4 projection;
        glm_perspective(glm_rad(camera.Zoom), (float)VIEW_WIDTH / (float)VIEW_HEIGHT, 0.1f, 100.0f, projection);
        mat4 view;
        camera.GetViewMatrix(&camera, view);
        containerCubeShader.setMat4(&containerCubeShader, "projection", projection);
        containerCubeShader.setMat4(&containerCubeShader, "view", view);

        // world transformation
        mat4 model;

        for (float i = -50.0f; i <= 50.0f; i += 1.0f)
        {
            for (float j = -50.0f; j <= 50.0f; j += 1.0f)
            {
                glm_mat4_identity(model);
                glm_translate(model, (vec3){i, -1.0f, j});
                containerCubeShader.setMat4(&containerCubeShader, "model", model);
                // render the cube
                cubeVAO.bind(&cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // also draw the lamp object
        lightCubeShader.use(&lightCubeShader);
        lightCubeShader.setMat4(&lightCubeShader, "projection", projection);
        lightCubeShader.setMat4(&lightCubeShader, "view", view);
        glm_mat4_identity(model);
        glm_translate(model, lightPos);
        glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
        lightCubeShader.setMat4(&lightCubeShader, "model", model);
        // render the cube
        cubeVAO.bind(&cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightPos[1] = 10.0f * cosf(currentFrame * DAY_SPEED / 100);
        lightPos[2] = 10.0f * sinf(currentFrame * DAY_SPEED / 100);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    cubeVBO.destroy(&cubeVBO);
    cubeVAO.destroy(&cubeVAO);
    camera.destructCamera(&camera);
    containerCubeShader.destructShader(&containerCubeShader);
    lightCubeShader.destructShader(&lightCubeShader);
    texture.destructTexture(&texture);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(&camera, FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(&camera, BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(&camera, LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(&camera, RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(&camera, UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(&camera, DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;
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

    camera.ProcessMouseMovement(&camera, xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(&camera, (float)yoffset);
}