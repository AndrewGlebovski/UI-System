#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "configs.hpp"
#include "vector.hpp"
#include "list.hpp"
#include "style.hpp"
#include "ui-base.hpp"
#include "button.hpp"
#include "scrollbar.hpp"
#include "canvas.hpp"
#include "ui-system.hpp"


int main() {
    sf::RenderWindow render_window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI", sf::Style::Fullscreen);

    // ENABLED VSYNC TO AVOID VISUAL ARTIFACTS WHEN WINDOWS ARE MOVING
    render_window.setVerticalSyncEnabled(true);

    sf::Font font;
    ASSERT(font.loadFromFile(FONT_FILE), "Failed to load font!\n");

    WindowStyle window_style(
        sf::Color(WINDOW_FRAME_COLOR),
        sf::Color(WINDOW_TITLE_COLOR),
        WINDOW_FRAME_OUTLINE,
        WINDOW_TITLE_BAR_HEIGHT,
        WINDOW_TITLE_SIZE,
        font
    );

    ScrollBarStyle scrollbar_style(
        sf::Color::Black,
        -1,
        sf::Color::White,
        sf::Color::Black,
        0.1f
    );

    MainWindow main_window(
        Vector2D(100, 100),
        Vector2D(SCREEN_W - 200, SCREEN_H - 200),
        1,
        "Paint",
        window_style
    );
    
    Window *w1 = new Window(
        Vector2D(100, 100),
        Vector2D(800, 600),
        2,
        nullptr,
        "picture1.png",
        window_style
    );
    
    Canvas *canvas = new Canvas(
        Vector2D(0, 0),
        Vector2D(774, 527),
        5,
        nullptr,
        nullptr
    );

    w1->addElement(canvas);

    w1->addElement(new VScrollBar(
        Vector2D(774, 0),
        Vector2D(20, 527),
        5,
        nullptr,
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    w1->addElement(new HScrollBar(
        Vector2D(0, 527),
        Vector2D(794, 20),
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

    while (render_window.isOpen()) {
        sf::Event event;

        while (render_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                render_window.close();
            
            main_window.parseEvent(event, transforms);
            ASSERT(transforms[0].length() < 0.001, "Transform failed!\n");
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
