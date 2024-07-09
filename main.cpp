#include "pch.h"

// Função para normalizar um vetor
sf::Vector2f normalizar(const sf::Vector2f& vetor) {
    float magnitude = std::sqrt(pow(vetor.x, 2) + pow(vetor.y, 2));
    return sf::Vector2f(vetor.x / magnitude, vetor.y / magnitude);
}

// Função para calcular o novo vetor de velocidade com base no ponto de contato
sf::Vector2f novaVelocidade(const sf::Vector2f& posBola, const sf::Vector2f& posPaddle, float heightPaddle, float velocidade) {
    float intY = (posPaddle.y + heightPaddle / 2.0f) - posBola.y;
    float intYNormal = (intY / (heightPaddle / 2.0f));
    float anguloRebote = intYNormal * (M_PI / 4);
    sf::Vector2f novaDirecao(-std::cos(anguloRebote), std::sin(anguloRebote));

    return normalizar(novaDirecao) * velocidade;
}

int main() {
    // TAMANHO E DEFINIÇÃO DA JANELA
    const int wWidth = 1024;
    const int wHeight = 720;

    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight, 32), "Pong");
    window.setFramerateLimit(90);

    // INICIALIZAÇÃO DOS PLACARES
    int score = 0;
    int score2 = 0;

    sf::Font font;
    if (!font.loadFromFile("./Minecraft.ttf")) {
        std::cout << "Erro ao carregar a fonte!";
        exit(-1);
    }

    // INICIALIZAÇÃO DA BOLA E PADDLES
    sf::CircleShape ball(10.f);
    ball.setFillColor(sf::Color::White);
    float velocidadeInicial = 1.f;
    std::srand(static_cast<unsigned>(std::time(0)));
    float direcaoX = (std::rand() % 2 == 0) ? velocidadeInicial : -velocidadeInicial;
    float direcaoY = (std::rand() % 2 == 0) ? velocidadeInicial : -velocidadeInicial;
    sf::Vector2f velocidadeBola(direcaoX, direcaoY);
    ball.setPosition(wWidth / 2, wHeight / 2);
    float velocidadeMagnitude = 1.f;

    sf::RectangleShape paddle(sf::Vector2f(20.f, 100.f));
    paddle.setFillColor(sf::Color::White);
    paddle.setPosition(30.f, wHeight / 2 - paddle.getSize().y / 2);

    sf::RectangleShape paddle2(sf::Vector2f(20.f, 100.f));
    paddle2.setFillColor(sf::Color::White);
    paddle2.setPosition(wWidth - 50.f, wHeight / 2 - paddle2.getSize().y / 2);

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

    bool pausado = false;
    // LOOP PRINCIPAL
    while (window.isOpen()) {
        // FECHANDO A JANELA
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                pausado = !pausado;
        }

        if (!pausado) {
            // MOVIMENTAÇÃO DO JOGO + COLISÕES
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getPosition().y > 0) {
                paddle.move(0.f, -1.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getPosition().y + paddle.getSize().y < window.getSize().y) {
                paddle.move(0.f, 1.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && paddle2.getPosition().y > 0) {
                paddle2.move(0.f, -1.0f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && paddle2.getPosition().y + paddle2.getSize().y < window.getSize().y) {
                paddle2.move(0.f, 1.0f);
            }
            ball.move(velocidadeBola);

            // Verificação de colisão com os paddles
            if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
                velocidadeBola = novaVelocidade(ball.getPosition(), paddle.getPosition(), paddle.getSize().x, velocidadeMagnitude);

            }
            if (ball.getGlobalBounds().intersects(paddle2.getGlobalBounds())) {
                velocidadeBola = novaVelocidade(ball.getPosition(), paddle2.getPosition(), paddle2.getSize().y, velocidadeMagnitude);

            }

            // Verificação de colisão com as bordas da janela
            if (ball.getPosition().y < 0 || ball.getPosition().y + ball.getRadius() * 2 > window.getSize().y) {
                velocidadeBola.y = -velocidadeBola.y;
            }

            // Atualização do placar
            if (ball.getPosition().x < 0) {
                score2++;
                sb2.setString(std::to_string(score2));
                ball.setPosition(wWidth / 2, wHeight / 2);
                velocidadeBola = sf::Vector2f(1.f, 1.f);
            }
            if (ball.getPosition().x + ball.getRadius() * 2 > window.getSize().x) {
                score++;
                sb1.setString(std::to_string(score));
                ball.setPosition(wWidth / 2, wHeight / 2);
                velocidadeBola = sf::Vector2f(-1.f, -1.f);
            }

        }
        if (pausado) {
            paused.setString("PAUSADO");
            // Verifica se a bola está sobrepondo o texto "PAUSADO"
            if (ball.getGlobalBounds().intersects(paused.getGlobalBounds())) {
                ball.setFillColor(sf::Color::Black);
            }

        }
        if (!pausado) {
            paused.setString("");
            ball.setFillColor(sf::Color::White);
        }


        window.clear();
        window.draw(sb1);
        window.draw(sb2);
        window.draw(ball);
        window.draw(paddle);
        window.draw(paddle2);
        window.draw(paused);
        window.display();
    }

    return 0;
}