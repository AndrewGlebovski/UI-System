#include "basic/clock.hpp"
#include "canvas/canvas_stuff.hpp"
#include "canvas/plugin_loader.hpp"
#include "canvas/palettes/palette_manager.hpp"
#include "common/utils.hpp"


/// Stores font, assets and styles
struct StyleManager {
    sf::Font font;
    WindowAsset window_asset;
    PaletteViewAsset palette_asset;

    WindowStyle window_style;
    ScrollBarStyle scrollbar_style;
    ClockStyle clock_style;
    FileDialogStyle dialog_style;
    RectButtonStyle menu_style;

    /// Inititalizes styles with values from configs.hpp
    StyleManager();

    /// Creates tool palette view in new subwindow
    Widget *createToolPaletteView();

    /// Creates color palette view in new subwindow
    Widget *createColorPaletteView();

    /// Creates palettes for palette manager
    void setupPaletteManager();

    /// Creates menu for main window
    Menu *createMainMenu(Window &dialog_parent);
};


int main() {
    sf::RenderWindow render_window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI", sf::Style::Fullscreen);

    // ENABLED VSYNC TO AVOID VISUAL ARTIFACTS WHEN WINDOWS ARE MOVING
    render_window.setVerticalSyncEnabled(true);

    StyleManager style_manager;
    
    // Init palettes
    style_manager.setupPaletteManager();
    
    MainWindow *main_window = new MainWindow(
        Widget::AUTO_ID,
        BoundLayoutBox(plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H)),
        "Paint",
        style_manager.window_style
    );
    ASSERT(main_window, "Failed to allocate tool main window!\n");

    main_window->setMenu(style_manager.createMainMenu(*main_window));

    main_window->addChild(new Clock(
        Widget::AUTO_ID,
        BoundLayoutBox(plug::Vec2d(), plug::Vec2d(100, 50)),
        style_manager.clock_style
    ));
    
    main_window->addChild(new FilterHotkey());

    main_window->addChild(openPicture(nullptr, style_manager.window_style, style_manager.scrollbar_style));
    main_window->addChild(style_manager.createToolPaletteView());
    main_window->addChild(style_manager.createColorPaletteView());

    PluginLoader plugin_loader(PLUGIN_DIR, *main_window->getMenu(), 1, *main_window);

    TransformStack stack;

    sf::Clock timer;

    RenderTexture render_texture;
    render_texture.create(SCREEN_W, SCREEN_H);

    while (render_window.isOpen()) {
        if (main_window->getStatus() == Widget::Status::Delete) {
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
        
        plug::EHC ehc = {stack, false, false};

        main_window->onEvent(plug::TickEvent(timer.getElapsedTime().asSeconds()), ehc);
        timer.restart();
        
        render_texture.clear(Black);

        main_window->draw(stack, render_texture);

        render_window.draw(sf::Sprite(render_texture.getSFMLTexture()));

        render_window.display();
    }
    
    delete main_window;
    main_window = nullptr;
    
    printf("UI System!\n");
    return 0;
}


// ============================================================================


StyleManager::StyleManager() :
    font(),
    window_asset(WINDOW_ASSET_DIR),
    palette_asset(PALETTE_ASSET_DIR),
    window_style(font, window_asset),
    scrollbar_style(),
    clock_style(font),
    dialog_style(window_style),
    menu_style(
        hex2Color(0xd4d0c8ff), hex2Color(0x000080ff), hex2Color(0x000080ff),
        dialog_style.window.font, 25, Black, White, White
    )
{
    ASSERT(font.loadFromFile(FONT_FILE), "Failed to load font!\n");
}


Widget *StyleManager::createToolPaletteView() {
    WindowStyle subwindow_style(window_style);
    subwindow_style.outline = 0;

    Window *subwindow = new Window(
        Widget::AUTO_ID,
        BoundLayoutBox(plug::Vec2d(0, 100), plug::Vec2d(218, 451)),
        "Tools",
        subwindow_style,
        false,
        true,
        false
    );

    subwindow->addChild(new ToolPaletteView(
        Widget::AUTO_ID,
        AnchorLayoutBox(plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H), plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H)),
        palette_asset
    ));

    return subwindow;
}


Widget *StyleManager::createColorPaletteView() {
    WindowStyle subwindow_style(window_style);
    subwindow_style.outline = 0;

    Window *subwindow = new Window(
        Widget::AUTO_ID,
        BoundLayoutBox(plug::Vec2d(1280, 100), plug::Vec2d(218, 451)),
        "Colors",
        subwindow_style,
        false,
        true,
        false
    );

    subwindow->addChild(new ColorPaletteView(
        Widget::AUTO_ID,
        AnchorLayoutBox(plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H), plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H))
    ));

    return subwindow;
}


void StyleManager::setupPaletteManager() {
    PaletteManager::getInstance().setColorPalette(new ColorPalette(Red, White));

    PaletteManager::getInstance().setToolPalette(new ToolPalette(COLOR_PALETTE));

    PaletteManager::getInstance().setFilterPalette(new FilterPalette(window_style));
}


Menu *StyleManager::createMainMenu(Window &dialog_parent) {
    Menu *main_menu = new Menu(
        Widget::AUTO_ID,
        BoundLayoutBox(),
        menu_style,
        hex2Color(0xd4d0c8ff)
    );

    ASSERT(main_menu, "Failed to allocate tool main menu!\n");

    main_menu->addMenuButton("File");
    main_menu->addButton(0, "Open", new CreateOpenFileDialog(dialog_parent, dialog_style, scrollbar_style));
    main_menu->addButton(0, "Save", new SaveFileAction());
    main_menu->addButton(0, "Save As", new CreateSaveAsFileDialog(dialog_parent, dialog_style));
    
    main_menu->addMenuButton("Filter");
    main_menu->addButton(1, "Lighten", new FilterAction(dialog_parent, FilterPalette::LIGHTEN_FILTER));
    main_menu->addButton(1, "Darken", new FilterAction(dialog_parent, FilterPalette::DARKEN_FILTER));
    main_menu->addButton(1, "Monochrome", new FilterAction(dialog_parent, FilterPalette::MONOCHROME_FILTER));
    main_menu->addButton(1, "Negative", new FilterAction(dialog_parent, FilterPalette::NEGATIVE_FILTER));
    main_menu->addButton(1, "Intensity Curve", new FilterAction(dialog_parent, FilterPalette::INTENSITY_CURVE));

    return main_menu;
}
