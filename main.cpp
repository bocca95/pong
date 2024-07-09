#include "pch.h"

// FUNCTION TO NORMALIZE A VECTOR
sf::Vector2f normalize(const sf::Vector2f& vector) {
    float magnitude = std::sqrt(pow(vector.x, 2) + pow(vector.y, 2));
    if (magnitude != 0) {
        return sf::Vector2f(vector.x / magnitude, vector.y / magnitude);
    }
    else {
        return sf::Vector2f(0.f, 0.f);
    }
}

// FUNCTION TO CALCULATE THE NEW VELOCITY BASED ON THE CONTACT POINT
sf::Vector2f newVelocity(const sf::Vector2f& ballPos, const sf::Vector2f& paddlePos, float paddleHeight, float velocity) {
    float relativeY = ballPos.y - (paddlePos.y + paddleHeight / 2.0f);
    float normalizedRelativeY = relativeY / (paddleHeight / 2.0f);
    float reboundAngle = normalizedRelativeY * (M_PI / 4);
    sf::Vector2f newDirection(-std::cos(reboundAngle), std::sin(reboundAngle));
    return normalize(newDirection) * velocity;
}

int main() {
    // WINDOW SIZE AND DEFINITION
    const int wWidth = 1024;
    const int wHeight = 720;

    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight, 32), "Pong");
    window.setFramerateLimit(90);

    // SCORE INIT.
    int score = 0;
    int score2 = 0;

    sf::Font font;
    if (!font.loadFromFile("./Minecraft.ttf")) {
        std::cout << "Error loading font!";
        exit(-1);
    }

    // BALL, PADDLES AND TEXT INIT.
    sf::CircleShape ball(10.f);
    ball.setFillColor(sf::Color::White);
    float initialVelocity = 4.f;
    std::srand(static_cast<unsigned>(std::time(0)));
    float directionX = (std::rand() % 2 == 0) ? initialVelocity : -initialVelocity;
    float directionY = (std::rand() % 2 == 0) ? initialVelocity : -initialVelocity;
    sf::Vector2f ballVelocity(directionX, directionY);
    ball.setPosition(wWidth / 2, wHeight / 2);
    float velocityMagnitude = initialVelocity;

    sf::RectangleShape Lpaddle(sf::Vector2f(20.f, 100.f));
    Lpaddle.setFillColor(sf::Color::White);
    Lpaddle.setPosition(30.f, wHeight / 2 - Lpaddle.getSize().y / 2);

    sf::RectangleShape Rpaddle(sf::Vector2f(20.f, 100.f));
    Rpaddle.setFillColor(sf::Color::White);
    Rpaddle.setPosition(wWidth - 50.f, wHeight / 2 - Rpaddle.getSize().y / 2);

    sf::Text sb1;
    sb1.setFont(font);
    sb1.setPosition(wWidth / 4, 30);
    sb1.setCharacterSize(60);
    sb1.setFillColor(sf::Color::White);
    sb1.setString(std::to_string(score));

    sf::Text sb2;
    sb2.setFont(font);
    sb2.setPosition(wWidth * 3 / 4, 30);
    sb2.setCharacterSize(60);
    sb2.setFillColor(sf::Color::White);
    sb2.setString(std::to_string(score2));

    sf::Text paused;
    paused.setFont(font);
    paused.setPosition((wWidth / 2) - 150, (wHeight / 2) - 150);
    paused.setCharacterSize(70);
    paused.setFillColor(sf::Color::White);
    paused.setString("");

    bool pause = false;
    // MAIN LOOP
    while (window.isOpen()) {
        // WINDOW CLOSE
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                pause = !pause;
        }

        if (!pause) {
            // GAME MOVEMENT
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && Lpaddle.getPosition().y > 0) {
                Lpaddle.move(0.f, -4.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && Lpaddle.getPosition().y + Lpaddle.getSize().y < window.getSize().y) {
                Lpaddle.move(0.f, 4.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && Rpaddle.getPosition().y > 0) {
                Rpaddle.move(0.f, -4.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && Rpaddle.getPosition().y + Rpaddle.getSize().y < window.getSize().y) {
                Rpaddle.move(0.f, 4.0f);
            }
            ball.move(ballVelocity);

            // COLLISION DETECTIONS - LEFT PADDLE
            if (ball.getGlobalBounds().intersects(Lpaddle.getGlobalBounds())) {
                ballVelocity = newVelocity(ball.getPosition(), Lpaddle.getPosition(), Lpaddle.getSize().y, velocityMagnitude);
                ballVelocity *= -1.f;
                ball.setPosition(Lpaddle.getPosition().x + Lpaddle.getSize().x + 1, ball.getPosition().y);
            }

            // COLLISION DETECTIONS - RIGHT PADDLE
            if (ball.getGlobalBounds().intersects(Rpaddle.getGlobalBounds())) {
                ballVelocity = newVelocity(ball.getPosition(), Rpaddle.getPosition(), Rpaddle.getSize().y, velocityMagnitude);
                ball.setPosition(Rpaddle.getPosition().x - ball.getRadius() * 2 - 1, ball.getPosition().y);
            }

            // COLLISION DETECTION - EDGES
            if (ball.getPosition().y < 0 || ball.getPosition().y + ball.getRadius() * 2 > window.getSize().y) {
                ballVelocity.y = -ballVelocity.y;
            }

            // SCORE UPDATE
            if (ball.getPosition().x < 0) {
                score2++;
                sb2.setString(std::to_string(score2));
                ball.setPosition(wWidth / 2, wHeight / 2);
                ballVelocity = sf::Vector2f(1.f, 1.f);
            }
            if (ball.getPosition().x + ball.getRadius() * 2 > window.getSize().x) {
                score++;
                sb1.setString(std::to_string(score));
                ball.setPosition(wWidth / 2, wHeight / 2);
                ballVelocity = sf::Vector2f(-1.f, -1.f);
            }
        }
        if (pause) {
            paused.setString("PAUSED");
            // CHECK IF THE BALL IS IN THE "PAUSED" TEXT
            if (ball.getGlobalBounds().intersects(paused.getGlobalBounds())) {
                ball.setFillColor(sf::Color::Black);
            }
        }
        if (!pause) {
            paused.setString("");
            ball.setFillColor(sf::Color::White);
        }
        // WINDOW CLEAR AND DRAWING OBJECTS
        window.clear();
        window.draw(sb1);
        window.draw(sb2);
        window.draw(ball);
        window.draw(Lpaddle);
        window.draw(Rpaddle);
        window.draw(paused);
        window.display();
    }

    return 0;
}
