#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode({ 960 ,540 }), "Jumpy Jack");
    window.setFramerateLimit(144);

    // Ground
    sf::RectangleShape ground(sf::Vector2f(window.getSize().x, 80.f));
    ground.setFillColor(sf::Color(100, 100, 100));
    ground.setPosition({ 0.f, window.getSize().y - ground.getSize().y });

    // Player
    sf::RectangleShape player(sf::Vector2f(80.f, 80.f));
    player.setFillColor(sf::Color(0, 0, 0));
    player.setPosition({ (player.getSize().x) * 3 , window.getSize().y - ground.getSize().y - player.getSize().y });

    // Player velocity & gravity
    bool isJumping = false;
    const float gravity = 5.f;
    const float jumpVelocity = -45.f;
    float playerVelocityY = 0.f;

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // *** PLAYER MOVEMENTS ***
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // spacebar: player jump
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

        // clear the window with black color
        window.clear(sf::Color::White);

        // Draw entities to screen
        window.draw(ground);
        window.draw(player);

        // end the current frame
        window.display();
    }
}