#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

// --- Globals ---
float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
int selectedBlockType = 1;

int worldData[24][10][24];


glm::vec3 cameraPos = glm::vec3(0.0f, 15.0f, 0.0f); 
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f, lastFrame = 0.0f;
float verticalVelocity = 0.0f;
const float gravity = -12.0f; 
bool isGrounded = false;

float crosshairVertices[] = { -0.02f, 0, 0, 0.02f, 0, 0, 0, -0.03f, 0, 0, 0.03f, 0 };

float getTerrainHeight(int x, int z) {
    return floor(sin(x * 0.2f) * 2.0f + cos(z * 0.2f) * 2.0f);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; 
    lastX = (float)xpos; lastY = (float)ypos;
    float sensitivity = 0.1f;
    yaw += xoffset * sensitivity; pitch += yoffset * sensitivity;
    if (pitch > 89.0f) pitch = 89.0f; if (pitch < -89.0f) pitch = -89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

const char* vShader = "#version 330 core\nlayout(location=0)in vec3 aPos;layout(location=1)in vec2 aTex;out vec2 TexCoord;uniform mat4 model;uniform mat4 view;uniform mat4 projection;void main(){gl_Position=projection*view*model*vec4(aPos,1.0);TexCoord=aTex;}\0";
const char* fShader = "#version 330 core\nout vec4 FragColor;in vec2 TexCoord;uniform sampler2D texture1;uniform sampler2D texture2;uniform bool isTop;void main(){if(isTop)FragColor=texture(texture2,TexCoord);else FragColor=texture(texture1,TexCoord);}\n\0";

void breakBlock() {
    float reach = 5.0f;
    float step = 0.1f;
    for (float t = 0; t < reach; t += step) {
        glm::vec3 checkPos = cameraPos + (cameraFront * t);
        int ix = (int)floor(checkPos.x + 0.5f) + 12;
        int iy = (int)floor(checkPos.y + 0.5f);
        int iz = (int)floor(checkPos.z + 0.5f) + 12;
        if (ix >= 0 && ix < 24 && iy >= 0 && iy < 10 && iz >= 0 && iz < 24) {
            if (worldData[ix][iy][iz] != 0) {
                worldData[ix][iy][iz] = 0; 
                return;
            }
        }
    }
}

void placeBlock() {
    float reach = 5.0f;
    float step = 0.1f;
    glm::vec3 lastEmptyPos = cameraPos;
    for (float t = 0; t < reach; t += step) {
        glm::vec3 checkPos = cameraPos + (cameraFront * t);
        int ix = (int)floor(checkPos.x + 0.5f) + 12;
        int iy = (int)floor(checkPos.y + 0.5f);
        int iz = (int)floor(checkPos.z + 0.5f) + 12;
        if (ix >= 0 && ix < 24 && iy >= 0 && iy < 10 && iz >= 0 && iz < 24) {
            if (worldData[ix][iy][iz] != 0) {
                int px = (int)floor(lastEmptyPos.x + 0.5f) + 12;
                int py = (int)floor(lastEmptyPos.y + 0.5f);
                int pz = (int)floor(lastEmptyPos.z + 0.5f) + 12;
                if (px >= 0 && px < 24 && py >= 0 && py < 10 && pz >= 0 && pz < 24) {
                    worldData[px][py][pz] = selectedBlockType;
                }
                return;
            }
        }
        lastEmptyPos = checkPos;
    }
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "MineC++", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    float vertices[] = {
        -0.5f,-0.5f,-0.5f,0,0, 0.5f,-0.5f,-0.5f,1,0, 0.5f,0.5f,-0.5f,1,1, 0.5f,0.5f,-0.5f,1,1, -0.5f,0.5f,-0.5f,0,1, -0.5f,-0.5f,-0.5f,0,0,
        -0.5f,-0.5f, 0.5f,0,0, 0.5f,-0.5f, 0.5f,1,0, 0.5f,0.5f, 0.5f,1,1, 0.5f,0.5f, 0.5f,1,1, -0.5f,0.5f, 0.5f,0,1, -0.5f,-0.5f, 0.5f,0,0,
        -0.5f, 0.5f, 0.5f,1,1, -0.5f, 0.5f,-0.5f,0,1, -0.5f,-0.5f,-0.5f,0,0, -0.5f,-0.5f,-0.5f,0,0, -0.5f,-0.5f, 0.5f,1,0, -0.5f, 0.5f, 0.5f,1,1,
         0.5f, 0.5f, 0.5f,1,1,  0.5f, 0.5f,-0.5f,0,1,  0.5f,-0.5f,-0.5f,0,0,  0.5f,-0.5f,-0.5f,0,0,  0.5f,-0.5f, 0.5f,1,0,  0.5f, 0.5f, 0.5f,1,1,
        -0.5f,-0.5f,-0.5f,0,.3f, 0.5f,-0.5f,-0.5f,1,.3f, 0.5f,-0.5f, 0.5f,1,0, 0.5f,-0.5f, 0.5f,1,0, -0.5f,-0.5f, 0.5f,0,0, -0.5f,-0.5f,-0.5f,0,.3f,
        -0.5f, 0.5f,-0.5f,0,1, 0.5f, 0.5f,-0.5f,1,1, 0.5f, 0.5f, 0.5f,1,0, 0.5f, 0.5f, 0.5f,1,0, -0.5f, 0.5f, 0.5f,0,0, -0.5f, 0.5f,-0.5f,0,1
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float))); glEnableVertexAttribArray(1);

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs,1,&vShader,NULL); glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs,1,&fShader,NULL); glCompileShader(fs);
    unsigned int sProg = glCreateProgram(); glAttachShader(sProg, vs); glAttachShader(sProg, fs); glLinkProgram(sProg);
    glDeleteShader(vs); glDeleteShader(fs);

    unsigned int tex1, tex2;
    glGenTextures(1, &tex1); glBindTexture(GL_TEXTURE_2D, tex1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int w, h, ch; stbi_set_flip_vertically_on_load(true);
    unsigned char* d1 = stbi_load("insert path/grass.png", &w, &h, &ch, 0);
    if(d1){ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d1); glGenerateMipmap(GL_TEXTURE_2D); }
    stbi_image_free(d1);
    glGenTextures(1, &tex2); glBindTexture(GL_TEXTURE_2D, tex2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char* d2 = stbi_load("insert path/top_grass.jpg", &w, &h, &ch, 0);
    if(d2){ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, d2); glGenerateMipmap(GL_TEXTURE_2D); }
    stbi_image_free(d2);

    unsigned int cVAO, cVBO;
    glGenVertexArrays(1, &cVAO); glGenBuffers(1, &cVBO);
    glBindVertexArray(cVAO); glBindBuffer(GL_ARRAY_BUFFER, cVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);

    for (int x = 0; x < 24; x++){
        for (int z = 0; z < 24; z++){
            float hTerrain = getTerrainHeight(x - 12, z - 12);
            for (int y = 0; y < 10; y++){
                if (y <= hTerrain + 5) worldData[x][y][z] = 1;
                else worldData[x][y][z] = 0;
            }
        }
    }

while (!glfwWindowShouldClose(window)) {
    // 1. Time
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // --- 2. WASD INPUT & WALL COLLISION ---
    float sprint = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 2.5f : 1.0f;
    float speed = 5.0f * deltaTime * sprint;

    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));
    
    glm::vec3 wishMove(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) wishMove += forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) wishMove -= forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) wishMove -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) wishMove += right;

    if (glm::length(wishMove) > 0.0f) {
        wishMove = glm::normalize(wishMove) * speed;
        float pRadius = 0.3f;
        float checkLevels[] = { -0.7f, 0.0f }; // Waist and Head

        // X Movement
        bool hitX = false;
        float nextX = cameraPos.x + wishMove.x;
        for (float lvl : checkLevels) {
            int nx = (int)floor(nextX + (wishMove.x > 0 ? pRadius : -pRadius) + 0.5f) + 12;
            int ny = (int)floor(cameraPos.y + lvl + 0.5f);
            int nz = (int)floor(cameraPos.z + 0.5f) + 12;
            if (nx >= 0 && nx < 24 && ny >= 0 && ny < 10 && nz >= 0 && nz < 24)
                if (worldData[nx][ny][nz] != 0) { hitX = true; break; }
        }
        if (!hitX) cameraPos.x = nextX;

        // Z Movement
        bool hitZ = false;
        float nextZ = cameraPos.z + wishMove.z;
        for (float lvl : checkLevels) {
            int nx = (int)floor(cameraPos.x + 0.5f) + 12;
            int ny = (int)floor(cameraPos.y + lvl + 0.5f);
            int nz = (int)floor(nextZ + (wishMove.z > 0 ? pRadius : -pRadius) + 0.5f) + 12;
            if (nx >= 0 && nx < 24 && ny >= 0 && ny < 10 && nz >= 0 && nz < 24)
                if (worldData[nx][ny][nz] != 0) { hitZ = true; break; }
        }
        if (!hitZ) cameraPos.z = nextZ;
    }

    // --- 3. JUMPING & VERTICAL PHYSICS ---
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded) {
        verticalVelocity = 6.5f; // Initial upward burst
        isGrounded = false;
    }

    // Apply Gravity
    verticalVelocity += gravity * deltaTime;
    cameraPos.y += verticalVelocity * deltaTime;

    // --- 4. GROUND COLLISION (Only if falling) ---
    isGrounded = false; 
    if (verticalVelocity <= 0.0f) { // Only check for ground if moving DOWN or stationary
        int ix = (int)floor(cameraPos.x + 0.5f) + 12;
        int iz = (int)floor(cameraPos.z + 0.5f) + 12;
        int iyGround = (int)floor(cameraPos.y - 1.55f + 0.5f); 

        if (ix >= 0 && ix < 24 && iyGround >= 0 && iyGround < 10 && iz >= 0 && iz < 24) {
            if (worldData[ix][iyGround][iz] != 0) {
                cameraPos.y = (float)iyGround + 1.51f;
                verticalVelocity = 0.0f;
                isGrounded = true;
            }
        }
    }

    // --- 5. BLOCK INTERACTION ---
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) selectedBlockType = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) selectedBlockType = 2;
    static bool lRel = true, rRel = true;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (lRel) { breakBlock(); lRel = false; }
    } else lRel = true;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (rRel) { placeBlock(); rRel = false; }
    } else rRel = true;

    // --- 6. RENDERING (Unchanged) ---
    glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(sProg);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(sProg, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(sProg, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, tex1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, tex2);
    glUniform1i(glGetUniformLocation(sProg, "texture1"), 0);
    glUniform1i(glGetUniformLocation(sProg, "texture2"), 1);

    unsigned int itl = glGetUniformLocation(sProg, "isTop");
    unsigned int ml = glGetUniformLocation(sProg, "model");

    glBindVertexArray(VAO);
    for (int x = 0; x < 24; x++) {
        for (int y = 0; y < 10; y++) {
            for (int z = 0; z < 24; z++) {
                if (worldData[x][y][z] != 0) {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x - 12, (float)y, z - 12));
                    glUniformMatrix4fv(ml, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(itl, false); glDrawArrays(GL_TRIANGLES, 0, 30);
                    glUniform1i(itl, true);  glDrawArrays(GL_TRIANGLES, 30, 6);
                }
            }
        }
    }

    // UI Crosshair
    glm::mat4 id = glm::mat4(1.0f);
    glUniformMatrix4fv(ml, 1, GL_FALSE, glm::value_ptr(id));
    glUniformMatrix4fv(glGetUniformLocation(sProg, "view"), 1, GL_FALSE, glm::value_ptr(id));
    glUniformMatrix4fv(glGetUniformLocation(sProg, "projection"), 1, GL_FALSE, glm::value_ptr(id));
    glUniform1i(itl, false); glBindVertexArray(cVAO); glDrawArrays(GL_LINES, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
    glfwTerminate(); return 0;
}