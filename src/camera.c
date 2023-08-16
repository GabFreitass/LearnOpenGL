#include "camera.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

static void updateCameraVectors(Camera *this)
{
    // calculate the new Front vector
    vec3 front;
    front[0] = cos(glm_rad(this->Yaw)) * cos(glm_rad(this->Pitch));
    front[1] = sin(glm_rad(this->Pitch));
    front[2] = sin(glm_rad(this->Yaw)) * cos(glm_rad(this->Pitch));
    glm_vec3_normalize_to(front, this->Front);
    // also re-calculate the Right and Up vector
    glm_vec3_crossn(this->Front, this->WorldUp, this->Right); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    glm_vec3_crossn(this->Right, this->Front, this->Up);
}

static void GetViewMatrix(Camera *this, mat4 dest)
{
    vec3 sum;
    glm_vec3_add(this->Position, this->Front, sum);
    return glm_lookat(this->Position, sum, this->Up, dest);
}

static void ProcessKeyboard(Camera *this, Camera_Movement direction, float deltaTime)
{
    vec3 frontScale, rightScale, upScale = {0};
    float velocity = this->MovementSpeed * deltaTime;
    if (direction == FORWARD)
    {
        glm_vec3_scale(this->Front, velocity, frontScale);
        glm_vec3_add(this->Position, frontScale, this->Position);
    }
    if (direction == BACKWARD)
    {
        glm_vec3_scale(this->Front, velocity, frontScale);
        glm_vec3_sub(this->Position, frontScale, this->Position);
    }
    if (direction == LEFT)
    {
        glm_vec3_scale(this->Right, velocity, rightScale);
        glm_vec3_sub(this->Position, rightScale, this->Position);
    }
    if (direction == RIGHT)
    {
        glm_vec3_scale(this->Right, velocity, rightScale);
        glm_vec3_add(this->Position, rightScale, this->Position);
    }
    if (direction == UP)
    {
        glm_vec3_scale(this->Up, velocity, upScale);
        glm_vec3_add(this->Position, upScale, this->Position);
    }
    if (direction == DOWN)
    {
        glm_vec3_scale(this->Up, velocity, upScale);
        glm_vec3_sub(this->Position, upScale, this->Position);
    }
}

static void ProcessMouseMovement(Camera *this, float xoffset, float yoffset)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw += xoffset;
    this->Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (this->Pitch > 89.0f)
        this->Pitch = 89.0f;
    if (this->Pitch < -89.0f)
        this->Pitch = -89.0f;

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors(this);
}

static void ProcessMouseScroll(Camera *this, float yoffset)
{
    this->Zoom -= (float)yoffset;
    if (this->Zoom < 1.0f)
        this->Zoom = 1.0f;
    if (this->Zoom > 45.0f)
        this->Zoom = 45.0f;
}

void destructCamera(Camera *this)
{
    free((void *)this);
}

Camera initCamera(vec3 cameraPosition)
{
    Camera camera = {0};
    glm_vec3((vec3){0.0f, 0.0f, -1.0f}, camera.Front);
    camera.MovementSpeed = SPEED;
    camera.MouseSensitivity = SENSITIVITY;
    camera.Zoom = ZOOM;
    glm_vec3(cameraPosition, camera.Position);
    glm_vec3((vec3){0.0f, 1.0f, 0.0f}, camera.WorldUp);
    camera.Yaw = YAW;
    camera.Pitch = PITCH;
    camera.GetViewMatrix = GetViewMatrix;
    camera.ProcessKeyboard = ProcessKeyboard;
    camera.ProcessMouseMovement = ProcessMouseMovement;
    camera.ProcessMouseScroll = ProcessMouseScroll;
    camera.destructCamera = destructCamera;
    updateCameraVectors(&camera);

    return camera;
}