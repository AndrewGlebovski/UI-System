#include "basic/clock.hpp"
#include "canvas/canvas_stuff.hpp"
#include "widget/shape.hpp"
#include "canvas/plugin_loader.hpp"


/// Creates tool palette view in new subwindow
Widget *createToolPaletteView(ToolPalette *tool_palette, WindowStyle &window_style, PaletteViewAsset &palette_asset);


int main() {
    sf::RenderWindow render_window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI", sf::Style::Fullscreen);

    // ENABLED VSYNC TO AVOID VISUAL ARTIFACTS WHEN WINDOWS ARE MOVING
    render_window.setVerticalSyncEnabled(true);

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
    
    Menu *main_menu = new Menu(
        Widget::AUTO_ID,
        BoundLayoutBox(),
        RectButtonStyle(
            Color(0xd4d0c8ff),
            Color(0x000080ff),
            Color(0x000080ff),
            font,
            25,
            Black,
            White,
            White
        ),
        Color(0xd4d0c8ff)
    );
    ASSERT(main_menu, "Failed to allocate tool main menu!\n");

    ColorPalette *color_palette = new ColorPalette(Red, Red);
    ASSERT(color_palette, "Failed to allocate color palette!\n");

    ToolPalette *tool_palette = new ToolPalette(*color_palette);
    ASSERT(tool_palette, "Failed to allocate tool palette!\n");

    CanvasGroup *canvas_group = new CanvasGroup();
    ASSERT(canvas_group, "Failed to allocate canvas group!\n");
    
    FilterPalette *filter_palette = new FilterPalette();
    ASSERT(filter_palette, "Failed to allocate filter palette!\n");

    FileDialogStyle dialog_style(window_style);

    main_menu->addMenuButton("File");
    main_menu->addButton(0, "Open", new CreateOpenFileDialog(*main_window, *tool_palette, *color_palette, *canvas_group, dialog_style, scrollbar_style));
    main_menu->addButton(0, "Save", new SaveFileAction(*canvas_group));
    main_menu->addButton(0, "Save As", new CreateSaveAsFileDialog(*main_window, *canvas_group, dialog_style));
    
    main_menu->addMenuButton("Filter");
    main_menu->addButton(1, "Lighten", new FilterAction(FilterPalette::LIGHTEN_FILTER, *filter_palette, *canvas_group));
    main_menu->addButton(1, "Darken", new FilterAction(FilterPalette::DARKEN_FILTER, *filter_palette, *canvas_group));
    main_menu->addButton(1, "Monochrome", new FilterAction(FilterPalette::MONOCHROME_FILTER, *filter_palette, *canvas_group));
    main_menu->addButton(1, "Negative", new FilterAction(FilterPalette::NEGATIVE_FILTER, *filter_palette, *canvas_group));

    main_window->setMenu(main_menu);

    main_window->addChild(new Clock(
        Widget::AUTO_ID,
        BoundLayoutBox(Vec2d(), Vec2d(100, 50)),
        clock_style
    ));
    
    main_window->addChild(new FilterHotkey(
        main_window,
        *filter_palette,
        *canvas_group
    ));

    main_window->addChild(openPicture(nullptr, *tool_palette, *color_palette, *canvas_group, window_style, scrollbar_style));
    main_window->addChild(createToolPaletteView(tool_palette, window_style, palette_asset));
    
    PluginLoader plugin_loader("plugins", *tool_palette, *filter_palette, *main_menu, 1, *canvas_group);

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
        
        Widget::EHC ehc(stack, false, false);

        main_window->onEvent(Widget::TickEvent(timer.getElapsedTime().asSeconds()), ehc);
        timer.restart();
        
        render_texture.clear(Black);

        main_window->draw(render_texture, stack);

        render_window.draw(sf::Sprite(render_texture.getSFMLTexture()));

        render_window.display();
    }
    
    delete main_window;
    delete tool_palette;
    delete color_palette;
    delete canvas_group;
    delete filter_palette;
    
    printf("UI System!\n");
    return 0;
}


Widget *createToolPaletteView(ToolPalette *tool_palette, WindowStyle &window_style, PaletteViewAsset &palette_asset) {
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
        tool_palette,
        palette_asset
    ));

    return subwindow;
}
