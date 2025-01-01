#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iostream>

// Obstacle class
class Obstacle
{
public:
    sf::RectangleShape shape;
    float speed;

    Obstacle(float x, float y, float width, float height, float speed)
        : speed(speed)
    {
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color::Red);
        shape.setPosition({ x, y });
    }

    void move()
    {
        shape.move({ speed, 0 });
    }

    bool isOffScreen(float windowWidth) const
    {
        return shape.getPosition().x > windowWidth;
    }
};

int main()
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode({ 960, 540 }), "Jumpy Jack");
    window.setFramerateLimit(60);

    // Loading font
    sf::Font font;
    if (!font.openFromFile("..\\..\\..\\..\\fonts\\framd.ttf"))             // path should be relative to executable
    {
        return -1;
    }
              
    // Scoring system
    int score = 0;
    sf::Text scoreText(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition({ window.getSize().x - 200.f, 10.f });

    // Score timer 
    float scoreTimer = 0.f; 
    const float scoreInterval = 1.f; // increment score every 1 second

    // Ground
    sf::RectangleShape ground(sf::Vector2f(window.getSize().x, 80.f));
    ground.setFillColor(sf::Color(100, 100, 100));
    ground.setPosition({ 0.f, window.getSize().y - ground.getSize().y });

    // Player
    sf::RectangleShape player(sf::Vector2f(50.f, 120.f));
    player.setFillColor(sf::Color(0, 0, 0));
    player.setPosition({ (player.getSize().x) * 3, window.getSize().y - ground.getSize().y - player.getSize().y });

    // Player velocity & gravity
    bool isJumping = false;
    const float gravity = 1.0f;
    const float jumpVelocity = -22.f;
    float playerVelocityY = 0.f;

    // Obstacles
    std::vector<Obstacle> obstacles;
    const float obstacleSpeed = -10.f;
    const float spawnInterval = 0.5f; // spawn every 0.5 seconds
    float spawnTimer = 0.f;

    // Possible obst. sizes
    std::vector<sf::Vector2f> obstacleSizes = {
        {40.f, 20.f},
        {50.f, 40.f},
        {30.f, 50.f},
        {10.f, 50.f},
        {20.f, 60.f},
        {60.f, 70.f},
        {30.f, 80.f},
    };

    std::srand(std::time(0));

    // Game state
    bool isGameRunning = true;

    // Run the program as long as the window is open
    while (window.isOpen())
    {
        // Check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "Close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // *** PLAYER MOVEMENTS ***
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // Spacebar: player jump
                if (keyPressed->scancode == sf::Keyboard::Scan::Space && !isJumping)
                {
                    playerVelocityY = jumpVelocity;
                    isJumping = true;
                }
            }
        }

        // Game is running without collision
        if (isGameRunning)
        {
            // Apply gravity and make player jump
            playerVelocityY += gravity;
            player.move({ 0, playerVelocityY });

            // Check if player landed back on ground
            if (player.getPosition().y >= window.getSize().y - ground.getSize().y - player.getSize().y)
            {
                player.setPosition({ player.getPosition().x, window.getSize().y - ground.getSize().y - player.getSize().y });
                playerVelocityY = 0.f;
                isJumping = false;
            }

            // Spawn obstacles
            spawnTimer += 1.f / 60; // 60 FPS
            if (spawnTimer >= spawnInterval)
            {
                spawnTimer = 0.f;
                // Choose random obstacle shape
                sf::Vector2f randomSize = obstacleSizes[std::rand() % obstacleSizes.size()];
                // spawning location of obstacle
                float obstacleX = window.getSize().x + randomSize.x;
                float minY = 150.f;
                float maxY = window.getSize().y - ground.getSize().y - randomSize.y;
                float obstacleY = minY + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX / (maxY - minY));
                obstacles.emplace_back(obstacleX, obstacleY, randomSize.x, randomSize.y, obstacleSpeed);
            }

            // Move and remove obstacles
            for (auto it = obstacles.begin(); it != obstacles.end(); )
            {
                it->move();
                if (it->isOffScreen(window.getSize().x))
                {
                    it = obstacles.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            // Collision detection using AABB
            for (const auto& obstacle : obstacles)
            {
                if (player.getGlobalBounds().findIntersection(obstacle.shape.getGlobalBounds()))
                {
                    // set flag to stop game state
                    isGameRunning = false;
                }
            }

            // Updating score
            scoreTimer += 1.f / 60;
            if (scoreTimer >= scoreInterval)
            {
                scoreTimer = 0.f; // reset the timer 
                score += 1; // increment the score 
            }
        }        

        // Updating score text
        std::stringstream ss;
        ss << "Score: " << score; 
        scoreText.setString(ss.str());

        // Clear the window with white color
        window.clear(sf::Color::White);

        // Draw entities to screen
        window.draw(ground);
        window.draw(player);
        window.draw(scoreText);
        for (const auto& obstacle : obstacles)
        {
            window.draw(obstacle.shape);
        }

        // Collision is detected and game needs to stop
        if (!isGameRunning)
        {
            // Display "Game Ended" text 
            sf::Text endText(font); 
            endText.setCharacterSize(48); 
            endText.setFillColor(sf::Color::Red); 
            endText.setString("Game Ended"); 
            // align the text message to screen centre
            endText.setPosition({ window.getSize().x / 2.f - endText.getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f - ground.getSize().y - endText.getGlobalBounds().size.y / 2.f});
            window.draw(endText);
        }

        // End the current frame
        window.display();
    }
}
