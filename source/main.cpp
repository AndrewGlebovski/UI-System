#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <time.h>
#include "vector.hpp"
#include "list.hpp"
#include "configs.hpp"
#include "asset.hpp"
#include "style.hpp"
#include "ui-base.hpp"
#include "button.hpp"
#include "scrollbar.hpp"
#include "canvas.hpp"
#include "ui-system.hpp"
#include "clock.hpp"


int main() {
    sf::RenderWindow render_window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI", sf::Style::Fullscreen);

    // ENABLED VSYNC TO AVOID VISUAL ARTIFACTS WHEN WINDOWS ARE MOVING
    render_window.setVerticalSyncEnabled(true);

    sf::Font font;
    ASSERT(font.loadFromFile(FONT_FILE), "Failed to load font!\n");

    WindowAsset window_asset(WINDOW_ASSET_DIR);

    WindowStyle window_style(
        sf::Color(WINDOW_TITLE_COLOR),
        WINDOW_TITLE_OFFSET,
        WINDOW_FONT_SIZE,
        font,
        window_asset,
        WINDOW_OUTLINE,
        WINDOW_TL_OFFSET,
        WINDOW_BR_OFFSET
    );

    ScrollBarStyle scrollbar_style(
        sf::Color(SCROLLBAR_FRAME_COLOR),
        SCROLLBAR_FRAME_OUTLINE,
        sf::Color(SCROLLBAR_BACKGROUND_COLOR),
        sf::Color(SCROLLBAR_SCROLLER_COLOR),
        SCROLLBAR_SCROLLER_FACTOR
    );

    ClockStyle clock_style(
        sf::Color::Black,
        20,
        font
    );

    MainWindow main_window(
        Vector2D(100, 100),
        Vector2D(SCREEN_W - 200, SCREEN_H - 200),
        1,
        "Paint",
        window_style
    );

    main_window.addElement(new Clock(
        Vector2D(0, 0),
        Vector2D(100, 50),
        5,
        nullptr,
        clock_style
    ));

    Window *w1 = new Window(
        Vector2D(200, 200),
        Vector2D(800, 600),
        2,
        nullptr,
        "picture1.png",
        window_style
    );

    Canvas *canvas = new Canvas(
        Vector2D(0, 0),
        w1->getAreaSize() - Vector2D(30, 30),
        5,
        nullptr,
        nullptr
    );

    w1->addElement(canvas);

    w1->addElement(new VScrollBar(
        Vector2D(w1->getAreaSize().x - 20, 0),
        Vector2D(20, w1->getAreaSize().y - 30),
        5,
        nullptr,
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    w1->addElement(new HScrollBar(
        Vector2D(0, w1->getAreaSize().y - 20),
        Vector2D(w1->getAreaSize().x, 20),
        5,
        nullptr,
        new HScrollCanvas(*canvas),
        scrollbar_style
    ));

    main_window.addElement(w1);
    
    sf::RenderTexture result;
    result.create(SCREEN_W, SCREEN_H);

    List<Vector2D> transforms;
    transforms.push_back(Vector2D());

    sf::Clock timer;

    while (render_window.isOpen()) {
        sf::Event event;

        while (render_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                render_window.close();
            
            main_window.parseEvent(event, transforms);
            ASSERT(transforms[0].length() < 0.001, "Transform failed!\n");
        }
        
        if (timer.getElapsedTime().asSeconds() > 1) {
            main_window.onTimer(timer.getElapsedTime().asSeconds());
            timer.restart();
        }

        result.clear();

        main_window.draw(result, transforms);
        ASSERT(transforms[0].length() < 0.001, "Transform failed!\n");

        result.display();
        sf::Sprite tool_sprite(result.getTexture());

        render_window.draw(tool_sprite);

        render_window.display();
    }

    printf("UI System!\n");
    return 0;
}
