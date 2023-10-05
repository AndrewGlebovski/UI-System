#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "configs.hpp"
#include "vector.hpp"
#include "list.hpp"
#include "action.hpp"
#include "style.hpp"
#include "ui-system.hpp"



int main() {
    sf::RenderWindow render_window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI", sf::Style::Fullscreen);

    sf::Font font;
    ASSERT(font.loadFromFile(FONT_FILE), "Failed to load font!\n");

    sf::Texture normal_texture;
    ASSERT(normal_texture.loadFromFile("assets/textures/normal_button.png"), "Failed to load button normal texture!\n");

    sf::Texture hover_texture;
    ASSERT(hover_texture.loadFromFile("assets/textures/hover_button.png"), "Failed to load button hover texture!\n");

    WindowStyle window_style(
        sf::Color(WINDOW_FRAME_COLOR),
        sf::Color(WINDOW_TITLE_COLOR),
        WINDOW_FRAME_OUTLINE,
        WINDOW_TITLE_BAR_HEIGHT,
        WINDOW_TITLE_SIZE,
        font
    );

    ButtonStyle button_style(
        sf::Color(WINDOW_FRAME_COLOR),
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
    
    main_window.addElement(new Window(
        Vector2D(100, 100),
        Vector2D(400, 400),
        2,
        main_window,
        "picture1.png",
        window_style
    ));

    main_window.addElement(new Window(
        Vector2D(400, 200),
        Vector2D(400, 400),
        3,
        main_window,
        "picture2.png",
        window_style
    ));
    
    sf::RenderTexture result;
    result.create(SCREEN_W, SCREEN_H);

    List<Vector2D> transforms;
    transforms.push_back(Vector2D());

    while (render_window.isOpen()) {
        sf::Event event;

        while (render_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                render_window.close();
            
            parseEvent(event, main_window, transforms);
        }
        
        result.clear();

        main_window.draw(result, transforms);
        result.display();
        sf::Sprite tool_sprite(result.getTexture());

        render_window.draw(tool_sprite);

        render_window.display();
    }

    printf("UI System!\n");
    return 0;
}
