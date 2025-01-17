/*
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <sstream>
#include <vector>

// Funkcja wyœwietlaj¹ca ekran pomocy
void showHelp(sf::RenderWindow& window, sf::Font& font) {
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(24);
    helpText.setFillColor(sf::Color::White);
    helpText.setString("Jak grac:\n\n- Sterowanie platforma: Strzalki lewo/prawo\n"
        "- Zbijaj bloki pilka\n"
        "- Tracisz zycie, gdy pilka spadnie\n"
        "- Gra konczy sie, gdy stracisz 3 zycia\n\n"
        "Wcisnij ESC, aby wrocic do menu.");
    helpText.setPosition(50, 50);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                return; // Powrót do menu
            }
        }

        window.clear();
        window.draw(helpText);
        window.display();
    }
}

// Funkcja g³ówna gry
int runGame(sf::RenderWindow& window, sf::Font& font) {
    // Platforma
    sf::RectangleShape paddle(sf::Vector2f(120, 20));
    paddle.setFillColor(sf::Color::Blue);
    paddle.setPosition(340, 550);

    // Pilka
    sf::CircleShape ball(10.f);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition(400, 300);
    sf::Vector2f ballVelocity(4.f, -4.f);

    // Bloki (pozycje i licznik trafieñ)
    sf::RectangleShape block(sf::Vector2f(60, 20));
    block.setFillColor(sf::Color::Green);
    std::vector<std::pair<sf::RectangleShape, int>> blocks;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 4; ++j) {
            block.setPosition(60 + i * 65, 50 + j * 25);
            blocks.emplace_back(block, 3); // Ka¿dy blok ma 3 "¿ycia"
        }
    }

    // Tekst do wyœwietlania punktów i ¿yæ
    sf::Text scoreText, livesText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(10, 40);

    // Liczba ¿yæ i punktów
    int lives = 3;
    int score = 0;

    while (window.isOpen()) {
        // Obs³uga zdarzeñ
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1; // Zakoñczenie gry
            }
        }

        // Ruch platformy
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getPosition().x > 0)
            paddle.move(-8.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getPosition().x < 680)
            paddle.move(8.f, 0.f);

        // Ruch pi³ki
        ball.move(ballVelocity);

        // Kolizja z krawêdziami okna
        if (ball.getPosition().x < 0 || ball.getPosition().x > 790)
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y < 0)
            ballVelocity.y = -ballVelocity.y;

        // Kolizja z platform¹
        if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
            ballVelocity.y = -ballVelocity.y;

        // Kolizja z blokami
        for (auto it = blocks.begin(); it != blocks.end();) {
            if (ball.getGlobalBounds().intersects(it->first.getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;
                it->second--; // Zmniejsz liczbê "¿ycia" ceg³y

                if (it->second <= 0) {
                    it = blocks.erase(it); // Usuñ ceg³ê, jeœli licznik "¿ycia" osi¹gnie 0
                    score += 10;
                }
                else {
                    ++it;
                }
            }
            else {
                ++it;
            }
        }

        // Sprawdzenie, czy pi³ka wypad³a z ekranu
        if (ball.getPosition().y > 600) {
            lives--;
            ball.setPosition(400, 300);
            ballVelocity = sf::Vector2f(4.f, -4.f);

            if (lives <= 0) {
                return score; // Zwrócenie wyniku po zakoñczeniu gry
            }
        }

        // Aktualizacja tekstów
        std::ostringstream scoreStream, livesStream;
        scoreStream << "Punkty: " << score;
        scoreText.setString(scoreStream.str());

        livesStream << "Zycia: " << lives;
        livesText.setString(livesStream.str());

        // Rysowanie
        window.clear();
        window.draw(paddle);
        window.draw(ball);
        for (const auto& block : blocks)
            window.draw(block.first); // Rysuj ceg³y (tylko kszta³ty)

        window.draw(scoreText);
        window.draw(livesText);

        window.display();
    }

    return score;
}

// Funkcja g³ówna
int main() {
    // Okno gry
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arkanoid");
    window.setFramerateLimit(60);

    // Czcionka
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie mozna zaladowac czcionki arial.ttf!" << std::endl;
        return -1;
    }

    // Menu g³ówne
    sf::Text menuText[3];
    std::string options[] = { "Graj", "Jak grac", "Wyjscie" };
    for (int i = 0; i < 3; ++i) {
        menuText[i].setFont(font);
        menuText[i].setCharacterSize(36);
        menuText[i].setFillColor(i == 0 ? sf::Color::Red : sf::Color::White); // Podœwietlenie pierwszej opcji
        menuText[i].setString(options[i]);
        menuText[i].setPosition(350, 200 + i * 50);
    }

    int selectedOption = 0;

    while (window.isOpen()) {
        // Obs³uga zdarzeñ
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedOption = (selectedOption + 2) % 3;
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    selectedOption = (selectedOption + 1) % 3;
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    if (selectedOption == 0) {
                        // Graj
                        int score = runGame(window, font);
                        std::cout << "Twoj wynik: " << score << std::endl;
                    }
                    else if (selectedOption == 1) {
                        // Jak graæ
                        showHelp(window, font);
                    }
                    else if (selectedOption == 2) {
                        // Wyjœcie
                        window.close();
                    }
                }
            }
        }

        // Aktualizacja podœwietlenia menu
        for (int i = 0; i < 3; ++i) {
            menuText[i].setFillColor(i == selectedOption ? sf::Color::Red : sf::Color::White);
        }

        // Rysowanie menu
        window.clear();
        for (int i = 0; i < 3; ++i) {
            window.draw(menuText[i]);
        }
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <sstream>
#include <vector>

// Funkcja wyœwietlaj¹ca ekran koñcowy
void showGameOverScreen(sf::RenderWindow& window, sf::Font& font, int score) {
    sf::Text gameOverText, scoreText;

    // Tekst "Koniec gry"
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setString("KONIEC GRY");
    gameOverText.setPosition(250, 200);

    // Tekst z wynikiem
    scoreText.setFont(font);
    scoreText.setCharacterSize(36);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Twoj wynik: " + std::to_string(score));
    scoreText.setPosition(250, 300);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                return; // Wyjœcie z ekranu koñcowego
            }
        }

        window.clear();
        window.draw(gameOverText);
        window.draw(scoreText);
        window.display();
    }
}

int runGame(sf::RenderWindow& window, sf::Font& font) {
    // Platforma
    sf::RectangleShape paddle(sf::Vector2f(120, 20));
    paddle.setFillColor(sf::Color::Blue);
    paddle.setPosition(340, 550);

    // Pilka
    sf::CircleShape ball(10.f);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition(400, 300);
    sf::Vector2f ballVelocity(4.f, -4.f);

    // Bloki
    sf::RectangleShape block(sf::Vector2f(60, 20));
    std::vector<std::pair<sf::RectangleShape, int>> blocks; // Bloki z licznikiem uderzeñ
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 4; ++j) {
            block.setPosition(60 + i * 65, 50 + j * 25);
            block.setFillColor(sf::Color::Green);
            blocks.emplace_back(block, 3); // Ka¿dy blok wymaga 3 uderzeñ
        }
    }

    // Tekst do wyœwietlania punktów i ¿yæ
    sf::Text scoreText, livesText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(10, 40);

    int lives = 3;
    int score = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }
        }

        // Ruch platformy
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getPosition().x > 0)
            paddle.move(-8.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getPosition().x < 680)
            paddle.move(8.f, 0.f);

        // Ruch pi³ki
        ball.move(ballVelocity);

        // Kolizja z krawêdziami okna
        if (ball.getPosition().x < 0 || ball.getPosition().x > 790)
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y < 0)
            ballVelocity.y = -ballVelocity.y;

        // Kolizja z platform¹
        if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
            ballVelocity.y = -ballVelocity.y;

        // Kolizja z blokami
        for (auto it = blocks.begin(); it != blocks.end();) {
            if (ball.getGlobalBounds().intersects(it->first.getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;

                // Zmniejsz licznik uderzeñ i zmieñ kolor bloku
                it->second -= 1;
                if (it->second == 2) {
                    it->first.setFillColor(sf::Color::Yellow);
                }
                else if (it->second == 1) {
                    it->first.setFillColor(sf::Color::Red);
                }
                else if (it->second == 0) {
                    it = blocks.erase(it);
                    score += 10;
                    continue;
                }
            }
            ++it;
        }

        // Sprawdzenie, czy pi³ka wypad³a z ekranu
        if (ball.getPosition().y > 600) {
            lives--;
            ball.setPosition(400, 300);
            ballVelocity = sf::Vector2f(4.f, -4.f);

            if (lives <= 0) {
                showGameOverScreen(window, font, score);
                return score;
            }
        }

        // Aktualizacja tekstów
        std::ostringstream scoreStream, livesStream;
        scoreStream << "Punkty: " << score;
        scoreText.setString(scoreStream.str());

        livesStream << "Zycia: " << lives;
        livesText.setString(livesStream.str());

        // Rysowanie
        window.clear();
        window.draw(paddle);
        window.draw(ball);
        for (const auto& block : blocks)
            window.draw(block.first);

        window.draw(scoreText);
        window.draw(livesText);
        window.display();
    }

    return score;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arkanoid");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie mozna zaladowac czcionki arial.ttf!" << std::endl;
        return -1;
    }

    sf::Text menuText[3];
    std::string options[] = { "Graj", "Jak grac", "Wyjscie" };
    for (int i = 0; i < 3; ++i) {
        menuText[i].setFont(font);
        menuText[i].setCharacterSize(36);
        menuText[i].setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
        menuText[i].setString(options[i]);
        menuText[i].setPosition(350, 200 + i * 50);
    }

    int selectedOption = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedOption = (selectedOption + 2) % 3;
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    selectedOption = (selectedOption + 1) % 3;
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    if (selectedOption == 0) {
                        int score = runGame(window, font);
                        std::cout << "Twoj wynik: " << score << std::endl;
                    }
                    else if (selectedOption == 1) {
                        // Jak graæ
                        // Implementacja pomocy

                    }
                    else if (selectedOption == 2) {
                        window.close();
                    }
                }
            }
        }

        for (int i = 0; i < 3; ++i) {
            menuText[i].setFillColor(i == selectedOption ? sf::Color::Red : sf::Color::White);
        }

        window.clear();
        for (int i = 0; i < 3; ++i) {
            window.draw(menuText[i]);
        }

        window.display();
    }

    return 0;
}
*/
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <sstream>
#include <vector>

// Funkcja wyœwietlaj¹ca ekran koñcowy
void showGameOverScreen(sf::RenderWindow& window, sf::Font& font, int score) {
    sf::Text gameOverText, scoreText;

    // Tekst "Koniec gry"
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setString("KONIEC GRY");
    gameOverText.setPosition(250, 200);

    // Tekst z wynikiem
    scoreText.setFont(font);
    scoreText.setCharacterSize(36);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Twoj wynik: " + std::to_string(score));
    scoreText.setPosition(250, 300);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                return; // Wyjœcie z ekranu koñcowego
            }
        }

        window.clear();
        window.draw(gameOverText);
        window.draw(scoreText);
        window.display();
    }
}

// Funkcja wyœwietlaj¹ca ekran "Jak graæ"
void showHowToPlayScreen(sf::RenderWindow& window, sf::Font& font) {
    sf::Text howToPlayText, instructionText;

    // Nag³ówek
    howToPlayText.setFont(font);
    howToPlayText.setCharacterSize(48);
    howToPlayText.setFillColor(sf::Color::White);
    howToPlayText.setString("Jak grac");
    howToPlayText.setPosition(250, 100);

    // Instrukcja
    instructionText.setFont(font);
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setString("Sterowanie:\n\nStrzalki lewo/prawo - ruch platformy\nZbijaj bloki pilka\nZbieraj punkty za niszczenie blokow\nUnikaj utraty pilki - masz 3 zycia");
    instructionText.setPosition(100, 200);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                return; // Wyjœcie z ekranu "Jak graæ"
            }
        }

        window.clear();
        window.draw(howToPlayText);
        window.draw(instructionText);
        window.display();
    }
}

int runGame(sf::RenderWindow& window, sf::Font& font) {
    // Platforma
    sf::RectangleShape paddle(sf::Vector2f(120, 20));
    paddle.setFillColor(sf::Color::Blue);
    paddle.setPosition(340, 550);

    // Pilka
    sf::CircleShape ball(10.f);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition(400, 300);
    sf::Vector2f ballVelocity(4.f, -4.f);

    // Bloki
    sf::RectangleShape block(sf::Vector2f(60, 20));
    std::vector<std::pair<sf::RectangleShape, int>> blocks; // Bloki z licznikiem uderzeñ
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 4; ++j) {
            block.setPosition(60 + i * 65, 50 + j * 25);
            block.setFillColor(sf::Color::Green);
            blocks.emplace_back(block, 3); // Ka¿dy blok wymaga 3 uderzeñ
        }
    }

    // Tekst do wyœwietlania punktów i ¿yæ
    sf::Text scoreText, livesText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(10, 40);

    int lives = 3;
    int score = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return -1;
            }
        }

        // Ruch platformy
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getPosition().x > 0)
            paddle.move(-8.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getPosition().x < 680)
            paddle.move(8.f, 0.f);

        // Ruch pi³ki
        ball.move(ballVelocity);

        // Kolizja z krawêdziami okna
        if (ball.getPosition().x < 0 || ball.getPosition().x > 790)
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y < 0)
            ballVelocity.y = -ballVelocity.y;

        // Kolizja z platform¹
        if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
            ballVelocity.y = -ballVelocity.y;

        // Kolizja z blokami
        for (auto it = blocks.begin(); it != blocks.end();) {
            if (ball.getGlobalBounds().intersects(it->first.getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;

                // Zmniejsz licznik uderzeñ i zmieñ kolor bloku
                it->second -= 1;
                if (it->second == 2) {
                    it->first.setFillColor(sf::Color::Yellow);
                }
                else if (it->second == 1) {
                    it->first.setFillColor(sf::Color::Red);
                }
                else if (it->second == 0) {
                    it = blocks.erase(it);
                    score += 10;
                    continue;
                }
            }
            ++it;
        }

        // Sprawdzenie, czy pi³ka wypad³a z ekranu
        if (ball.getPosition().y > 600) {
            lives--;
            ball.setPosition(400, 300);
            ballVelocity = sf::Vector2f(4.f, -4.f);

            if (lives <= 0) {
                showGameOverScreen(window, font, score);
                return score;
            }
        }

        // Aktualizacja tekstów
        std::ostringstream scoreStream, livesStream;
        scoreStream << "Punkty: " << score;
        scoreText.setString(scoreStream.str());

        livesStream << "Zycia: " << lives;
        livesText.setString(livesStream.str());

        // Rysowanie
        window.clear();
        window.draw(paddle);
        window.draw(ball);
        for (const auto& block : blocks)
            window.draw(block.first);

        window.draw(scoreText);
        window.draw(livesText);
        window.display();
    }

    return score;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arkanoid");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie mozna zaladowac czcionki arial.ttf!" << std::endl;
        return -1;
    }

    sf::Text menuText[3];
    std::string options[] = { "Graj", "Jak grac", "Wyjscie" };
    for (int i = 0; i < 3; ++i) {
        menuText[i].setFont(font);
        menuText[i].setCharacterSize(36);
        menuText[i].setFillColor
        (i == 0 ? sf::Color::Red : sf::Color::White);
        menuText[i].setString(options[i]);
        menuText[i].setPosition(350, 200 + i * 50);
    }

    int selectedOption = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedOption = (selectedOption + 2) % 3;
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    selectedOption = (selectedOption + 1) % 3;
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    if (selectedOption == 0) {
                        int score = runGame(window, font);
                        std::cout << "Twoj wynik: " << score << std::endl;
                    }
                    else if (selectedOption == 1) {
                        showHowToPlayScreen(window, font);
                    }
                    else if (selectedOption == 2) {
                        window.close();
                    }
                }
            }
        }

        for (int i = 0; i < 3; ++i) {
            menuText[i].setFillColor(i == selectedOption ? sf::Color::Red : sf::Color::White);
        }

        window.clear();
        for (int i = 0; i < 3; ++i) {
            window.draw(menuText[i]);
        }
        window.display();
    }

    return 0;
}


