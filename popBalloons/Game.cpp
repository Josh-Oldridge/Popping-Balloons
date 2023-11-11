#include "Game.h"
#include "Fragment.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/random.hpp>

Game::Game()
    : score(0),
      lives(3),
      gen(std::random_device{}()),
      window(nullptr) // Initialize other variables as needed
{
    lastTime = glfwGetTime(); // Initialize lastTime to current time
    nextBalloonTime = lastTime + generateRandomTime(); // Set initial timer using lastTime
    //... Other initialization code as needed ...

    // Initialize the renderer here if needed
    // Initialize the GLFW and other related systems in the `run` method
}

Game::~Game() {
    cleanup();
}

void Game::run() {
    if (!initializeGLFW() || !initializeWindow() || !initializeGLEW()) {
        std::cerr << "Initialization failed." << std::endl;
        return;
    }

    setupScene();

    registerClickCallback();
    // Main game loop
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        update(deltaTime);
        renderScene();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
}

void Game::update(float deltaTime) {
    // Update each balloon using deltaTime
    for (auto& balloon : balloons) {
        balloon.update(deltaTime);
    }

    // Update the fragments
    for (auto it = fragments.begin(); it != fragments.end();) {
        it->position += it->velocity * deltaTime;  // Update position based on velocity
        it->velocity += glm::vec3(0.0f, -9.8f * deltaTime, 0.0f); // Apply gravity, tune gravity to your game

        // Fade out the fragment over time or shrink it
        it->color.a = glm::max(it->color.a - (deltaTime / it->lifetime), 0.0f);
        // Reduce the fragment's lifetime
        it->lifetime -= deltaTime;

        // Remove fragment if its lifetime is over (or if it's invisible)
        if (it->lifetime <= 0.0f) {
            it = fragments.erase(it);
        } else {
            ++it;
        }
    }

    // Check if any balloons are off-screen and remove them
    for (size_t i = 0; i < balloons.size();) {
        if (balloons[i].isOffScreen()) {
            // Consider adding score penalty or life penalty here, only if needed
            balloons.erase(balloons.begin() + i);
            --lives;
            // Game over logic
            if (lives <= 0) {
                endGame();
                return; // Stop the update loop because the game is over
            }
        } else {
            ++i;
        }
    }

    // Check if it's time to create a new balloon
    float currentTime = glfwGetTime();
    if (currentTime >= nextBalloonTime) {
        createBalloon(); // Create a new balloon at random position and color
        nextBalloonTime = currentTime + generateRandomTime(); // Set the time for creating the next balloon
    }
}

void Game::popBalloon(int balloonIndex) {
    if (balloonIndex < 0 || balloonIndex >= static_cast<int>(balloons.size())) {
        return; // Index out of range
    }

    Balloon& balloon = balloons[balloonIndex];
    // Calculate score based on size (smaller balloon gives higher score)
    score += 100;

    // Generate the fragments for the explosion effect
    int numFragments = 10; // This can be any number that you feel looks good
    for (int i = 0; i < numFragments; ++i) {
        glm::vec3 position = balloon.getPosition(); // Start the fragment at the balloon's position
        glm::vec3 velocity = glm::ballRand(1.0f);   // Randomize velocity direction
        glm::vec4 color = glm::vec4(balloon.getColor(), 1.0f); // Assuming Balloon::getColor() returns a glm::vec3 and specifying alpha
        float size = 1.0f;      // Size of the fragment (use the appropriate size for your fragment)
        float lifetime = 1.0f;  // Set how long the fragment should be alive

        Fragment frag(position, velocity, color, size, lifetime); // Using the Fragment constructor with parameters
        fragments.push_back(frag);
    }

    balloons.erase(balloons.begin() + balloonIndex);
}


void Game::createBalloon() {
    // Randomize position, color, and size within certain bounds
    std::uniform_real_distribution<float> dis(-1.0, 1.0); // For x-coordinate
    std::uniform_real_distribution<float> disColor(0.0, 1.0); // For color
    //std::uniform_real_distribution<float> disSize(0.05f, 0.15f); // For size

    glm::vec3 position(dis(gen), -1.0f, 0.0f); // Start from the bottom of the screen
    glm::vec3 color(disColor(gen), disColor(gen), disColor(gen)); // Random color for each balloon
    //float size = disSize(gen); // Random size for each balloon
    float size = 0.2f;

    balloons.push_back(Balloon(position, size, color));
}

void Game::cleanup() {
    renderer.cleanup(); // Let Renderer handle its own cleanup

    // Destroy the window if it was created
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    // Terminate GLFW
    glfwTerminate();
}

// Add other private member function implementations below

float Game::generateRandomTime() {
    std::uniform_real_distribution<float> disTime(2.0f, 5.0f); // Between 2 and 5 seconds
    return disTime(gen);
}

bool Game::initializeGLFW() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    return true;
}

bool Game::initializeWindow() {
    // Create a windowed mode window and its OpenGL context
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(mode->width, mode->height, "Pop Balloons", glfwGetPrimaryMonitor(), nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set this object to be the user pointer
    glfwSetWindowUserPointer(window, this);

    // Set the frame buffer resize callback
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height) {
        // If the window is minimized, GLFW may set width and height to zero.
        if (width == 0 || height == 0) {
            // This can happen if the window is minimized. Do not update sizes to zero.
            std::cout << "Resize was called with a zero width or height, ignoring." << std::endl;
            return;
        }

        Game* game = static_cast<Game*>(glfwGetWindowUserPointer(win));
        if (game) {
            // Update framebuffer size stored in the Game class
            game->fbWidth = width;
            game->fbHeight = height;

            // Call the Renderer class's resize method to update viewport and projection
            game->renderer.resize(width, height);

            std::cout << "Framebuffer size updated in game class: " << width << "x" << height << std::endl;
        }
    });
    // Get the framebuffer size
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    std::cout << "Framebuffer size after window creation: " << fbWidth << "x" << fbHeight << std::endl;

    return true;
}
bool Game::initializeGLEW() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cerr << "Failed to initialize GLEW\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    return true;
}

void Game::setupScene() {
    // Initialize renderer - make sure this is done post-GLEW initialization
    renderer.initialize();

    // Set the initial projection matrix
    float aspectRatio = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f);

    renderer.setProjectionMatrix(projection);
}
void Game::updateScene(float deltaTime) {
    // Update each balloon using deltaTime
    for (auto& balloon : balloons) {
        balloon.update(deltaTime);
    }

    // Check if any balloons are off-screen and remove them, decreasing lives accordingly
    balloons.erase(std::remove_if(balloons.begin(), balloons.end(),
        [this](const Balloon& balloon) {
            bool isOffScreen = balloon.isOffScreen();
            if (isOffScreen) {
                --lives;
                if (lives <= 0) {
                    endGame();
                }
            }
            return isOffScreen;
        }), balloons.end());

    // Check if it's time to create a new balloon
    double currentTime = glfwGetTime();
    if (currentTime >= nextBalloonTime) {
        createBalloon(); // Create a new balloon at random position and color
        nextBalloonTime = currentTime + generateRandomTime(); // Set the time for creating the next balloon
    }
}

void Game::renderScene() {
    // Clear the screen with a specific color (e.g., black)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Delegate the rendering of the balloons to the Renderer class
    renderer.render(balloons, fragments);

    // If there are other things to render (e.g., UI, background), do it here or delegate as well

    // Swap buffers if using double buffering, handled in Game::run method
}

void Game::registerClickCallback() {
    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);

            Game* game = static_cast<Game*>(glfwGetWindowUserPointer(win));
            if (game) {
                game->handleClick(static_cast<float>(xpos), static_cast<float>(ypos));
            }
        }
    });
}

void Game::handleClick(float xpos, float ypos) {
    std::cout << "Click received at (" << xpos << ", " << ypos << ")" << std::endl;
    
    // Convert from screen space to normalized device coordinates (NDC)
    float aspectRatio = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);
    float ndcX = (xpos / static_cast<float>(fbWidth)) * 2.0f - 1.0f;
    float ndcY = (ypos / static_cast<float>(fbHeight)) * -2.0f + 1.0f;

    std::cout << "Converted to NDC at (" << ndcX << ", " << ndcY << ")" << std::endl;
    
    for (size_t i = 0; i < balloons.size(); ++i) {
    Balloon& balloon = balloons[i];
    glm::vec3 position = balloon.getPosition();
    
    float radius = balloon.getSize();
    
    float dx = (ndcX - position.x) / aspectRatio; // <-- change this line
    float dy = (ndcY - position.y);
    float distanceSquared = dx * dx + dy * dy;
    /*
    float dx = (ndcX - position.x);
    float dy = (ndcY - position.y);
    float distanceSquared = dx * dx + dy * dy;*/

    

    std::cout << "Balloon " << i << " position=(" << position.x << ", " << position.y
                  << "), size=" << balloon.getSize() << ", radius=" << radius
                  << ", distanceSquared=" << distanceSquared << std::endl;

        if (distanceSquared <= (radius * radius) * 1.1f) { // the factor of 1.1 is arbitrary, you can adjust it as necessary
            std::cout << "Balloon " << i << " popped!" << std::endl;
            popBalloon(i);
            break;
        }
    }
}
void Game::endGame() {
    // Output final score or trigger game over screen/behavior
    std::cout << "Game Over! Your score: " << score << std::endl;
    // Clean up and prepare to exit, set a flag or directly indicate window to close
    glfwSetWindowShouldClose(window, GL_TRUE);
}