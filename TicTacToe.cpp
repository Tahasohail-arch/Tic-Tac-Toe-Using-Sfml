// Taha Sohail 
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
using namespace std;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 1024;
const int GRID_SIZE = 600; 
const int GRID_LEFT = (WINDOW_WIDTH - GRID_SIZE) / 2;
const int GRID_TOP = (WINDOW_HEIGHT - GRID_SIZE) / 2;
const int CELL_WIDTH = GRID_SIZE / 3;
const int CELL_HEIGHT = GRID_SIZE / 3;

string board[3][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"}};

int currentPlayer = 1;
int player1wins = 0, player2wins = 0;
bool win = false;
bool gameOver = false;
bool inMenu = true;
bool Pause = false;
int drawCount = 0;
bool showingScoreboard = false;
bool transitionSoundPlayed = false;

sf::Font font;
sf::Texture backgroundTexture;
sf::Sprite backgroundSprite;
sf::Clock blinkClock;
sf::Texture playTexture, resumeTexture, exitTexture;
sf::Sprite playButton, resumeButton, exitButton;
sf::RectangleShape fadeOverlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
sf::Clock xoGlowClock;
sf::Clock pulseClock;
sf::Texture gameBackgroundTexture;
sf::Sprite gameBackgroundSprite;
sf::Sprite pauseResumeButton, pauseRestartButton, pauseExitButton;
sf::Texture pauseButtonTexture, resumeButtonTexture, pauseExitButtonTexture;
sf::SoundBuffer clickBuffer, victoryBuffer, drawBuffer, transitionBuffer;
sf::Sound clickSound, victorySound, drawSound, transitionSound;
sf::Music menuMusic, gameMusic;
bool isMuted = false;

bool isGamePaused = false;
bool transitioning = false;
float transitionAlpha = 255.0f; // starts fully opaque
bool transitionToGame = false;

void drawGrid(sf::RenderWindow &window)
{
    float time = pulseClock.getElapsedTime().asSeconds();
    float pulse = std::sin(time * 2.0f) * 0.5f + 0.5f;

    // Animate glow color between red and blue
    sf::Color glowColor(
        static_cast<sf::Uint8>(255 * pulse),
        static_cast<sf::Uint8>(100),
        static_cast<sf::Uint8>(255 * (1.0f - pulse)),
        static_cast<sf::Uint8>(100 + 100 * pulse));

    sf::Color coreColor(255, 255, 255); // White core lines

    float glowThickness = 18 + 4 * pulse;
    float coreThickness = 6;

    // Background rectangle (rounded effect via corners)
    sf::RectangleShape gridBG(sf::Vector2f(GRID_SIZE, GRID_SIZE));
    gridBG.setPosition(GRID_LEFT, GRID_TOP);
    gridBG.setFillColor(sf::Color(20, 0, 40, 200)); // dark transparent violet
    gridBG.setOutlineColor(glowColor);
    gridBG.setOutlineThickness(10);
    window.draw(gridBG);

    // Draw horizontal grid lines
    for (int i = 1; i < 3; ++i)
    {
        float y = GRID_TOP + i * CELL_HEIGHT;

        // Glow
        sf::RectangleShape glowLine(sf::Vector2f(GRID_SIZE, glowThickness));
        glowLine.setFillColor(glowColor);
        glowLine.setOrigin(0, glowThickness / 2);
        glowLine.setPosition(GRID_LEFT, y);
        window.draw(glowLine);

        // Core
        sf::RectangleShape coreLine(sf::Vector2f(GRID_SIZE, coreThickness));
        coreLine.setFillColor(coreColor);
        coreLine.setOrigin(0, coreThickness / 2);
        coreLine.setPosition(GRID_LEFT, y);
        window.draw(coreLine);
    }

    // Draw vertical grid lines
    for (int i = 1; i < 3; ++i)
    {
        float x = GRID_LEFT + i * CELL_WIDTH;

        // Glow
        sf::RectangleShape glowLine(sf::Vector2f(glowThickness, GRID_SIZE));
        glowLine.setFillColor(glowColor);
        glowLine.setOrigin(glowThickness / 2, 0);
        glowLine.setPosition(x, GRID_TOP);
        window.draw(glowLine);

        // Core
        sf::RectangleShape coreLine(sf::Vector2f(coreThickness, GRID_SIZE));
        coreLine.setFillColor(coreColor);
        coreLine.setOrigin(coreThickness / 2, 0);
        coreLine.setPosition(x, GRID_TOP);
        window.draw(coreLine);
    }

    // Rounded glowing border corners (optional sparkle points)
    std::vector<sf::Vector2f> corners = {
        {GRID_LEFT, GRID_TOP},
        {GRID_LEFT + GRID_SIZE, GRID_TOP},
        {GRID_LEFT, GRID_TOP + GRID_SIZE},
        {GRID_LEFT + GRID_SIZE, GRID_TOP + GRID_SIZE}};

    for (auto &corner : corners)
    {
        sf::CircleShape circleGlow(glowThickness);
        circleGlow.setOrigin(glowThickness, glowThickness);
        circleGlow.setPosition(corner);
        circleGlow.setFillColor(glowColor);
        window.draw(circleGlow);
    }
}

void drawBoard(sf::RenderWindow &window)
{
    float time = xoGlowClock.getElapsedTime().asSeconds();
    float glow = std::sin(time * 3.0f) * 0.5f + 0.5f;
    sf::Color glowColor(255, 255, 255, static_cast<sf::Uint8>(100 + 100 * glow)); // Animated white glow

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            string cell = board[row][col];
            float centerX = GRID_LEFT + col * CELL_WIDTH + CELL_WIDTH / 2;
            float centerY = GRID_TOP + row * CELL_HEIGHT + CELL_HEIGHT / 2;

            if (cell == "X")
            {
                // Draw glow for X
                sf::RectangleShape glowLine1(sf::Vector2f(140, 30));
                sf::RectangleShape glowLine2(sf::Vector2f(140, 30));
                glowLine1.setFillColor(glowColor);
                glowLine2.setFillColor(glowColor);
                glowLine1.setOrigin(70, 15);
                glowLine2.setOrigin(70, 15);
                glowLine1.setPosition(centerX, centerY);
                glowLine2.setPosition(centerX, centerY);
                glowLine1.setRotation(45);
                glowLine2.setRotation(-45);
                window.draw(glowLine1);
                window.draw(glowLine2);

                // Draw actual X
                sf::RectangleShape line1(sf::Vector2f(140, 20));
                sf::RectangleShape line2(sf::Vector2f(140, 20));
                line1.setFillColor(sf::Color(0, 255, 255)); // Neon cyan
                line2.setFillColor(sf::Color(0, 255, 255));
                line1.setOrigin(70, 10);
                line2.setOrigin(70, 10);
                line1.setPosition(centerX, centerY);
                line2.setPosition(centerX, centerY);
                line1.setRotation(45);
                line2.setRotation(-45);
                window.draw(line1);
                window.draw(line2);
            }
            else if (cell == "O")
            {
                // Draw glow for O
                sf::CircleShape glowCircle(65);
                glowCircle.setFillColor(sf::Color::Transparent);
                glowCircle.setOutlineColor(glowColor);
                glowCircle.setOutlineThickness(20.0f);
                glowCircle.setOrigin(65, 65);
                glowCircle.setPosition(centerX, centerY);
                window.draw(glowCircle);

                // Draw actual O
                sf::CircleShape circle(60);
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color(255, 105, 180)); // Neon pink
                circle.setOutlineThickness(10.0f);
                circle.setOrigin(60, 60);
                circle.setPosition(centerX, centerY);
                window.draw(circle);
            }
        }
    }
}

bool checkWin()
{
    for (int i = 0; i < 3; ++i)
    {
        // Check rows
        if (!board[i][0].empty() && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return true;

        // Check columns
        if (!board[0][i].empty() && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return true;
    }

    // Check diagonals
    if (!board[0][0].empty() && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return true;

    if (!board[0][2].empty() && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return true;

    return false;
}

void drawScoreboard(sf::RenderWindow &window)
{
    sf::Text scoreboard;
    scoreboard.setFont(font);
    scoreboard.setCharacterSize(30);
    scoreboard.setStyle(sf::Text::Bold);

    // Neon cyan pulse glow
    float time = pulseClock.getElapsedTime().asSeconds();
    float pulse = std::sin(time * 3.5f) * 0.5f + 0.5f;
    sf::Color neonCyan(0, 255, 255, static_cast<sf::Uint8>(180 + 75 * pulse));

    scoreboard.setFillColor(neonCyan);
    scoreboard.setOutlineColor(sf::Color::White);
    scoreboard.setOutlineThickness(2);

    // Plain text scoreboard
    std::string text = "Player 1: " + std::to_string(player1wins) +
                       "   |   Player 2: " + std::to_string(player2wins) +
                       "   |   Draws: " + std::to_string(drawCount);

    scoreboard.setString(text);

    // Center horizontally at top
    sf::FloatRect bounds = scoreboard.getLocalBounds();
    scoreboard.setOrigin(bounds.width / 2, 0);
    scoreboard.setPosition(WINDOW_WIDTH / 2.0f, 20); // Top center with margin

    window.draw(scoreboard);
}

bool isDraw()
{
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (board[r][c].empty()) // no X or O yet
                return false;
    return true;
}

void handleClick(int x, int y)
{
    if (gameOver)
        return;

    int col = (x - GRID_LEFT) / CELL_WIDTH;
    int row = (y - GRID_TOP) / CELL_HEIGHT;

    if (row >= 0 && row < 3 && col >= 0 && col < 3)
    {
        if (board[row][col] != "X" && board[row][col] != "O")
        {
            string marker = (currentPlayer == 1) ? "X" : "O";
            board[row][col] = marker;

            clickSound.play();

            if (checkWin())
            {
                win = true;
                gameOver = true;
                if (currentPlayer == 1)
                    player1wins++;
                else
                    player2wins++;
            }
            else if (isDraw())
            {
                gameOver = true;
                drawCount++;
            }

            currentPlayer = (currentPlayer == 1) ? 2 : 1;
        }
    }
}

void drawGameOver(sf::RenderWindow &window)
{
    static sf::Clock fadeClock;
    float time = fadeClock.getElapsedTime().asSeconds();

    // Glow/fade animation: Alpha oscillates between 100 and 255
    float glowAlpha = 100 + std::sin(time * 2.0f) * 80;                   // Pulsing alpha
    sf::Color glowColor(255, 50, 100, static_cast<sf::Uint8>(glowAlpha)); // Neon pink/red glow

    sf::Text msg;
    msg.setFont(font);
    msg.setCharacterSize(48);
    msg.setFillColor(glowColor);

    if (win)
        msg.setString("Player " + to_string(currentPlayer == 1 ? 2 : 1) + " wins! Click to restart.");
    else if (gameOver)
        msg.setString("It's a draw! Click to restart.");

    // Center horizontally
    sf::FloatRect msgBounds = msg.getLocalBounds();
    msg.setOrigin(msgBounds.left + msgBounds.width / 2.0f, msgBounds.top + msgBounds.height / 2.0f);
    msg.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 60);

    // Optional white outline for visibility
    msg.setOutlineColor(sf::Color::White);
    msg.setOutlineThickness(2);

    window.draw(msg);
}

void resetBoard()
{
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            board[r][c] = ""; // No number
    currentPlayer = 1;
    win = false;
    gameOver = false;
}

void drawMenu(sf::RenderWindow &window)
{
    float centerX = WINDOW_WIDTH / 2.0f;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos(mousePos.x, mousePos.y);

    // Title (keep blinking effect if desired)
    sf::Text title("TIC TAC TOE", font, 130);
    title.setStyle(sf::Text::Bold);
    float time = blinkClock.getElapsedTime().asSeconds();
    int alpha = static_cast<int>((std::sin(time * 2.0f) + 1) / 2 * 155 + 100);
    title.setFillColor(sf::Color(255, 255, 255, alpha));
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    title.setPosition(centerX, 150);
    if (title.getGlobalBounds().contains(mouseWorldPos))
    {
        title.setFillColor(sf::Color(0, 255, 255, alpha));
        title.setOutlineColor(sf::Color::White);
        title.setOutlineThickness(4);
        title.setScale(1.1f, 1.1f);
    }
    else
    {
        title.setOutlineThickness(0);
        title.setScale(1.0f, 1.0f);
    }

    window.draw(title);

    // Button hover scale
    auto hoverEffect = [&](sf::Sprite &sprite)
    {
        if (sprite.getGlobalBounds().contains(mouseWorldPos))
            sprite.setScale(0.55f, 0.55f); // Slightly bigger on hover
        else
            sprite.setScale(0.5f, 0.5f);
    };

    hoverEffect(playButton);
    hoverEffect(resumeButton);
    hoverEffect(exitButton);

    // Recenter after scale change
    playButton.setPosition(centerX - playButton.getGlobalBounds().width / 2, 350);
    resumeButton.setPosition(centerX - resumeButton.getGlobalBounds().width / 2, 500);
    exitButton.setPosition(centerX - exitButton.getGlobalBounds().width / 2, 650);

    // Draw buttons
    window.draw(playButton);
    window.draw(resumeButton);
    window.draw(exitButton);

    sf::Color basePink(255, 182, 193);  // Soft pastel pink
    sf::Color hoverPink(255, 228, 225); // Light blush on hover

    auto styledText = [&](sf::Text &txt, const string &str, const sf::Sprite &btn)
    {
        txt.setFont(font);
        txt.setString(str);

        if (str == "Resume")
            txt.setCharacterSize(35); // Decreased font size for "Resume"
        else
            txt.setCharacterSize(50); // Default size for "Play" and "Exit"
        txt.setStyle(sf::Text::Bold);

        sf::FloatRect btnBounds = btn.getGlobalBounds();
        sf::FloatRect txtBounds = txt.getLocalBounds();
        txt.setOrigin(txtBounds.left + txtBounds.width / 2, txtBounds.top + txtBounds.height / 2);
        txt.setPosition(btnBounds.left + btnBounds.width / 2, btnBounds.top + btnBounds.height / 2);

        float time = blinkClock.getElapsedTime().asSeconds();
        float glow = std::sin(time * 3.0f) * 0.5f + 0.5f;

        sf::Color basePink(255, 182, 193);
        sf::Color hoverPink(255, 228, 225);

        bool hovering = btn.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)));

        sf::Color dynamicGlow = hoverPink;
        dynamicGlow.a = static_cast<sf::Uint8>(180 + 75 * glow); // Glow alpha

        if (hovering)
        {
            txt.setFillColor(dynamicGlow);
            txt.setOutlineColor(sf::Color::White);
            txt.setOutlineThickness(3);
        }
        else
        {
            txt.setFillColor(basePink);
            txt.setOutlineThickness(0);
        }
    };

    // Create and style each label
    sf::Text playText, resumeText, exitText;
    styledText(playText, "Play", playButton);
    styledText(resumeText, "Resume", resumeButton);
    styledText(exitText, "Exit", exitButton);

    // Draw them
    window.draw(playText);
    window.draw(resumeText);
    window.draw(exitText);

    // Draw text
    /* window.draw(playText);
     window.draw(resumeText);
     window.draw(exitText); */
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tic Tac Toe - Made by Taha Sohail");

    window.setFramerateLimit(60);

    if (!playTexture.loadFromFile("button1.png") || !resumeTexture.loadFromFile("button1.png") || !exitTexture.loadFromFile("button1.png"))
    {
        cerr << "One or more button images failed to load!" << endl;
        return -1;
    }

    playButton.setTexture(playTexture);
    resumeButton.setTexture(resumeTexture);
    exitButton.setTexture(exitTexture);

    // Scale the buttons
    playButton.setScale(0.5f, 0.5f);
    resumeButton.setScale(0.5f, 0.5f);
    exitButton.setScale(0.5f, 0.5f);

    if (!pauseButtonTexture.loadFromFile("button1.png") || !resumeButtonTexture.loadFromFile("button1.png") || !pauseExitButtonTexture.loadFromFile("button1.png"))
    {
        cerr << "Pause button image failed to load!" << endl;
        return -1;
    }
    pauseResumeButton.setTexture(pauseButtonTexture);
    pauseRestartButton.setTexture(resumeButtonTexture);
    pauseExitButton.setTexture(pauseExitButtonTexture);

    // Scale the buttons
    pauseResumeButton.setScale(0.5f, 0.5f);
    pauseRestartButton.setScale(0.5f, 0.5f);
    pauseExitButton.setScale(0.5f, 0.5f);

    const float BUTTON_SPACING = 30.0f;
    float totalWidth = 3 * pauseResumeButton.getGlobalBounds().width + 2 * BUTTON_SPACING;
    float startX = (WINDOW_WIDTH - totalWidth) / 2;
    float buttonY = GRID_TOP + GRID_SIZE + 20; // Position buttons 10 pixels below the grid

    pauseResumeButton.setPosition(startX, buttonY);
    pauseRestartButton.setPosition(startX + pauseResumeButton.getGlobalBounds().width + BUTTON_SPACING, buttonY);
    pauseExitButton.setPosition(startX + 2 * (pauseResumeButton.getGlobalBounds().width + BUTTON_SPACING), buttonY);

    // Positioning
    float centerX = WINDOW_WIDTH / 2.0f;
    playButton.setPosition(centerX - playButton.getGlobalBounds().width / 2, 350);
    resumeButton.setPosition(centerX - resumeButton.getGlobalBounds().width / 2, 500);
    exitButton.setPosition(centerX - exitButton.getGlobalBounds().width / 2, 650);

    if (!font.loadFromFile("arial.ttf"))
    {
        cerr << "Font loading failed! Please ensure arial.ttf is available." << endl;
        return -1;
    }
    if (!backgroundTexture.loadFromFile("background2.png"))
    {
        cerr << "Background image not found! Please place 'background2.png' in the executable folder." << endl;
        return -1;
    }

    // Load the game background
    if (!gameBackgroundTexture.loadFromFile("background1.png"))
    {
        cerr << "Game background image not found!" << endl;
        return -1;
    }

    // Load sound buffers
    if (!clickBuffer.loadFromFile("click.wav") || !victoryBuffer.loadFromFile("victory.wav") || !drawBuffer.loadFromFile("draw.wav") || !transitionBuffer.loadFromFile("transition.wav"))
    {
        cerr << "One or more sound files failed to load!" << endl;
        return -1;
    }
    clickSound.setBuffer(clickBuffer);
    victorySound.setBuffer(victoryBuffer);
    drawSound.setBuffer(drawBuffer);
    transitionSound.setBuffer(transitionBuffer);
    clickSound.setVolume(50);
    victorySound.setVolume(70);
    drawSound.setVolume(70);
    transitionSound.setVolume(50);

    // Load music
    if (!menuMusic.openFromFile("menu_music.ogg") || !gameMusic.openFromFile("game_music.ogg"))
    {
        cerr << "One or more music files failed to load!" << endl;
        return -1;
    }
    menuMusic.setVolume(30);
    menuMusic.setLoop(true);
    gameMusic.setVolume(30);
    gameMusic.setLoop(true);
    menuMusic.play();
    gameBackgroundSprite.setTexture(gameBackgroundTexture);
    backgroundSprite.setTexture(backgroundTexture);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::P)
                {
                    isGamePaused = !isGamePaused;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                int mx = event.mouseButton.x;
                int my = event.mouseButton.y;

                if (inMenu)
                {
                    sf::Vector2f mousePos(mx, my);

                    if (playButton.getGlobalBounds().contains(mousePos))
                    {
                        resetBoard(); // Fresh game
                        Pause = false;
                        transitioning = true;
                        transitionAlpha = 255.0f;
                        inMenu = false;
                        menuMusic.stop(); // Stop menu music
                        gameMusic.play(); // Start game music
                    }
                    else if (resumeButton.getGlobalBounds().contains(mousePos))
                    {
                        Pause = false;
                        transitioning = true;
                        transitionAlpha = 255.0f;
                        inMenu = false;
                    }
                    else if (exitButton.getGlobalBounds().contains(mousePos))
                    {
                        window.close();
                    }
                }
                else
                {
                    if (isGamePaused)
                    {
                        sf::Vector2f mousePos(mx, my);
                        if (pauseResumeButton.getGlobalBounds().contains(mousePos))
                            isGamePaused = false;
                        else if (pauseRestartButton.getGlobalBounds().contains(mousePos))
                            resetBoard();
                        else if (pauseExitButton.getGlobalBounds().contains(mousePos))
                        {
                            inMenu = true;
                            Pause = true;
                            transitioning = true;
                            transitionAlpha = 255.0f;
                        }
                    }
                    else
                    {
                        if (!gameOver)
                            handleClick(mx, my);
                        else
                        {
                            inMenu = true;
                            Pause = true;
                            transitioning = true;
                            transitionAlpha = 255.0f;
                        }
                    }
                }
            }
        }

        window.clear();

        if (inMenu)
            window.draw(backgroundSprite); // menu background
        else
            window.draw(gameBackgroundSprite); // game background

        if (inMenu && !transitioning)
            drawMenu(window);
        else if (!inMenu && !transitioning)
        {
            drawGrid(window);
            drawBoard(window);
            drawScoreboard(window);

            if (isGamePaused)
            {
                // Dim screen
                sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                overlay.setFillColor(sf::Color(0, 0, 0, 180));
                window.draw(overlay);

                // Draw buttons
                window.draw(pauseResumeButton);
                window.draw(pauseRestartButton);
                window.draw(pauseExitButton);

                // ✨ Add labels with neon effect
                sf::Text resumeLabel("Resume", font, 36);
                sf::Text restartLabel("Restart", font, 36);
                sf::Text exitLabel("Exit", font, 36);

                // Neon effect: Set fill color and outline
                resumeLabel.setFillColor(sf::Color::Cyan);
                resumeLabel.setOutlineColor(sf::Color(0, 255, 255, 150)); // Glowing cyan outline
                resumeLabel.setOutlineThickness(2.0f);

                restartLabel.setFillColor(sf::Color::Magenta);
                restartLabel.setOutlineColor(sf::Color(255, 0, 255, 150)); // Glowing magenta outline
                restartLabel.setOutlineThickness(2.0f);

                exitLabel.setFillColor(sf::Color::Yellow);
                exitLabel.setOutlineColor(sf::Color(255, 255, 0, 150)); // Glowing yellow outline
                exitLabel.setOutlineThickness(2.0f);

                // Position resumeLabel on the button
                sf::FloatRect resumeButtonBounds = pauseResumeButton.getGlobalBounds();
                sf::FloatRect resumeLabelBounds = resumeLabel.getLocalBounds();
                resumeLabel.setOrigin(resumeLabelBounds.width / 2, resumeLabelBounds.height / 2); // Center the origin
                resumeLabel.setPosition(
                    resumeButtonBounds.left + resumeButtonBounds.width / 2, // Center horizontally
                    resumeButtonBounds.top + resumeButtonBounds.height / 2  // Center vertically
                );

                // Position restartLabel on the button
                sf::FloatRect restartButtonBounds = pauseRestartButton.getGlobalBounds();
                sf::FloatRect restartLabelBounds = restartLabel.getLocalBounds();
                restartLabel.setOrigin(restartLabelBounds.width / 2, restartLabelBounds.height / 2); // Center the origin
                restartLabel.setPosition(
                    restartButtonBounds.left + restartButtonBounds.width / 2, // Center horizontally
                    restartButtonBounds.top + restartButtonBounds.height / 2  // Center vertically
                );

                // Position exitLabel on the button
                sf::FloatRect exitButtonBounds = pauseExitButton.getGlobalBounds();
                sf::FloatRect exitLabelBounds = exitLabel.getLocalBounds();
                exitLabel.setOrigin(exitLabelBounds.width / 2, exitLabelBounds.height / 2); // Center the origin
                exitLabel.setPosition(
                    exitButtonBounds.left + exitButtonBounds.width / 2, // Center horizontally
                    exitButtonBounds.top + exitButtonBounds.height / 2  // Center vertically
                );

                window.draw(resumeLabel);
                window.draw(restartLabel);
                window.draw(exitLabel);
            }
            else if (gameOver)
            {
                drawGameOver(window);
            }
        }

        // Animate transition
        if (transitioning)
        {
            // Play sound only once when transition starts
            if (!transitionSoundPlayed && !isMuted)
            {
                transitionSound.play();
                transitionSoundPlayed = true;
            }

            transitionAlpha -= 8.0f;
            if (transitionAlpha <= 0)
            {
                transitioning = false;
                transitionAlpha = 0;
                transitionSoundPlayed = false; // reset for next time
            }

            fadeOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(transitionAlpha)));
            window.draw(fadeOverlay);
        }

        window.display();
    }
    return 0;
}