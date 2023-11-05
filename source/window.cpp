/**
 * \file
 * \brief Contains sources of window class functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "asset.hpp"
#include "configs.hpp"
#include "widget.hpp"
#include "container.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "window.hpp"


const float MIN_OUTLINE = 0.0001f;                          ///< If outline is smaller then window can not be resized
const Vector2D CLOSE_OFFSET = Vector2D(-45, 12);            ///< Window close button offset from window top-right corner
const Vector2D EXPAND_OFFSET = Vector2D(-85, 12);           ///< Window expand button offset from window top-right corner
const size_t CLOSE_BUTTON_ID = Widget::AUTO_ID + 1;         ///< Window append button ID
const size_t EXPAND_BUTTON_ID = Widget::AUTO_ID + 2;        ///< Window close button ID


/// Invisible button for moving windows
class MoveButton : public BaseButton {
protected:
    Window &window;         ///< Window to move
    Vector2D prev_mouse;    ///< Previous mouse click position
    bool is_moving;         ///< If moving is active

public:
    MoveButton(
        size_t id_, const Transform &transform_, const Vector2D &size_, Widget *parent_, 
        Window &window_
    );


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onParentResize() override;
};


/// Invisible button for resizing windows
class ResizeButton : public BaseButton {
protected:
    Window &window;         ///< Window to move
    Vector2D prev_mouse;    ///< Previous mouse click position
    bool is_moving;         ///< If moving is active
    const int resize_dir;   ///< Resize direction

public:
    enum RESIZE_DIRECTION {
        LEFT,
        TOP,
        BOTTOM,
        RIGHT,
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };


    ResizeButton(
        size_t id_, const Transform &transform_, const Vector2D &size_, Widget *parent_, 
        Window &window_, RESIZE_DIRECTION resize_dir_
    );


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onParentResize() override;
};


/// Sets widget status as DELETE
class CloseAction : public ButtonAction {
private:
    Widget &widget;

public:
    CloseAction(Widget &widget_) : widget(widget_) {}


    virtual void operator () () override {
        widget.setStatus(Widget::DELETE);
    }


    virtual ButtonAction *clone() override {
        return new CloseAction(widget);
    }
};


/// Maximizes widget size
class ExpandAction : public ButtonAction {
private:
    Widget &widget;

public:
    ExpandAction(Widget &widget_) : widget(widget_) {}


    virtual void operator () () override {
        Transform new_transform = widget.transform;
        new_transform.offset = Vector2D();
        widget.tryTransform(new_transform);
        widget.tryResize(Vector2D(SCREEN_W, SCREEN_H));
    }


    virtual ButtonAction *clone() override {
        return new ExpandAction(widget);
    }
};


#define ADD_RESIZE_BUTTON(POSITION, SIZE, TYPE) \
buttons.addChild(new ResizeButton(              \
    Widget::AUTO_ID,                            \
    Transform(POSITION),                        \
    SIZE,                                       \
    nullptr,                                    \
    *this,                                      \
    TYPE                                        \
))


Window::Window(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_, 
    const sf::String &title_, const WindowStyle &style_
) :
    Widget(id_, transform_, size_, z_index_, parent_),
    title(title_), style(style_),
    buttons(1, Transform(), size, 1, this, false), 
    container(2, Transform(), Vector2D(), 1, this),
    menu(nullptr)
{
    container.transform.offset = getAreaPosition() - transform.offset;
    container.size = getAreaSize();

    float offset = style.outline;

    buttons.addChild(new MoveButton(
        Widget::AUTO_ID,
        Transform(Vector2D(1, 1) * offset),
        Vector2D(size.x - 2 * offset, style.tl_offset.y - offset),
        nullptr,
        *this
    ));

    buttons.addChild(new TextureIconButton(
        CLOSE_BUTTON_ID,
        Transform(Vector2D(size.x, 0) + CLOSE_OFFSET),
        2,
        nullptr,
        new CloseAction(*this),
        nullptr,
        style.asset[WindowAsset::BUTTON_NORMAL],
        style.asset[WindowAsset::BUTTON_HOVER],
        style.asset[WindowAsset::BUTTON_PRESSED],
        style.asset[WindowAsset::CLOSE_ICON]
    ));

    buttons.addChild(new TextureIconButton(
        EXPAND_BUTTON_ID,
        Transform(Vector2D(size.x, 0) + EXPAND_OFFSET),
        2,
        nullptr,
        new ExpandAction(*this),
        nullptr,
        style.asset[WindowAsset::BUTTON_NORMAL],
        style.asset[WindowAsset::BUTTON_HOVER],
        style.asset[WindowAsset::BUTTON_PRESSED],
        style.asset[WindowAsset::EXPAND_ICON]
    ));

    if (offset < MIN_OUTLINE) return;

    ADD_RESIZE_BUTTON(Vector2D(0, offset),                          Vector2D(offset, size.y - 2 * offset),  ResizeButton::LEFT);
    ADD_RESIZE_BUTTON(Vector2D(offset, 0),                          Vector2D(size.x - 2 * offset, offset),  ResizeButton::TOP);
    ADD_RESIZE_BUTTON(Vector2D(offset, size.y - offset),            Vector2D(size.x - 2 * offset, offset),  ResizeButton::BOTTOM);
    ADD_RESIZE_BUTTON(Vector2D(size.x - offset, offset),            Vector2D(offset, size.y - 2 * offset),  ResizeButton::RIGHT);
    ADD_RESIZE_BUTTON(Vector2D(),                                   Vector2D(offset, offset),               ResizeButton::TOP_LEFT);
    ADD_RESIZE_BUTTON(Vector2D(size.x - offset, 0),                 Vector2D(offset, offset),               ResizeButton::TOP_RIGHT);
    ADD_RESIZE_BUTTON(Vector2D(0, size.y - offset),                 Vector2D(offset, offset),               ResizeButton::BOTTOM_LEFT);
    ADD_RESIZE_BUTTON(Vector2D(size.x - offset, size.y - offset),   Vector2D(offset, offset),               ResizeButton::BOTTOM_RIGHT);
}


#undef ADD_RESIZE_BUTTON


Vector2D Window::getAreaPosition() const {
    Vector2D area_position = transform.offset + style.tl_offset;
    if (menu) area_position.y += menu->size.y;
    return area_position;
}


Vector2D Window::getAreaSize() const {
    Vector2D area_size = size - style.tl_offset - style.br_offset;
    if (menu) area_size.y -= menu->size.y;
    return area_size;
}


#define DRAW_TEXTURE(TEXTURE_ID, POSITION, TEXTURE_RECT_SIZE)                   \
do {                                                                            \
    tool_sprite.setTexture(style.asset[TEXTURE_ID]);                            \
    tool_sprite.setTextureRect(sf::IntRect(Vector2D(), TEXTURE_RECT_SIZE));     \
    tool_sprite.setPosition(POSITION + transforms.front().offset);                   \
    result.draw(tool_sprite);                                                   \
} while(0)


void Window::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    Vector2D tl_size = style.asset[WindowAsset::FRAME_TL].getSize();
    Vector2D br_size = style.asset[WindowAsset::FRAME_BR].getSize();

    vec_t center_h = size.y - tl_size.y - br_size.y;
    vec_t center_w = size.x - tl_size.x - br_size.x;

    sf::Sprite tool_sprite;

    DRAW_TEXTURE(WindowAsset::FRAME_TL,     Vector2D(),                                             tl_size);
    DRAW_TEXTURE(WindowAsset::FRAME_L,      Vector2D(0, tl_size.y),                                 Vector2D(tl_size.x, center_h));
    DRAW_TEXTURE(WindowAsset::FRAME_BL,     Vector2D(0, tl_size.y + center_h),                      Vector2D(tl_size.x, br_size.y));
    DRAW_TEXTURE(WindowAsset::FRAME_B,      Vector2D(tl_size.x, tl_size.y + center_h),              Vector2D(center_w, br_size.y));
    DRAW_TEXTURE(WindowAsset::FRAME_BR,     Vector2D(tl_size.x + center_w, tl_size.y + center_h),   br_size);
    DRAW_TEXTURE(WindowAsset::FRAME_R,      Vector2D(tl_size.x + center_w, tl_size.y),              Vector2D(br_size.x, center_h));
    DRAW_TEXTURE(WindowAsset::FRAME_TR,     Vector2D(tl_size.x + center_w, 0),                      Vector2D(br_size.x, tl_size.y));
    DRAW_TEXTURE(WindowAsset::TITLE,        Vector2D(tl_size.x, 0),                                 Vector2D(center_w, tl_size.y));
    DRAW_TEXTURE(WindowAsset::FRAME_CENTER, tl_size,                                                Vector2D(center_w, center_h));

    sf::Text text(title, style.font, style.font_size);
    text.setPosition(transforms.front().offset + style.title_offset);
    text.setFillColor(style.title_color);

    result.draw(text);

    container.draw(result, transforms);
    buttons.draw(result, transforms);

    if (menu) menu->draw(result, transforms);
}


#undef DRAW_TEXTURE


void Window::setMenu(Menu *menu_) {
    if (menu) delete menu;

    menu = menu_;
    menu->transform.offset = style.tl_offset;
    menu->parent = this;
    menu->size.x = getAreaSize().x;

    container.transform.offset = getAreaPosition() - transform.offset;
    container.size = getAreaSize();
    container.onParentResize();
}


Menu *Window::getMenu() {
    return menu;
}


Widget *Window::findWidget(size_t widget_id) {
    Widget *result = container.findWidget(widget_id);
    if (result) return result;
    return Widget::findWidget(widget_id);
}


size_t Window::addChild(Widget *child) {
    return container.addChild(child);
}


void Window::removeChild(size_t child_id) {
    container.removeChild(child_id);
}


#define BROADCAST_MOUSE_EVENT(CALL_FUNC)                                            \
do {                                                                                \
    if (buttons.CALL_FUNC == HANDLED)                                               \
        return HANDLED;                                                             \
    if (menu && menu->CALL_FUNC == HANDLED)                                         \
        return HANDLED;                                                             \
    else if (container.CALL_FUNC == HANDLED)                                        \
        return HANDLED;                                                             \
    else if (isInsideRect(transforms.front().offset, size, Vector2D(mouse_x, mouse_y)))  \
        return HANDLED;                                                             \
} while(0)


int Window::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    BROADCAST_MOUSE_EVENT(onMouseMove(mouse_x, mouse_y, transforms));

    return UNHANDLED;
}


int Window::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    BROADCAST_MOUSE_EVENT(onMouseButtonUp(mouse_x, mouse_y, button_id, transforms));

    return UNHANDLED;
}


int Window::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    BROADCAST_MOUSE_EVENT(onMouseButtonDown(mouse_x, mouse_y, button_id, transforms));

    return UNHANDLED;
}


#undef BROADCAST_MOUSE_EVENT


int Window::onKeyUp(int key_id) {
    if (buttons.onKeyUp(key_id) == HANDLED) return HANDLED;
    if (container.onKeyUp(key_id) == HANDLED) return HANDLED;
    return UNHANDLED;
}


int Window::onKeyDown(int key_id) {
    if (buttons.onKeyDown(key_id) == HANDLED) return HANDLED;
    if (container.onKeyDown(key_id) == HANDLED) return HANDLED;
    return UNHANDLED;
}


int Window::onTimer(float delta_time) {
    buttons.onTimer(delta_time);
    container.onTimer(delta_time);
    return UNHANDLED;
}


void Window::tryResize(const Vector2D &new_size) {
    Widget::tryResize(new_size);

    buttons.size = size;
    buttons.onParentResize();

    container.size = getAreaSize();
    container.onParentResize();

    buttons.findWidget(CLOSE_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + CLOSE_OFFSET;
    buttons.findWidget(EXPAND_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + EXPAND_OFFSET;

    if (menu) menu->size.x = getAreaSize().x;
}


int Window::onParentResize() {
    // WINDOW MOVES TO KEEP ITS SIZE AS LONG AS POSSIBLE
    tryTransform(transform);

    // CHECKS IF RESIZE IS NECESSARY
    Vector2D new_size = size;

    if (transform.offset.x < 0) {
        transform.offset.x = 0;
        new_size.x = parent->size.x;
    }

    if (transform.offset.y < 0) {
        transform.offset.y = 0;
        new_size.y = parent->size.y;
    }

    tryResize(new_size);

    return UNHANDLED;
}


void Window::checkChildren() {
    container.checkChildren();
}


Window::~Window() {
    if (menu) delete menu;
}


MainWindow::MainWindow(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_,
    const sf::String &title_, const WindowStyle &style_
) :
    Window(id_, transform_, size_, z_index_, nullptr, title_, style_)
{}


void MainWindow::tryResize(const Vector2D &new_size) {
    size = new_size;

    if (transform.offset.x + new_size.x > SCREEN_W) size.x = SCREEN_W - transform.offset.x;
    else if (new_size.x < 0) size.x = 0;
    
    if (transform.offset.y + new_size.y > SCREEN_H) size.y = SCREEN_H - transform.offset.y;
    else if (new_size.y < 0) size.y = 0;

    buttons.size = size;
    buttons.onParentResize();

    container.size = getAreaSize();
    container.onParentResize();

    buttons.findWidget(CLOSE_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + CLOSE_OFFSET;
    buttons.findWidget(EXPAND_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + EXPAND_OFFSET;
}


void MainWindow::tryTransform(const Transform &new_transform) {
    transform = new_transform;

    if (new_transform.offset.x < 0)
        transform.offset.x = 0;
    else if (new_transform.offset.x + size.x > SCREEN_W)
        transform.offset.x = SCREEN_W - size.x;
    
    if (new_transform.offset.y < 0)
        transform.offset.y = 0;
    else if (new_transform.offset.y + size.y > SCREEN_H)
        transform.offset.y = SCREEN_H - size.y;
}


void MainWindow::parseEvent(const sf::Event &event, List<Transform> &transforms) {
    switch (event.type) {
        case sf::Event::KeyPressed:
            onKeyDown(event.key.code); break;
        case sf::Event::KeyReleased:
            onKeyUp(event.key.code); break;
        case sf::Event::MouseButtonPressed:
            onMouseButtonDown(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button, transforms); break;
        case sf::Event::MouseButtonReleased:
            onMouseButtonUp(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button, transforms); break;
        case sf::Event::MouseMoved:
            onMouseMove(event.mouseMove.x, event.mouseMove.y, transforms); break;
        default:
            return;
    }
}


MoveButton::MoveButton(
    size_t id_, const Transform &transform_, const Vector2D &size_, Widget *parent_, 
    Window &window_
) :
    BaseButton(id_, transform_, size_, 0, parent_),
    window(window_), prev_mouse(Vector2D()), is_moving(false)
{}


void MoveButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    /* DEBUG DRAWING
    TransformApplier add_transform(transforms, transform);

    sf::RectangleShape rect(size);
    rect.setPosition(transforms.front().offset);
    rect.setFillColor(sf::Color::Cyan);
    result.draw(rect);
    */
}


int MoveButton::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    if (!is_moving) return UNHANDLED;

    // WE CHANGED WINDOW POSITION SO CURRENT TRANSFORM WILL BE INCORRECT
    // BUT IT DOESN'T MATTER CAUSE EVENT IS HANDLED
    // AND FARTHER BROADCASTING IS NOT NEEDED

    Vector2D new_position = window.transform.offset + (Vector2D(mouse_x, mouse_y) - prev_mouse);
    prev_mouse = Vector2D(mouse_x, mouse_y);

    window.tryTransform(Transform(new_position));

    return HANDLED;
}


int MoveButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideRect(transforms.front().offset, size, Vector2D(mouse_x, mouse_y))) {
        is_moving = true;
        prev_mouse = Vector2D(mouse_x, mouse_y);
        return HANDLED;
    }

    return UNHANDLED;
}


int MoveButton::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    is_moving = false;
    return UNHANDLED;
}


int MoveButton::onParentResize() {
    float outline = window.getStyle().outline;
    Vector2D window_size = window.size;

    size = Vector2D(window_size.x - 2 * outline, window.getStyle().tl_offset.y - outline);
    
    return UNHANDLED;
}


ResizeButton::ResizeButton(
    size_t id_, const Transform &transform_, const Vector2D &size_, Widget *parent_, 
    Window &window_, RESIZE_DIRECTION resize_dir_
) :
    BaseButton(id_, transform_, size_, 0, parent_),
    window(window_), prev_mouse(Vector2D()), is_moving(false), resize_dir(resize_dir_)
{}


void ResizeButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    /* DEBUG DRAWING
    TransformApplier add_transform(transforms, transform);

    sf::RectangleShape rect(size);
    rect.setPosition(transforms.front().offset);
    rect.setFillColor(sf::Color::Red);
    result.draw(rect);
    */
}


int ResizeButton::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    if (!is_moving) return UNHANDLED;

    Vector2D shift = Vector2D(mouse_x, mouse_y) - prev_mouse;
    prev_mouse = Vector2D(mouse_x, mouse_y);

    switch (resize_dir) {
        case LEFT: 
            shift.y = 0;
            window.tryTransform(Transform(window.transform.offset + shift));
            window.tryResize(window.size - shift);
            break;
        case TOP: 
            shift.x = 0;
            window.tryTransform(Transform(window.transform.offset + shift));
            window.tryResize(window.size - shift);
            break;
        case BOTTOM: 
            shift.x = 0;
            window.tryResize(window.size + shift);
            break;
        case RIGHT: 
            shift.y = 0;
            window.tryResize(window.size + shift);
            break;
        case TOP_LEFT: 
            window.tryTransform(Transform(window.transform.offset + shift));
            window.tryResize(window.size - shift);
            break;
        case TOP_RIGHT: 
            window.tryTransform(Transform(window.transform.offset + Vector2D(0, shift.y)));
            window.tryResize(window.size + Vector2D(shift.x, -shift.y));
            break;
        case BOTTOM_LEFT: 
            window.tryTransform(Transform(window.transform.offset + Vector2D(shift.x, 0)));
            window.tryResize(window.size + Vector2D(-shift.x, shift.y));
            break;
        case BOTTOM_RIGHT: 
            window.tryResize(window.size + shift);
            break;
        default:
            printf("Unknown resize direction!\n"); abort();
    }

    return HANDLED;
}


int ResizeButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideRect(transforms.front().offset, size, Vector2D(mouse_x, mouse_y))) {
        is_moving = true;
        prev_mouse = Vector2D(mouse_x, mouse_y);
        return HANDLED;
    }

    return UNHANDLED;
}


int ResizeButton::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    is_moving = false;
    return UNHANDLED;
}


int ResizeButton::onParentResize() {
    float outline = window.getStyle().outline;
    Vector2D window_size = window.size;

    switch (resize_dir) {
        case LEFT: 
            size = Vector2D(outline, window_size.y - 2 * outline);
            break;
        case TOP: 
            size = Vector2D(window_size.x - 2 * outline, outline);
            break;
        case BOTTOM: 
            transform.offset = Vector2D(outline, window_size.y - outline);
            size = Vector2D(window_size.x - 2 * outline, outline);
            break;
        case RIGHT: 
            transform.offset = Vector2D(window_size.x - outline, outline);
            size = Vector2D(outline, window_size.y - 2 * outline);
            break;
        case TOP_LEFT: 
            size = Vector2D(outline, outline);
            break;
        case TOP_RIGHT: 
            transform.offset = Vector2D(window_size.x - outline, 0);
            size = Vector2D(outline, outline);
            break;
        case BOTTOM_LEFT: 
            transform.offset = Vector2D(0, window_size.y - outline);
            size = Vector2D(outline, outline);
            break;
        case BOTTOM_RIGHT: 
            transform.offset = Vector2D(window_size.x - outline, window_size.y - outline);
            size = Vector2D(outline, outline);
            break;
        default:
            printf("Unknown resize direction!\n"); abort();
    }
    
    return UNHANDLED;
}


#pragma GCC diagnostic pop
