#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "configs.hpp"
#include "vector.hpp"
#include "list.hpp"
#include "action.hpp"
#include "style.hpp"
#include "ui-base.hpp"
#include "ui-system.hpp"
#include "canvas.hpp"



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
    
    w1->addElement(new Canvas(
        Vector2D(0, 0),
        Vector2D(794, 550),
        5,
        nullptr,
        nullptr
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
