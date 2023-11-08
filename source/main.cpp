#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
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
#include "dialog.hpp"
#include "canvas_dialogs.hpp"


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

    LineEditStyle line_edit_style(
        font,
        20,
        sf::Color::Black,
        sf::Color::White,
        sf::Color::Black,
        sf::Color::Black,
        1
    );

    ButtonStyle button_style(
        font,
        20,
        sf::Color::Black,
        sf::Color::White,
        sf::Color::White
    );

    ClockStyle clock_style(
        sf::Color::Black,
        20,
        font
    );

    MainWindow *main_window = new MainWindow(
        Widget::AUTO_ID,
        Transform(),
        Vec2d(SCREEN_W, SCREEN_H),
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
    
    main_menu->addMenuButton("File");
    main_menu->addButton(0, "Open", new CreateOpenFileDialog(
        *main_window,
        *palette,
        *canvas_group,
        window_style,
        scrollbar_style,
        line_edit_style,
        button_style
    ));

    main_menu->addButton(0, "Save", new SaveFileAction(
        *canvas_group
    ));
    
    main_menu->addButton(0, "Save As", new CreateSaveAsFileDialog(
        *main_window,
        *canvas_group,
        window_style,
        line_edit_style,
        button_style
    ));
    
    main_menu->addMenuButton("Filter");
    main_menu->addButton(1, "Lighten", new FilterAction(FilterPalette::LIGHTEN_FILTER, *filter_palette, *canvas_group));
    main_menu->addButton(1, "Darken", new FilterAction(FilterPalette::DARKEN_FILTER, *filter_palette, *canvas_group));
    main_menu->addButton(1, "Monochrome", new FilterAction(FilterPalette::MONOCHROME_FILTER, *filter_palette, *canvas_group));
    main_menu->addButton(1, "Negative", new FilterAction(FilterPalette::NEGATIVE_FILTER, *filter_palette, *canvas_group));

    main_window->setMenu(main_menu);

    main_window->addChild(new Clock(
        Widget::AUTO_ID,
        Transform(),
        Vec2d(100, 50),
        3,
        nullptr,
        clock_style
    ));

    main_window->addChild(new FilterHotkey(
        main_window,
        *filter_palette,
        *canvas_group
    ));

    main_window->addChild(openPicture(nullptr, *palette, *canvas_group, window_style, scrollbar_style));
    main_window->addChild(createToolPaletteView(palette, window_style, palette_asset));

    List<Transform> transforms;
    transforms.push_back(Transform());

    sf::Clock timer;

    while (render_window.isOpen()) {
        if (main_window->getStatus() == Widget::DELETE) {
            render_window.close();
            break;
        }

        main_window->checkChildren();

        sf::Event event;

        while (render_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                render_window.close();
                break;
            }
            
            main_window->parseEvent(event, transforms);
        }
        
        main_window->onTimer(timer.getElapsedTime().asSeconds());
        timer.restart();

        render_window.clear();

        main_window->draw(render_window, transforms);

        render_window.display();
    }

    delete main_window;
    delete palette;
    delete canvas_group;
    delete filter_palette;
    
    printf("UI System!\n");
    return 0;
}


Widget *createToolPaletteView(ToolPalette *palette, WindowStyle &window_style, PaletteViewAsset &palette_asset) {
    WindowStyle subwindow_style(window_style);
    subwindow_style.outline = 0;

    Window *subwindow = new Window(
        Widget::AUTO_ID,
        Transform(Vec2d(0, 100)),
        Vec2d(218, 451),
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
        Vec2d(188, 376),
        1,
        nullptr,
        palette,
        palette_asset
    ));

    return subwindow;
}
