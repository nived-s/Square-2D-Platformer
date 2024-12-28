#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Obstacle class
class Obstacle
{
public:
    sf::RectangleShape shape;
    float speed;

    Obstacle(float x, float y, float size, float speed)
        : speed(speed)
    {
        shape.setSize(sf::Vector2f(size, size));
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
    window.setFramerateLimit(144);

    // Ground
    sf::RectangleShape ground(sf::Vector2f(window.getSize().x, 80.f));
    ground.setFillColor(sf::Color(100, 100, 100));
    ground.setPosition({ 0.f, window.getSize().y - ground.getSize().y });

    // Player
    sf::RectangleShape player(sf::Vector2f(80.f, 80.f));
    player.setFillColor(sf::Color(0, 0, 0));
    player.setPosition({ (player.getSize().x) * 3, window.getSize().y - ground.getSize().y - player.getSize().y });

    // Player velocity & gravity
    bool isJumping = false;
    const float gravity = 1.0f;
    const float jumpVelocity = -15.f;
    float playerVelocityY = 0.f;

    // Obstacles
    std::vector<Obstacle> obstacles;
    const float obstacleSize = 40.f;
    const float obstacleSpeed = -10.f;
    const float spawnInterval = 1.f; // spawn every 2 seconds
    float spawnTimer = 0.f;

    std::srand(std::time(0));

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
        spawnTimer += 1.f / 144; // assuming 144 FPS
        if (spawnTimer >= spawnInterval)
        {
            spawnTimer = 0.f;
            //float obstacleY = window.getSize().y - ground.getSize().y - obstacleSize;
            float obstacleX = window.getSize().x - obstacleSize;
            float obstacleY = 200.f;
            obstacles.emplace_back(obstacleX, obstacleY, obstacleSize, obstacleSpeed);
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

        // Clear the window with white color
        window.clear(sf::Color::White);

        // Draw entities to screen
        window.draw(ground);
        window.draw(player);
        for (const auto& obstacle : obstacles)
        {
            window.draw(obstacle.shape);
        }

        // End the current frame
        window.display();
    }
}
