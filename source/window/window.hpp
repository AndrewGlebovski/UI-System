/**
 * \file
 * \brief Contains window class and protypes of their functions 
*/


#ifndef _WINDOW_H_
#define _WINDOW_H_


#include "basic/container.hpp"
#include "basic/button.hpp"
#include "window/menu.hpp"
#include "widget/shape.hpp"
#include "basic/clock.hpp"


/// Controls how window looks
class WindowStyle {
public:
    const sf::Font &font;       ///< Title font
    const WindowAsset &asset;   ///< Textures used in window
    plug::Color title_color;    ///< Title color
    plug::Vec2d title_offset;   ///< Title offset from top-left corner
    unsigned font_size;         ///< Title font size
    float outline;              ///< Describes size of window resizing buttons
    plug::Vec2d tl_offset;      ///< Window inner area offset from top-left corner
    plug::Vec2d br_offset;      ///< Window inner area offset from bottom-right corner


    WindowStyle(
        const sf::Font &font_,
        const WindowAsset &asset_,
        plug::Color title_color_ = hex2Color(WINDOW_TITLE_COLOR),
        const plug::Vec2d &title_offset_ = WINDOW_TITLE_OFFSET,
        unsigned font_size_ = WINDOW_FONT_SIZE,
        float outline_ = WINDOW_OUTLINE, 
        const plug::Vec2d &tl_offset_ = WINDOW_TL_OFFSET,
        const plug::Vec2d &br_offset_ = WINDOW_BR_OFFSET
    ) :
        font(font_),
        asset(asset_),
        title_color(title_color_),
        title_offset(title_offset_),
        font_size(font_size_),
        outline(outline_),
        tl_offset(tl_offset_),
        br_offset(br_offset_)
    {}
};


/// Frame with widgets inside
class Window : public Widget {
public:
    /**
     * \brief Contstructs window
     * \note Position and size consider title bar and frame
    */
    Window(
        size_t id_, const plug::LayoutBox &layout_,
        const std::string &title_,
        const WindowStyle &style_,
        bool can_resize = true,
        bool can_move = true,
        bool can_close = true
    );

    Window(const Window &window) = delete;
    Window &operator = (const Window &window) = delete;

    /**
     * \brief Returns position of the window inner area relative to window top-left corner
    */
    plug::Vec2d getAreaPosition() const;

    /**
     * \brief Returns size of the window inner area
    */
    plug::Vec2d getAreaSize() const;

    /**
     * \brief Adds menu to window or replaces existing one
     * \warning Menu must be allocated using new and will be deleted by window
    */
    void setMenu(Menu *menu_);

    /**
     * \brief Returns pointer to menu
    */
    Menu *getMenu();

    /**
     * \brief Sets window position
    */
    bool setPosition(const plug::Vec2d &position_);

    /**
     * \brief Sets window size
    */
    bool setSize(const plug::Vec2d &size_);

    /**
     * \brief Finds pointer to widget inside window container
    */
    virtual Widget *findWidget(size_t widget_id) override;

    /**
     * \brief Adds new widget to window container
     * \warning Widgets should be allocated using new and will be deleted by window
    */
    virtual size_t addChild(Widget *child);

    /**
     * \brief Removes child from window container
    */
    virtual void removeChild(size_t child_id);

    /**
     * \brief Returns window style
    */
    const WindowStyle &getStyle() const { return style; }

    /**
     * \brief Draws window frame, title bar and its content
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;
    
    /**
     * \brief Broadcast events to window children
    */
    virtual void onEvent(const plug::Event &event, plug::EHC &ehc) override;

    /**
     * \brief Allows widget to change its position and size according to parent
    */
    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

    /**
     * \brief Checks children statuses
    */
    virtual void checkChildren() override;

    /**
     * \brief Delete menu if window has one
    */
    virtual ~Window() override;

protected:
    virtual void onMouseMove(const plug::MouseMoveEvent &event, plug::EHC &ehc) override;
    virtual void onMousePressed(const plug::MousePressedEvent &event, plug::EHC &ehc) override;

    WindowStyle style;          ///< Window style
    Container buttons;          ///< Window title bar and resize buttons
    Container container;        ///< Window content manager
    Menu *menu;                 ///< Window menu
    TextShape title;            ///< Window title

private:
    /**
     * \brief Adds buttons for managing window
    */
    void addButtons(bool can_resize, bool can_move, bool can_close);

    /**
     * \brief Adds close button to window
    */
    void addCloseButton();

    /**
     * \brief Adds expand button to window
    */
    void addExpandButton();

    /**
     * \brief Adds move button to window
    */
    void addMoveButton();

    /**
     * \brief Adds resize buttons to window
    */
    void addResizeButtons();
};


/// MainWindow of the application
class MainWindow : public Window {
public:
    MainWindow(
        size_t id_, const plug::LayoutBox &layout_,
        const std::string &title_,
        const WindowStyle &style_,
        const ClockStyle &clock_style_
    );

    /**
     * \brief Draws window then clock
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;

    /**
     * \brief Broadcast events to window children
    */
    virtual void onEvent(const plug::Event &event, plug::EHC &ehc) override;

    /**
     * \brief Parses SFML event into my own event system
    */
    void parseEvent(const sf::Event &event, plug::TransformStack &stack);

    /**
     * \brief Allows widget to change its position and size according to parent
    */
    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

private:
    Clock my_clock;
};


#endif
