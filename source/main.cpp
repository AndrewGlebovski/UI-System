#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <time.h>
#include "vector.hpp"
#include "list.hpp"
#include "configs.hpp"
#include "asset.hpp"
#include "widget.hpp"
#include "container.hpp"
#include "button.hpp"
#include "line-edit.hpp"
#include "scrollbar.hpp"
#include "menu.hpp"
#include "window.hpp"
#include "canvas.hpp"
#include "clock.hpp"


/// Opens picture on canvas in new subwindow with scrollbars
Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style);


/// Creates palette view in new subwindow
Widget *createToolPaletteView(ToolPalette *palette, WindowStyle &window_style, PaletteViewAsset &palette_asset);


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

    MenuStyle menu_style(
        font,
        25,
        sf::Color::Black,
        sf::Color::White,
        sf::Color::White,
        sf::Color(0xd4d0c8ff),
        sf::Color(0x000080ff),
        sf::Color(0x000080ff)
    );

    ClockStyle clock_style(
        sf::Color::Black,
        20,
        font
    );

    MainWindow main_window(
        Widget::AUTO_ID,
        Transform(),
        Vector2D(SCREEN_W, SCREEN_H),
        1,
        "Paint",
        window_style
    );

    Menu *main_menu = new Menu(
        Widget::AUTO_ID,
        Transform(),
        1,
        nullptr,
        menu_style
    );

    ToolPalette *palette = new ToolPalette();
    CanvasGroup *canvas_group = new CanvasGroup();
    FilterPalette *filter_palette = new FilterPalette();
    
    main_menu->addMenuButton("Filter");
    main_menu->addButton(0, "Lighten", new FilterAction(FilterPalette::LIGHTEN_FILTER, *filter_palette, *canvas_group));
    main_menu->addButton(0, "Darken", new FilterAction(FilterPalette::DARKEN_FILTER, *filter_palette, *canvas_group));

    main_window.setMenu(main_menu);

    main_window.addChild(new Clock(
        Widget::AUTO_ID,
        Transform(),
        Vector2D(100, 50),
        3,
        nullptr,
        clock_style
    ));

    main_window.addChild(new FilterHotkey(
        &main_window,
        *filter_palette,
        *canvas_group
    ));

    main_window.addChild(openPicture(nullptr, *palette, *canvas_group, window_style, scrollbar_style));
    main_window.addChild(createToolPaletteView(palette, window_style, palette_asset));
    
    sf::RenderTexture result;
    result.create(SCREEN_W, SCREEN_H);

    List<Transform> transforms;
    transforms.push_back(Transform());

    sf::Clock timer;

    while (render_window.isOpen()) {
        if (main_window.getStatus() == Widget::DELETE) {
            render_window.close();
            break;
        }

        main_window.checkChildren();

        sf::Event event;

        while (render_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                render_window.close();
                break;
            }
            
            main_window.parseEvent(event, transforms);
        }
        
        main_window.onTimer(timer.getElapsedTime().asSeconds());
        timer.restart();

        result.clear();

        main_window.draw(result, transforms);

        result.display();
        sf::Sprite tool_sprite(result.getTexture());

        render_window.draw(tool_sprite);

        render_window.display();
    }

    delete palette;
    
    printf("UI System!\n");
    return 0;
}


Widget *openPicture(const char *filename, ToolPalette &palette, CanvasGroup &group, WindowStyle &window_style, ScrollBarStyle &scrollbar_style) {
    Window *subwindow = new Window(
        Widget::AUTO_ID,
        Transform(Vector2D(300, 100)),
        Vector2D(800, 600),
        1,
        nullptr,
        (filename) ? filename : "Canvas",
        window_style
    );

    Canvas *canvas = new Canvas(
        Widget::AUTO_ID,
        Transform(),
        subwindow->getAreaSize() - Vector2D(30, 30),
        0,
        nullptr,
        palette,
        group
    );

    if (filename) canvas->openImage(filename);
    else canvas->createImage(1920, 1080);

    subwindow->addChild(canvas);

    subwindow->addChild(new VScrollBar(
        Widget::AUTO_ID,
        Transform(Vector2D(subwindow->getAreaSize().x - 20, 0)),
        Vector2D(20, subwindow->getAreaSize().y - 30),
        1,
        nullptr,
        new VScrollCanvas(*canvas),
        scrollbar_style
    ));

    subwindow->addChild(new HScrollBar(
        Widget::AUTO_ID,
        Transform(Vector2D(0, subwindow->getAreaSize().y - 20)),
        Vector2D(subwindow->getAreaSize().x, 20),
        1,
        nullptr,
        new HScrollCanvas(*canvas),
        scrollbar_style
    ));

    return subwindow;
}


Widget *createToolPaletteView(ToolPalette *palette, WindowStyle &window_style, PaletteViewAsset &palette_asset) {
    WindowStyle subwindow_style(window_style);
    subwindow_style.outline = 0;

    Window *subwindow = new Window(
        Widget::AUTO_ID,
        Transform(Vector2D(0, 100)),
        Vector2D(218, 451),
        2,
        nullptr,
        "Tools",
        subwindow_style,
        false,
        true,
        false
    );

    subwindow->addChild(new ToolPaletteView(
        Widget::AUTO_ID,
        Transform(),
        Vector2D(188, 376),
        1,
        nullptr,
        palette,
        palette_asset
    ));

    return subwindow;
}
