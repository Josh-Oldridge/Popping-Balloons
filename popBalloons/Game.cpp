#include "Game.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp> 

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

    // Check if any balloons are off-screen and remove them
    for (size_t i = 0; i < balloons.size();) {
        if (balloons[i].isOffScreen()) {
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
    score += static_cast<int>((0.15f - balloon.getSize()) * 100.0f);
    // Remove balloon from the vector
    balloons.erase(balloons.begin() + balloonIndex);
}

void Game::createBalloon() {
    // Randomize position, color, and size within certain bounds
    std::uniform_real_distribution<float> dis(-1.0, 1.0); // For x-coordinate
    std::uniform_real_distribution<float> disColor(0.0, 1.0); // For color
    std::uniform_real_distribution<float> disSize(0.05f, 0.15f); // For size

    glm::vec3 position(dis(gen), -1.0f, 0.0f); // Start from the bottom of the screen
    glm::vec3 color(disColor(gen), disColor(gen), disColor(gen)); // Random color for each balloon
    float size = disSize(gen); // Random size for each balloon

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
        Game* game = static_cast<Game*>(glfwGetWindowUserPointer(win));
        game->renderer.resize(width, height);
    });

    // Get the framebuffer size
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

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
    renderer.render(balloons);

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
    float ndcX = (xpos / fbWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (ypos / fbHeight) * 2.0f;
    
    for (size_t i = 0; i < balloons.size(); ++i) {
        Balloon& balloon = balloons[i];
        glm::vec3 position = balloon.getPosition();
        float horizontal_radius = balloon.getSize() * 0.5f; // Horizontal radius of the ellipse
        float vertical_radius = balloon.getSize(); // Vertical radius of the ellipse

        // Debug balloon position and size
        std::cout << "Balloon " << i << " position=(" << position.x << ", " << position.y
                  << "), size=" << balloon.getSize() << std::endl;
    
        // Adjusted collision detection for elliptical balloons
        float dx = (ndcX - position.x) / horizontal_radius;
        float dy = (ndcY - position.y) / vertical_radius;
        if (dx * dx + dy * dy <= 1.0f) {
            // Debug message to indicate a pop should occur
            std::cout << "Balloon " << i << " popped!" << std::endl;

            // Remove the balloon from the list (simple handling)
            balloons.erase(balloons.begin() + i);

            // Optionally, you can do more (e.g., play sound, animation, etc.)
            
            break; // If you want to only pop one balloon per click
        }
    }
}

void Game::endGame() {
    // Output final score or trigger game over screen/behavior
    std::cout << "Game Over! Your score: " << score << std::endl;
    // Clean up and prepare to exit, set a flag or directly indicate window to close
    glfwSetWindowShouldClose(window, GL_TRUE);
}