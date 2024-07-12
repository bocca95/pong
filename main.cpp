#include "pch.h"

// FUNCTION TO NORMALIZE A VECTOR
sf::Vector2f normalize(const sf::Vector2f& vector) 
{
    float magnitude = std::sqrt(pow(vector.x, 2) + pow(vector.y, 2));
    if (magnitude != 0) 
    {
        return sf::Vector2f(vector.x / magnitude, vector.y / magnitude);
    }
    else 
    {
        return sf::Vector2f(0.f, 0.f);
    }
}

// FUNCTION TO CALCULATE THE NEW VELOCITY BASED ON THE CONTACT POINT
sf::Vector2f newVelocity(const sf::Vector2f& ballPos, const sf::Vector2f& paddlePos, float paddleHeight, float velocity) 
{
    float relativeY = ballPos.y - (paddlePos.y + paddleHeight / 2.0f);
    float normalizedRelativeY = relativeY / (paddleHeight / 2.0f);
    float reboundAngle = normalizedRelativeY * (M_PI / 4);
    sf::Vector2f newDirection(-std::cos(reboundAngle), std::sin(reboundAngle));
    return normalize(newDirection) * velocity;
}

// FUNCTION TO INCREMENT POWERUP
int pwrupIncrement(int& powerup, int score) 
{
    if (score % 5 == 0) 
    {
        powerup += 1;
    }
    return powerup;
}

// FUNCTION TO HANDLE PADDLE MOVEMENT
void handlePaddleMovement(sf::RectangleShape& paddle, float moveUpKey, float moveDownKey, const sf::RenderWindow& window) 
{
    if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(moveUpKey)) && paddle.getPosition().y > 0) 
    {
        paddle.move(0.f, -4.0f);
    }
    if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(moveDownKey)) && paddle.getPosition().y + paddle.getSize().y < window.getSize().y) 
    {
        paddle.move(0.f, 4.0f);
    }
}

// FUNCTION TO HANDLE BALL MOVEMENT AND COLLISION
void handleBallMovementAndCollision(sf::CircleShape& ball, sf::Vector2f& ballVelocity, const sf::RenderWindow& window, sf::RectangleShape& Lpaddle, sf::RectangleShape& Rpaddle, float& velocityMagnitude) 
{
    ball.move(ballVelocity);

    if (ball.getGlobalBounds().intersects(Lpaddle.getGlobalBounds())) 
    {
        ballVelocity = newVelocity(ball.getPosition(), Lpaddle.getPosition(), Lpaddle.getSize().y, velocityMagnitude);
        ballVelocity.x = -ballVelocity.x;
    }

    if (ball.getGlobalBounds().intersects(Rpaddle.getGlobalBounds())) 
    {
        ballVelocity = newVelocity(ball.getPosition(), Rpaddle.getPosition(), Rpaddle.getSize().y, velocityMagnitude);
    }

    if (ball.getPosition().y < 0 || ball.getPosition().y + ball.getRadius() * 2 > window.getSize().y) 
    {
        ballVelocity.y = -ballVelocity.y;
    }
}

// FUNCTION TO UPDATE SCORE
void updateScore(sf::CircleShape& ball, sf::Vector2f& ballVelocity, sf::Text& scoreText, int& score, int& pwr_count, int wWidth, int wHeight, bool& powerup, sf::Text& powerUpText) 
{
    score++;
    pwrupIncrement(pwr_count, score);
    scoreText.setString(std::to_string(score));
    ball.setPosition(wWidth / 2, wHeight / 2);
    ballVelocity = sf::Vector2f((ballVelocity.x > 0 ? 3.5f : -3.5f), (ballVelocity.y > 0 ? 3.5f : -3.5f));
    powerUpText.setString(std::to_string(pwr_count));
}

// FUNCTION TO HANDLE POWERUP STATE
void handlePowerupState(sf::Vector2f& ballVelocity, bool& powerup) 
{
    if (powerup) {
        ballVelocity.x = ballVelocity.x * 2.5f;
        ballVelocity.y = ballVelocity.y * 2.5f;
        powerup = false;
    }
}

// FUNCTION TO HANDLE PAUSE STATE
void handlePauseState(sf::Text& paused, sf::CircleShape& ball, bool pause) 
{
    if (pause) 
    {
        paused.setString("PAUSED");
        if (ball.getGlobalBounds().intersects(paused.getGlobalBounds())) 
        {
            ball.setFillColor(sf::Color::Black);
        }
    }
    else 
    {
        paused.setString("");
        ball.setFillColor(sf::Color::White);
    }
}


void drawProgress(int& count, sf::RenderWindow& window, sf::RectangleShape& progresso, sf::RectangleShape& progressoPlayer) 
{
    sf::Vector2f size = progresso.getSize();

    if (count < 4) 
    {
        progressoPlayer.setSize(sf::Vector2f((size.x / 5) * (count + 1), size.y));
        count++;
    }
    else {
        progressoPlayer.setSize(sf::Vector2f(0, size.y));
        count = 0; 
    }
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
    int count1 = 0;
    int count2 = 0;

    sf::Font font;
    if (!font.loadFromFile("./Minecraft.ttf")) {
        std::cout << "Error loading font!";
        exit(-1);
    }

    // BALL, PADDLES AND TEXT INIT.
    sf::CircleShape ball(10.f);
    ball.setFillColor(sf::Color::White);
    float initialVelocity = 6.f;
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

    int p1_power = 2;
    int p2_power = 2;
    
    sf::Text player1;
    player1.setFont(font);
    player1.setPosition(wWidth/4, wHeight* 7/8-50);
    player1.setCharacterSize(70);
    player1.setFillColor(sf::Color::White);
    player1.setString(std::to_string(p1_power));

    sf::Text player2;
    player2.setFont(font);
    player2.setPosition(wWidth * 3 / 4, (wHeight * 7 / 8)-50);
    player2.setCharacterSize(70);
    player2.setFillColor(sf::Color::White);
    player2.setString(std::to_string(p2_power));
 
    sf::RectangleShape progressbar(sf::Vector2f(100.f, 20.f));
    progressbar.setPosition((wWidth / 4) - 30, (wHeight * 7 / 8) + 40);
    progressbar.setFillColor(sf::Color::White);

    sf::RectangleShape progressbar2(sf::Vector2f(100.f, 20.f));
    progressbar2.setPosition((wWidth * 3/4) - 30, (wHeight * 7 / 8) + 40);
    progressbar2.setFillColor(sf::Color::White);

    sf::RectangleShape progressp1(sf::Vector2f(0.f, 20.f));  // Initialize with height and zero width
    progressp1.setPosition((wWidth / 4) - 30, (wHeight * 7 / 8) + 40);
    progressp1.setFillColor(sf::Color::Green);

    sf::RectangleShape progressp2(sf::Vector2f(0.f, 20.f));
    progressp2.setPosition((wWidth * 3/4) - 30, (wHeight * 7 / 8) + 40);
    progressp2.setFillColor(sf::Color::Green);


    bool powerup = false;
    bool pause = false;


    // MAIN LOOP
    while (window.isOpen()) 
    {
        // WINDOW CLOSE
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                pause = !pause;
            if (p1_power > 0) 
            {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::H) 
                {
                    powerup = true;
                    p1_power -= 1;
                    player1.setString(std::to_string(p1_power));
                }
            }
            if (p2_power > 0)
            {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) 
                {
                    powerup = true;
                    p2_power -= 1;
                    player2.setString(std::to_string(p2_power));
                }
            }

        }

        handlePowerupState(ballVelocity, powerup);
        handlePauseState(paused, ball, pause);

        if (!pause) 
        {
            handlePaddleMovement(Lpaddle, sf::Keyboard::A, sf::Keyboard::D, window);
            handlePaddleMovement(Rpaddle, sf::Keyboard::Left, sf::Keyboard::Right, window);

            handleBallMovementAndCollision(ball, ballVelocity, window, Lpaddle, Rpaddle, velocityMagnitude);

            
            if (ball.getPosition().x + ball.getRadius() * 2 > window.getSize().x) 
            {
                updateScore(ball, ballVelocity, sb1, score, p1_power, wWidth, wHeight, powerup, player1);
                drawProgress(count1, window, progressbar, progressp1);  // Change this to draw progress for player 2 if needed
            }
            if (ball.getPosition().x < 0) 
            {
                updateScore(ball, ballVelocity, sb2, score2, p2_power, wWidth, wHeight, powerup, player2);
                drawProgress(count2, window, progressbar, progressp2);
            }

        }

        
        // WINDOW CLEAR AND DRAWING OBJECTS
        window.clear();
        window.draw(sb1);
        window.draw(sb2);
        window.draw(ball);
        window.draw(Lpaddle);
        window.draw(Rpaddle);
        window.draw(player1);
        window.draw(player2);
        window.draw(paused);
        window.draw(progressbar);
        window.draw(progressbar2);
        window.draw(progressp1);
        window.draw(progressp2);
        window.display();
    }

    return 0;
}
