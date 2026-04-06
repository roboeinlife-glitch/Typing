#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

float random(float min, float max) {
    return min + static_cast<float>(std::rand()) / RAND_MAX * (max - min);
}

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float life;
    float maxLife;
    int type;
    char character;
    float size;
};

struct TextBuffer {
    std::vector<std::string> lines;
    int maxWidthPx;
    int maxLines;

    TextBuffer(int w, int h) : maxWidthPx(w), maxLines(h) {}

    void addChar(char c, sf::Font& font, int charSize) {
        if (lines.empty()) lines.push_back("");

        std::string& current = lines.back();
        sf::Text test(current + c, font, charSize);

        if (test.getLocalBounds().width > maxWidthPx && !current.empty()) {
            lines.push_back(std::string(1, c));
        } else {
            current += c;
        }

        while ((int)lines.size() > maxLines) {
            lines.erase(lines.begin());
        }
    }

    void removeLastChar() {
        if (lines.empty()) return;
        std::string& last = lines.back();
        if (!last.empty()) {
            last.pop_back();
        }
        if (last.empty() && lines.size() > 1) {
            lines.pop_back();
        }
    }

    void clear() {
        lines.clear();
    }

    std::string getString() const {
        std::string result;
        for (const auto& line : lines) {
            result += line + "\n";
        }
        return result;
    }

    int getLineCount() const { return lines.size(); }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(800, 600), "🎮 NGUOI QUE GO PHIM - PHIEN BAN HOAN CHINH 🎮");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // ===== BỐ CỤC (xịch xuống để lộ người que) =====
    const float screenX = 260, screenY = 60;
    const float screenW = 280, screenH = 170;
    const float keyboardX = 260, keyboardY = 280;
    const float keyboardW = 280, keyboardH = 70;
    const float bodyX = 400, bodyY = 440; // Tâm thân người (xịch xuống)

    // Text buffer
    TextBuffer textBuffer(260, 7);
    sf::Text displayText("", font, 18);
    displayText.setFillColor(sf::Color(0, 255, 100));
    displayText.setPosition(screenX + 10, screenY + 10);

    sf::Text cursor("_", font, 18);
    cursor.setFillColor(sf::Color(0, 255, 100));
    sf::Clock cursorClock;
    bool showCursor = true;

    // Màn hình CRT
    sf::RectangleShape monitorFrame(sf::Vector2f(screenW + 20, screenH + 20));
    monitorFrame.setFillColor(sf::Color(30, 30, 40));
    monitorFrame.setOutlineColor(sf::Color(0, 255, 0));
    monitorFrame.setOutlineThickness(3);
    monitorFrame.setPosition(screenX - 10, screenY - 10);

    sf::RectangleShape screen(sf::Vector2f(screenW, screenH));
    screen.setFillColor(sf::Color(0, 0, 0));
    screen.setOutlineColor(sf::Color(0, 200, 0));
    screen.setOutlineThickness(2);
    screen.setPosition(screenX, screenY);

    sf::Text logo(">_ TYPING MASTER _<", font, 11);
    logo.setFillColor(sf::Color(0, 200, 0));
    logo.setPosition(screenX + screenW/2 - 70, screenY - 18);

    // Bàn phím
    sf::RectangleShape keyboard(sf::Vector2f(keyboardW, keyboardH));
    keyboard.setFillColor(sf::Color(40, 40, 50));
    keyboard.setOutlineColor(sf::Color(0, 255, 0));
    keyboard.setOutlineThickness(2);
    keyboard.setPosition(keyboardX, keyboardY);

    // Các phím ảo
    std::vector<sf::RectangleShape> keys;
    std::vector<sf::Text> keyLabels;
    std::string keyOrder = "QWERTYUIOPASDFGHJKLZXCVBNM";
    for (int i = 0; i < 26; i++) {
        sf::RectangleShape key(sf::Vector2f(26, 26));
        key.setFillColor(sf::Color(50, 50, 70));
        key.setOutlineColor(sf::Color(0, 200, 0));
        key.setOutlineThickness(1);
        float x = keyboardX + 8 + (i % 10) * 28;
        float y = keyboardY + 8 + (i / 10) * 28;
        key.setPosition(x, y);
        keys.push_back(key);

        sf::Text label(std::string(1, keyOrder[i]), font, 11);
        label.setFillColor(sf::Color(0, 255, 0));
        label.setPosition(x + 7, y + 5);
        keyLabels.push_back(label);
    }

    // ===== NGƯỜI QUE (góc nhìn từ sau, bỏ mắt miệng) =====
    // Vùng hoạt động của tay trên bàn phím
    const float leftHandZone[] = {keyboardX + 30, keyboardX + 100, keyboardY + 10, keyboardY + 40};
    const float rightHandZone[] = {keyboardX + keyboardW - 110, keyboardX + keyboardW - 30, keyboardY + 10, keyboardY + 40};

    float leftHandX = leftHandZone[0], leftHandY = leftHandZone[2];
    float rightHandX = rightHandZone[0], rightHandY = rightHandZone[2];
    float armAngle = 0;
    float bodyShake = 0;
    sf::Clock armClock;
    sf::Clock bodyShakeClock;
    sf::Clock eyeClock;

    // ===== HIỆU ỨNG =====
    std::vector<Particle> particles;
    sf::Clock clock, typeClock;
    int typeCount = 0;
    bool bugMode = false;
    bool isShaking = false;
    float shakeDuration = 0;
    sf::Clock keyGlowClock;
    bool keyGlow = false;

    std::string correctWord = "matrix";
    int correctIndex = 0;

    auto addParticle = [&](float x, float y, int type, char ch = ' ') {
        Particle p;
        p.pos = sf::Vector2f(x, y);
        p.life = p.maxLife = random(0.5f, 1.0f);
        p.type = type;
        p.character = ch;
        p.size = random(3, 10);

        if (type == 0) {
            p.vel = sf::Vector2f(random(-60, 60), random(-120, -40));
            p.size = random(6, 14);
        } else if (type == 1) {
            p.vel = sf::Vector2f(random(-100, 100), random(-180, -60));
        } else if (type == 2) {
            float angle = random(0, 360) * 3.14159f / 180.f;
            float speed = random(200, 500);
            p.vel = sf::Vector2f(cos(angle) * speed, sin(angle) * speed);
            p.size = random(5, 15);
            char bugChars[] = {'!', '@', '#', '$', '%', '{', '}', ';', '?'};
            p.character = bugChars[std::rand() % 9];
        } else {
            p.vel = sf::Vector2f(random(-200, 200), random(-250, -80));
            p.size = random(4, 8);
        }
        particles.push_back(p);
    };

    auto spawnSmoke = [&](float x, float y) {
        for (int i = 0; i < 10; i++) addParticle(x + random(-40, 40), y + random(-10, 20), 0);
    };

    auto spawnTears = [&](float x, float y) {
        for (int i = 0; i < 12; i++) addParticle(x + random(-20, 20), y + random(-10, 15), 1);
    };

    auto spawnBigBang = [&](float x, float y) {
        isShaking = true;
        shakeDuration = 0.2f;
        for (int i = 0; i < 90; i++) addParticle(x + random(-40, 40), y + random(-40, 40), 2);
    };

    auto shakeBody = [&]() {
        bodyShake = 5;
        bodyShakeClock.restart();
    };

    // Hàm di chuyển tay đến vị trí ngẫu nhiên trong vùng
    auto moveHands = [&]() {
        leftHandX = random(leftHandZone[0], leftHandZone[1]);
        leftHandY = random(leftHandZone[2], leftHandZone[3]);
        rightHandX = random(rightHandZone[0], rightHandZone[1]);
        rightHandY = random(rightHandZone[2], rightHandZone[3]);
        armAngle = 25;
        armClock.restart();
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::BackSpace) {
                    textBuffer.removeLastChar();
                    displayText.setString(textBuffer.getString());
                    shakeBody();
                    moveHands();
                }
                if (event.key.code == sf::Keyboard::Delete) {
                    textBuffer.clear();
                    displayText.setString("");
                    shakeBody();
                    moveHands();
                }
                if (event.key.code == sf::Keyboard::B) {
                    bugMode = !bugMode;
                    if (bugMode) spawnBigBang(screenX + screenW/2, screenY + screenH/2);
                    shakeBody();
                    moveHands();
                }
                if (event.key.code == sf::Keyboard::Space && sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                    isShaking = true;
                    shakeDuration = 0.25f;
                    for (int i = 0; i < 40; i++) addParticle(keyboardX + random(0, keyboardW), keyboardY + random(0, keyboardH), 3);
                    std::string current = textBuffer.getString();
                    if (current.length() > 5) {
                        textBuffer.clear();
                        for (size_t i = 0; i < current.length() / 2; i++) {
                            textBuffer.addChar(current[i], font, 18);
                        }
                    } else {
                        textBuffer.clear();
                    }
                    displayText.setString(textBuffer.getString());
                    for (int i = 0; i < 15; i++) spawnSmoke(keyboardX + keyboardW/2, keyboardY);
                    shakeBody();
                    moveHands();
                }
            }

            if (event.type == sf::Event::TextEntered && event.text.unicode < 128 && event.text.unicode != 8) {
                char c = static_cast<char>(event.text.unicode);
                if (c >= 32 && c <= 126) {
                    // Di chuyển tay đến vị trí mới mỗi lần gõ
                    moveHands();
                    keyGlow = true;
                    keyGlowClock.restart();

                    float dtType = typeClock.restart().asSeconds();
                    if (dtType < 0.18f) {
                        typeCount++;
                        if (typeCount >= 3) {
                            spawnSmoke(keyboardX + keyboardW/2, keyboardY - 15);
                            typeCount = 0;
                        }
                    } else {
                        typeCount = 1;
                    }

                    if (bugMode) {
                        textBuffer.addChar('?', font, 18);
                        spawnBigBang(screenX + screenW/2, screenY + screenH/2);
                    } else {
                        textBuffer.addChar(c, font, 18);
                        if (correctIndex < correctWord.length() && tolower(c) == correctWord[correctIndex]) {
                            correctIndex++;
                            if (correctIndex == correctWord.length()) {
                                for (int i = 0; i < 20; i++) addParticle(bodyX, bodyY - 40, 0);
                                correctIndex = 0;
                            }
                        } else if (correctIndex > 0) {
                            spawnTears(bodyX, bodyY - 40);
                            correctIndex = 0;
                        }
                    }
                    displayText.setString(textBuffer.getString());
                    shakeBody();
                }
            }
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.033f) dt = 0.033f;

        // Rung màn hình
        sf::View view = window.getDefaultView();
        if (isShaking) {
            shakeDuration -= dt;
            if (shakeDuration <= 0) {
                isShaking = false;
                view.setCenter(400, 300);
            } else {
                view.setCenter(400 + random(-8, 8), 300 + random(-8, 8));
            }
            window.setView(view);
        } else {
            window.setView(view);
        }

        // Cập nhật particle
        for (size_t i = 0; i < particles.size(); ) {
            Particle& p = particles[i];
            p.pos += p.vel * dt;
            if (p.type == 1) p.vel.y += 500 * dt;
            else if (p.type == 2) { p.vel.y += 150 * dt; p.vel *= 0.97f; }
            else if (p.type == 0) { p.vel.y += 40 * dt; p.vel *= 0.95f; }
            else { p.vel.y += 700 * dt; p.vel.x *= 0.98f; }

            p.life -= dt;
            if (p.life <= 0) particles.erase(particles.begin() + i);
            else ++i;
        }

        // Cập nhật animation
        if (armClock.getElapsedTime().asSeconds() > 0.12f) armAngle = 0;
        if (bodyShakeClock.getElapsedTime().asSeconds() > 0.08f) bodyShake = 0;

        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) { showCursor = !showCursor; cursorClock.restart(); }
        if (keyGlowClock.getElapsedTime().asSeconds() > 0.1f) keyGlow = false;

        // ==================== VẼ ====================
        window.clear(sf::Color(0, 0, 0));

        // Nền Matrix
        for (int i = 0; i < 20; i++) {
            sf::Text rain("01", font, 10);
            rain.setFillColor(sf::Color(0, 50, 0, 30));
            rain.setPosition(i * 40, (std::rand() % 600));
            window.draw(rain);
        }

        // Màn hình
        window.draw(monitorFrame);
        window.draw(screen);
        window.draw(logo);
        window.draw(displayText);

        // Con trỏ
        if (showCursor && !bugMode && textBuffer.getLineCount() < 7) {
            int lineCount = textBuffer.getLineCount();
            float lastX = screenX + 10 + displayText.getLocalBounds().width;
            float lastY = screenY + 10 + std::max(0, lineCount - 1) * 22;
            if (lastY < screenY + screenH - 20) {
                cursor.setPosition(lastX, lastY);
                window.draw(cursor);
            }
        }

        // Bàn phím
        window.draw(keyboard);
        for (size_t i = 0; i < keys.size(); i++) {
            window.draw(keys[i]);
            window.draw(keyLabels[i]);
        }

        if (keyGlow) {
            sf::RectangleShape glow(sf::Vector2f(keyboardW, keyboardH));
            glow.setFillColor(sf::Color(0, 255, 0, 70));
            glow.setPosition(keyboardX, keyboardY);
            window.draw(glow);
        }

        // ===== NGƯỜI QUE (góc nhìn từ sau, bỏ mắt miệng) =====
        float currentBodyX = bodyX + (bodyShake * random(-1, 1));
        float currentBodyY = bodyY + (bodyShake * random(-0.5f, 0.5f));

        // Đầu (hình tròn đơn giản, không mắt miệng)
        sf::CircleShape head(24);
        head.setPosition(currentBodyX - 24, currentBodyY - 95);
        head.setFillColor(sf::Color::Transparent);
        head.setOutlineColor(sf::Color(0, 255, 0));
        head.setOutlineThickness(3);
        window.draw(head);

        // Thân
        sf::Vertex body[] = {
            sf::Vertex(sf::Vector2f(currentBodyX, currentBodyY - 46), sf::Color(0, 255, 0)),
            sf::Vertex(sf::Vector2f(currentBodyX, currentBodyY), sf::Color(0, 255, 0))
        };
        window.draw(body, 2, sf::Lines);

        // TAY TRÁI (đập vào vùng ASDF)
        float currentLeftX = leftHandX - (armAngle * 0.5f);
        float currentLeftY = leftHandY - (armAngle * 0.3f);
        sf::Vertex leftArm[] = {
            sf::Vertex(sf::Vector2f(currentBodyX - 8, currentBodyY - 45), sf::Color(0, 255, 0)),
            sf::Vertex(sf::Vector2f(currentLeftX, currentLeftY), sf::Color(0, 255, 0))
        };
        window.draw(leftArm, 2, sf::Lines);

        // TAY PHẢI (đập vào vùng JKL;)
        float currentRightX = rightHandX - (armAngle * 0.5f);
        float currentRightY = rightHandY - (armAngle * 0.3f);
        sf::Vertex rightArm[] = {
            sf::Vertex(sf::Vector2f(currentBodyX + 8, currentBodyY - 45), sf::Color(0, 255, 0)),
            sf::Vertex(sf::Vector2f(currentRightX, currentRightY), sf::Color(0, 255, 0))
        };
        window.draw(rightArm, 2, sf::Lines);

        // Chân
        sf::Vertex leftLeg[] = {
            sf::Vertex(sf::Vector2f(currentBodyX - 7, currentBodyY), sf::Color(0, 255, 0)),
            sf::Vertex(sf::Vector2f(currentBodyX - 20, currentBodyY + 50), sf::Color(0, 255, 0))
        };
        sf::Vertex rightLeg[] = {
            sf::Vertex(sf::Vector2f(currentBodyX + 7, currentBodyY), sf::Color(0, 255, 0)),
            sf::Vertex(sf::Vector2f(currentBodyX + 20, currentBodyY + 50), sf::Color(0, 255, 0))
        };
        window.draw(leftLeg, 2, sf::Lines);
        window.draw(rightLeg, 2, sf::Lines);

        // Vẽ particle
        for (const auto& p : particles) {
            if (p.type == 0) {
                sf::CircleShape smoke(p.size);
                smoke.setFillColor(sf::Color(100, 100, 100, 150 * p.life / p.maxLife));
                smoke.setPosition(p.pos);
                window.draw(smoke);
            } else if (p.type == 1) {
                sf::CircleShape tear(p.size);
                tear.setFillColor(sf::Color(80, 200, 255, 200 * p.life / p.maxLife));
                tear.setPosition(p.pos);
                window.draw(tear);
            } else if (p.type == 2) {
                sf::Text bang(std::string(1, p.character), font, p.size);
                bang.setFillColor(sf::Color(255, 0, 0, 220 * p.life / p.maxLife));
                bang.setPosition(p.pos);
                window.draw(bang);
            } else {
                sf::RectangleShape frag(sf::Vector2f(4, 4));
                frag.setFillColor(sf::Color(150, 150, 150, 180 * p.life / p.maxLife));
                frag.setPosition(p.pos);
                window.draw(frag);
            }
        }

        // Trạng thái
        sf::Text status;
        status.setFont(font);
        status.setCharacterSize(14);
        status.setPosition(10, 10);
        if (bugMode) {
            status.setString("⚠️ BUG MODE ON ⚠️");
            status.setFillColor(sf::Color(255, 0, 0));
        } else {
            status.setString("✓ NORMAL | B:BUG | C+SPACE:SMASH");
            status.setFillColor(sf::Color(0, 255, 0));
        }
        window.draw(status);

        sf::Text guide;
        guide.setFont(font);
        guide.setCharacterSize(11);
        guide.setFillColor(sf::Color(0, 180, 0));
        guide.setString("TYPE | BACKSPACE | DELETE | B | C+SPACE");
        guide.setPosition(10, 575);
        window.draw(guide);

        if (typeCount >= 2 && !bugMode) {
            sf::Text fast("⚡ FAST TYPING! ⚡", font, 14);
            fast.setFillColor(sf::Color(255, 200, 0));
            fast.setPosition(bodyX - 60, bodyY - 130);
            window.draw(fast);
        }

        window.display();
    }

    return 0;
}
