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
#include "ui-system.hpp"
#include "canvas.hpp"
#include "clock.hpp"


/// Opens picture on canvas in new subwindow with scrollbars
BaseUI *openPicture(const char *filename, Palette *palette, WindowStyle &window_style, ScrollBarStyle &scrollbar_style);


/// Creates palette view in new subwindow
BaseUI *createPaletteView(Palette *palette, WindowStyle &window_style, PaletteViewAsset &palette_asset);


int main() {
    sf::RenderWindow render_window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI", sf::Style::Fullscreen);

    // ENABLED VSYNC TO AVOID VISUAL ARTIFACTS WHEN WINDOWS ARE MOVING
    render_window.setVerticalSyncEnabled(true);

    sf::Font font;
    ASSERT(font.loadFromFile(FONT_FILE), "Failed to load font!\n");

    WindowAsset window_asset(WINDOW_ASSET_DIR);

    PaletteViewAsset palette_asset(PALETTE_ASSET_DIR);
    
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
        Vector2D(0, 0),
        Vector2D(SCREEN_W, SCREEN_H),
        1,
        "Paint",
        window_style
    );

    main_window.addElement(new Clock(
        Vector2D(0, 0),
        Vector2D(100, 50),
        3,
        nullptr,
        clock_style
    ));

    Palette *palette = new Palette();

    main_window.addElement(openPicture(nullptr, palette, window_style, scrollbar_style));

    main_window.addElement(createPaletteView(palette, window_style, palette_asset));
    
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

    delete palette;
    
    printf("UI System!\n");
    return 0;
}


BaseUI *openPicture(const char *filename, Palette *palette, WindowStyle &window_style, ScrollBarStyle &scrollbar_style) {
    Window *subwindow = new Window(
        Vector2D(300, 100),
        Vector2D(800, 600),
        1,
        nullptr,
        (filename) ? filename : "Canvas",
        window_style
    );

    Canvas *canvas = new Canvas(
        Vector2D(0, 0),
        subwindow->getAreaSize() - Vector2D(30, 30),
        0,
        nullptr,
        filename,
        palette
    );

    subwindow->addElement(canvas);

    subwindow->addElement(new VScrollBar(
        Vector2D(subwindow->getAreaSize().x - 20, 0),
        Vector2D(20, subwindow->getAreaSize().y - 30),
        1,
        nullptr,
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    subwindow->addElement(new HScrollBar(
        Vector2D(0, subwindow->getAreaSize().y - 20),
        Vector2D(subwindow->getAreaSize().x, 20),
        1,
        nullptr,
        new HScrollCanvas(*canvas),
        scrollbar_style
    ));

    return subwindow;
}


BaseUI *createPaletteView(Palette *palette, WindowStyle &window_style, PaletteViewAsset &palette_asset) {
    WindowStyle subwindow_style(window_style);
    subwindow_style.outline = 0;


    Window *subwindow = new Window(
        Vector2D(0, 100),
        Vector2D(218, 451),
        2,
        nullptr,
        "Palette",
        subwindow_style
    );


    subwindow->addElement(new PaletteView(
        Vector2D(0, 0),
        Vector2D(188, 376),
        1,
        nullptr,
        palette,
        palette_asset
    ));

    return subwindow;
}
