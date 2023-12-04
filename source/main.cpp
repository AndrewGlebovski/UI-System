#include "basic/clock.hpp"
#include "canvas/canvas_stuff.hpp"
#include "canvas/plugin_loader.hpp"
#include "canvas/palettes/palette_manager.hpp"


/// Creates tool palette view in new subwindow
Widget *createToolPaletteView(
    WindowStyle &window_style,
    PaletteViewAsset &palette_asset
);


/// Creates palettes for palette manager
void setupPaletteManager();


/// Creates menu for main window
Menu *createMainMenu(
    Window &dialog_parent,
    FileDialogStyle &dialog_style,
    ScrollBarStyle &scrollbar_style
);


int main() {
    sf::RenderWindow render_window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI", sf::Style::Fullscreen);

    // ENABLED VSYNC TO AVOID VISUAL ARTIFACTS WHEN WINDOWS ARE MOVING
    render_window.setVerticalSyncEnabled(true);

    // Init palettes
    setupPaletteManager();

    sf::Font font;
    ASSERT(font.loadFromFile(FONT_FILE), "Failed to load font!\n");
    
    WindowAsset window_asset(WINDOW_ASSET_DIR);

    PaletteViewAsset palette_asset(PALETTE_ASSET_DIR);
    
    WindowStyle window_style(
        Color(WINDOW_TITLE_COLOR),
        WINDOW_TITLE_OFFSET,
        WINDOW_FONT_SIZE,
        font,
        window_asset,
        WINDOW_OUTLINE,
        WINDOW_TL_OFFSET,
        WINDOW_BR_OFFSET
    );
    
    ScrollBarStyle scrollbar_style(
        Color(SCROLLBAR_FRAME_COLOR),
        SCROLLBAR_FRAME_OUTLINE,
        Color(SCROLLBAR_BACKGROUND_COLOR),
        Color(SCROLLBAR_SCROLLER_COLOR),
        SCROLLBAR_SCROLLER_FACTOR
    );

    ClockStyle clock_style(
        Black,
        20,
        font
    );
    
    MainWindow *main_window = new MainWindow(
        Widget::AUTO_ID,
        BoundLayoutBox(Vec2d(), Vec2d(SCREEN_W, SCREEN_H)),
        "Paint",
        window_style
    );
    ASSERT(main_window, "Failed to allocate tool main window!\n");

    FileDialogStyle dialog_style(window_style);

    main_window->setMenu(createMainMenu(*main_window, dialog_style, scrollbar_style));

    main_window->addChild(new Clock(
        Widget::AUTO_ID,
        BoundLayoutBox(Vec2d(), Vec2d(100, 50)),
        clock_style
    ));
    
    main_window->addChild(new FilterHotkey());

    main_window->addChild(openPicture(nullptr, window_style, scrollbar_style));
    main_window->addChild(createToolPaletteView(window_style, palette_asset));
    
    PluginLoader plugin_loader(PLUGIN_DIR, *main_window->getMenu(), 1);

    TransformStack stack;

    sf::Clock timer;

    RenderTexture render_texture;
    render_texture.create(SCREEN_W, SCREEN_H);

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
            
            main_window->parseEvent(event, stack);
        }
        
        EHC ehc(stack, false, false);

        main_window->onEvent(TickEvent(timer.getElapsedTime().asSeconds()), ehc);
        timer.restart();
        
        render_texture.clear(Black);

        main_window->draw(render_texture, stack);

        render_window.draw(sf::Sprite(render_texture.getSFMLTexture()));

        render_window.display();
    }
    
    delete main_window;
    main_window = nullptr;
    
    printf("UI System!\n");
    return 0;
}


Widget *createToolPaletteView(WindowStyle &window_style, PaletteViewAsset &palette_asset) {
    WindowStyle subwindow_style(window_style);
    subwindow_style.outline = 0;

    Window *subwindow = new Window(
        Widget::AUTO_ID,
        BoundLayoutBox(Vec2d(0, 100), Vec2d(218, 451)),
        "Tools",
        subwindow_style,
        false,
        true,
        false
    );

    subwindow->addChild(new ToolPaletteView(
        Widget::AUTO_ID,
        AnchorLayoutBox(Vec2d(), Vec2d(SCREEN_W, SCREEN_H), Vec2d(), Vec2d(SCREEN_W, SCREEN_H)),
        palette_asset
    ));

    return subwindow;
}


void setupPaletteManager() {
    PaletteManager::getInstance().setColorPalette(new ColorPalette(Red, White));

    PaletteManager::getInstance().setToolPalette(new ToolPalette(COLOR_PALETTE));

    PaletteManager::getInstance().setFilterPalette(new FilterPalette());
}


Menu *createMainMenu(
    Window &dialog_parent,
    FileDialogStyle &dialog_style,
    ScrollBarStyle &scrollbar_style
) {
    Menu *main_menu = new Menu(
        Widget::AUTO_ID,
        BoundLayoutBox(),
        RectButtonStyle(
            Color(0xd4d0c8ff),
            Color(0x000080ff),
            Color(0x000080ff),
            dialog_style.window.font,
            25,
            Black,
            White,
            White
        ),
        Color(0xd4d0c8ff)
    );

    ASSERT(main_menu, "Failed to allocate tool main menu!\n");

    main_menu->addMenuButton("File");
    main_menu->addButton(0, "Open", new CreateOpenFileDialog(dialog_parent, dialog_style, scrollbar_style));
    main_menu->addButton(0, "Save", new SaveFileAction());
    main_menu->addButton(0, "Save As", new CreateSaveAsFileDialog(dialog_parent, dialog_style));
    
    main_menu->addMenuButton("Filter");
    main_menu->addButton(1, "Lighten", new FilterAction(FilterPalette::LIGHTEN_FILTER));
    main_menu->addButton(1, "Darken", new FilterAction(FilterPalette::DARKEN_FILTER));
    main_menu->addButton(1, "Monochrome", new FilterAction(FilterPalette::MONOCHROME_FILTER));
    main_menu->addButton(1, "Negative", new FilterAction(FilterPalette::NEGATIVE_FILTER));

    return main_menu;
}
