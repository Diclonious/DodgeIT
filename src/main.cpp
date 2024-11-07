#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <array>
#include <cstdlib>
#include "SFML/Audio.hpp"


std::chrono::steady_clock::time_point lastArrowDrawTime;
const std::chrono::milliseconds arrowDrawInterval(3000);
GLFWwindow* window;
float characterX = 0.0f;
float characterY = 0.0f;
void renderBackground(GLuint backgroundTexture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

struct Arrow {
    float x;
    float y;
    GLuint texture;
};

std::vector<Arrow> arrows;
GLuint characterTexture;
GLuint objectTexture;
GLuint arrowTexture;
GLuint arrowTexture1;
GLuint arrowTexture2;
GLuint arrowTexture3;
GLuint arrowTexture4;
GLuint arrowTexture5;
GLuint arrowTexture6;
GLuint arrowTexture7;

void loadTexture(const char* imagePath, GLuint& textureID) {
    int width, height, channels;
    unsigned char* image = stbi_load(imagePath, &width, &height, &channels, STBI_rgb_alpha);

    if (!image) {
        fprintf(stderr, "Failed to load image: %s\n", imagePath);
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
}

std::vector<bool> fireObjectReachedCenter;
void renderCharacter(float characterX, float characterY) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, characterTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(characterX - 0.1f, characterY - 0.1f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(characterX + 0.1f, characterY - 0.1f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(characterX + 0.1f, characterY + 0.1f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(characterX - 0.1f, characterY + 0.1f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void renderArrow(float arrowX, float arrowY, GLuint arrowTexture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, arrowTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    const float arrowSize = 0.05f;

    glTexCoord2f(0.0f, 1.0f); glVertex2f(arrowX - arrowSize, arrowY - arrowSize);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(arrowX + arrowSize, arrowY - arrowSize);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(arrowX + arrowSize, arrowY + arrowSize);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(arrowX - arrowSize, arrowY + arrowSize);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void renderObject(float objectX, float objectY) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, objectTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(objectX - 0.05f, objectY - 0.05f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(objectX + 0.05f, objectY - 0.05f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(objectX + 0.05f, objectY + 0.05f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(objectX - 0.05f, objectY + 0.05f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

bool checkCollision(float characterX, float characterY, float objectX, float objectY, float objectWidth, float objectHeight) {
    float characterWidth = 0.02f; // Adjust as needed
    float characterHeight = 0.02f; // Adjust as needed

    bool overlapX = characterX + characterWidth > objectX - objectWidth &&
                    characterX - characterWidth < objectX + objectWidth;

    bool overlapY = characterY + characterHeight > objectY - objectHeight &&
                    characterY - characterHeight < objectY + objectHeight;

    return overlapX && overlapY;
}

void renderObjects(const std::vector<std::pair<float, float>>& objectPositions) {
    for (const auto& objectPosition : objectPositions) {
        renderObject(objectPosition.first, objectPosition.second);
    }
}
bool shouldExitGame = false;

void keyCallbackExit(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        shouldExitGame = true; // Set the flag to exit the game
    }
}
const int numFireObjects = 8;  // Change this to the desired number of fire objects
std::array<std::pair<float, float>, numFireObjects> fireObjectPositions;

std::vector<std::pair<float, float>> generateRandomObjectPositions(int numObjects, float radius) {
    std::vector<std::pair<float, float>> objectPositions;

    float angleIncrement = 360.0f / numObjects;

    for (int i = 0; i < numObjects; ++i) {
        float angle = glm::radians(static_cast<float>(i) * angleIncrement);
        float randomX = characterX + radius * cos(angle);
        float randomY = characterY + radius * sin(angle);

        objectPositions.emplace_back(randomX, randomY);
        fireObjectPositions[i] = std::make_pair(randomX, randomY);
    }

    return objectPositions;
}



void renderArrowAtIndex(size_t index) {
    if (index < arrows.size()) {
        const auto& arrow = arrows[index];
        renderArrow(arrow.x, arrow.y, arrow.texture);
    }
}
bool First=true;
std::array<std::pair<float, float>, numFireObjects> firePrevPosition;
bool Collide=false;
int main() {
    if (!glfwInit()) {
        return -1;
    }
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    glfwSetErrorCallback(errorCallback);

    window = glfwCreateWindow(1000, 1000, "Character Movement Test", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    const char* characterImagePath = "C:\\Users\\anabe\\CLionProjects\\new\\src\\d9gef55-58a42c2d-55c8-422c-b484-0e7dbb6e8f33.png";
    const char* objectImagePath = "C:\\Users\\anabe\\CLionProjects\\new\\src\\fire.png";
    const char* UP = "C:\\Users\\anabe\\CLionProjects\\new\\src\\U.png";
    const char* RIGHT = "C:\\Users\\anabe\\CLionProjects\\new\\src\\R.png";
    const char* LEFT = "C:\\Users\\anabe\\CLionProjects\\new\\src\\L.png";
    const char* DOWN = "C:\\Users\\anabe\\CLionProjects\\new\\src\\D.png";
    const char* UL = "C:\\Users\\anabe\\CLionProjects\\new\\src\\UL.png";
    const char* DR = "C:\\Users\\anabe\\CLionProjects\\new\\src\\DR.png";
    const char* DL = "C:\\Users\\anabe\\CLionProjects\\new\\src\\DL.png";
    const char* UR = "C:\\Users\\anabe\\CLionProjects\\new\\src\\UR.png";


    loadTexture(characterImagePath, characterTexture);
    loadTexture(objectImagePath, objectTexture);
    loadTexture(RIGHT, arrowTexture);
    loadTexture(UR, arrowTexture1);
    loadTexture(UP, arrowTexture2);
    loadTexture(UL, arrowTexture3);
    loadTexture(LEFT, arrowTexture4);
    loadTexture(DL, arrowTexture5);
    loadTexture(DOWN, arrowTexture6);
    loadTexture(DR, arrowTexture7);

    float charspeed = 0.0015f;
    const char* backgroundImagePath = "C:\\Users\\anabe\\CLionProjects\\new\\res\\textures\\alfzmyle-battlemaps-480x2703-export-export.jpg";
    GLuint backgroundTexture;
    loadTexture(backgroundImagePath, backgroundTexture);

    std::vector<std::pair<float, float>> objectPositions = generateRandomObjectPositions(8, 0.7f);

    // Remove the initializeArrows function

    // Arrow initialization directly in the main function
    arrows.push_back({ 0.85f, 0.0f, arrowTexture4 });     // Adjusted the initial position
    arrows.push_back({ 0.65f, 0.65f, arrowTexture5 });
    arrows.push_back({ 0.0f, 0.85f, arrowTexture6});
    arrows.push_back({ -0.65f, 0.65f, arrowTexture7 });
    arrows.push_back({ -0.85f, 0.0f, arrowTexture });     // Adjusted the initial position
    arrows.push_back({ -0.65f, -0.65f, arrowTexture1 });
    arrows.push_back({ 0.0f, -0.85f, arrowTexture2 });
    arrows.push_back({ 0.65f, -0.65f, arrowTexture3 });

    size_t currentArrowIndex = -1;
    for (size_t i = 0; i < firePrevPosition.size(); ++i) {
        firePrevPosition[i] = fireObjectPositions[i];
    }

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        renderBackground(backgroundTexture);


        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedDuration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastArrowDrawTime);
        for (size_t i = 0; i < firePrevPosition.size(); ++i) {
            renderObject(firePrevPosition[i].first, firePrevPosition[i].second);
        }
        // Update position of the fire element towards the center if collision detected
        for (size_t i = 0; i < fireObjectPositions.size(); ++i) {
            if(i==fireObjectPositions.size()-1)
            {
                charspeed+=0.0005;
            }

            auto& firePosition = fireObjectPositions[i];

            renderObject(firePosition.first, firePosition.second);


            float distanceFromOrigin = sqrt(firePosition.first * firePosition.first + firePosition.second * firePosition.second);
            if (distanceFromOrigin > 3.0f) {
                // Reset the fire object's position
                float angle = glm::radians(static_cast<float>(currentArrowIndex) * (360.0f / numFireObjects));
                float randomX = characterX + 0.7f * cos(angle); // Adjust the radius as needed
                float randomY = characterY + 0.7f * sin(angle); // Adjust the radius as needed
                firePosition = std::make_pair(randomX, randomY);
            }
            if (i == currentArrowIndex) {
                // Define direction vector based on the type of fire object
                float dx = 0.0f, dy = 0.0f;
                switch (currentArrowIndex) {
                    case 2:  // Upper fire object, move downwards
                        dy = -1.0f;
                        break;
                    case 1:  // Upper-right fire object, move downwards and to the left
                        dx = -1.0f;
                        dy = -1.0f;
                        break;
                    case 0:  // Right fire object, move to the left
                        dx = -1.0f;
                        break;
                    case 7:  // Lower-right fire object, move upwards and to the left
                        dx = -1.0f;
                        dy = 1.0f;
                        break;
                    case 6:  // Lower fire object, move upwards
                        dy = 1.0f;
                        break;
                    case 5:  // Lower-left fire object, move upwards and to the right
                        dx = 1.0f;
                        dy = 1.0f;
                        break;
                    case 4:  // Left fire object, move to the right
                        dx = 1.0f;
                        break;
                    case 3:  // Upper-left fire object, move downwards and to the right
                        dx = 1.0f;
                        dy = -1.0f;
                        break;
                        // Cases for each fire object direction...
                }

                if (checkCollision(characterX, characterY, firePosition.first, firePosition.second, 0.09, 0.09)) {
                    fprintf(stdout, "Collision detected with object at (%f, %f)!\n", firePosition.first, firePosition.second);
                    Collide= true;
                }

                // Normalize the direction vector if needed
                float length = sqrt(dx * dx + dy * dy);
                if (length != 0) {
                    dx /= length;
                    dy /= length;
                }

                // Move the fire in the direction of the vector
                float speed = 0.015f; // Adjust speed as needed
                firePosition.first += dx * speed;
                firePosition.second += dy * speed;


                // Check if the fire object has passed 2.0f distance from the origin

            }
        }

        // Draw the current arrow
        if (elapsedDuration >= arrowDrawInterval) {
            // Draw the current arrow
            currentArrowIndex = std::rand() % arrows.size();

            // Update the last draw time
            lastArrowDrawTime = currentTime;
        }

        // Render the object


        // Render the arrow
        if (currentArrowIndex != -1) {
            renderArrowAtIndex(currentArrowIndex);
        }


        // Handle character movement
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            characterX -= charspeed;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            characterX += charspeed;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            characterY += charspeed;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            characterY -= charspeed;
        }
        if(Collide)
        {
            break;


        }
        renderCharacter(characterX, characterY);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }





    glDeleteTextures(1, &characterTexture);
    glDeleteTextures(1, &objectTexture);

    glfwTerminate();
    return 0;
}