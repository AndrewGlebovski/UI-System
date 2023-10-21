/**
 * \file
 * \brief Contains sources of UI classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "asset.hpp"
#include "style.hpp"
#include "configs.hpp"
#include "key-id.hpp"
#include "ui-base.hpp"
#include "button.hpp"
#include "ui-system.hpp"


const float MIN_OUTLINE = 0.0001f;                          ///< If outline is smaller then window can not be resized
const Vector2D CLOSE_OFFSET = Vector2D(-45, 12);            ///< Window close button offset from window top-right corner
const Vector2D EXPAND_OFFSET = Vector2D(-85, 12);           ///< Window expand button offset from window top-right corner
const size_t CLOSE_BUTTON_ID = BaseUI::AUTO_ID + 1;         ///< Window append button ID
const size_t EXPAND_BUTTON_ID = BaseUI::AUTO_ID + 2;        ///< Window close button ID


Container::Container(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    bool focus_enabled_
) :
    BaseUI(id_, transform_, size_, z_index_, parent_),
    elements(), focused(0), focus_enabled(focus_enabled_)
{}


void Container::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    size_t count = elements.getSize();
    if (count == 0) return;

    TransformApplier add_transform(transforms, transform);

    for (size_t i = count - 1; i > focused; i--)
        elements[i]->draw(result, transforms);
    
    for (size_t i = focused - 1; i < count; i--)
        elements[i]->draw(result, transforms);
    
    elements[focused]->draw(result, transforms);
}


BaseUI *Container::findElement(size_t element_id) {
    BaseUI *result = nullptr;

    for (size_t i = 0; i < elements.getSize(); i++)
        if ((result = elements[i]->findElement(element_id))) return result;

    return BaseUI::findElement(element_id);
}


size_t Container::addChild(BaseUI *child) {
    ASSERT(child, "Child is nullptr!\n");

    // Set this container as child's parent
    child->parent = this;
    // Container is empty
    if (elements.getSize() == 0) {
        elements.push_back(child);
        focused = 0;
        return child -> getId();
    }
    // Find place for UI element according to z_index
    for (size_t i = 0; i < elements.getSize(); i++) {
        if (child->z_index > elements[i]->z_index) {
            elements.insert(i, child);
            focused = i;
            return child -> getId();
        }
    }
    // Element has the biggest z_index
    elements.push_back(child);
    focused = elements.getSize() - 1;
    return child -> getId();
}


void Container::removeElement(size_t index) {
    ASSERT(index < elements.getSize(), "Index is out of range!\n");

    if (index < focused) focused--;
    else if (index == focused) focused = elements.getSize() - 2;

    delete elements[index];
    elements.remove(index);
}


void Container::removeChild(size_t child_id) {
    for (size_t i = 0; i < elements.getSize(); i++) {
        if (elements[i]->getId() == child_id) {
            removeElement(i);
            break;
        }
    }
}


/**
 * \brief Special define for short loops inside container's event handler
*/
#define CONTAINER_FOR(CALL_FUNC, ...)                                               \
do {                                                                                \
    if (focus_enabled) {                                                               \
        if (elements[focused]->CALL_FUNC == HANDLED) return HANDLED;                \
        \
        for (size_t i = 0; i < focused; i++)                                        \
            if (elements[i]->CALL_FUNC == HANDLED) { __VA_ARGS__; return HANDLED; } \
        \
        for (size_t i = focused + 1; i < elements.getSize(); i++)                   \
            if (elements[i]->CALL_FUNC == HANDLED) { __VA_ARGS__; return HANDLED; } \
    }                                                                               \
    else {                                                                          \
        for (size_t i = 0; i < elements.getSize(); i++)                             \
            if (elements[i]->CALL_FUNC == HANDLED) return HANDLED;                  \
    }                                                                               \
} while(0)


int Container::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    if (elements.getSize() == 0) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    CONTAINER_FOR(onMouseMove(mouse_x, mouse_y, transforms));
    
    return UNHANDLED;
}


int Container::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (elements.getSize() == 0) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    CONTAINER_FOR(onMouseButtonUp(mouse_x, mouse_y, button_id, transforms));
    
    return UNHANDLED;
}


int Container::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (elements.getSize() == 0) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    CONTAINER_FOR(onMouseButtonDown(mouse_x, mouse_y, button_id, transforms), focused = i);
    
    return UNHANDLED;
}


int Container::onKeyUp(int key_id) {
    if (elements.getSize() == 0) return UNHANDLED;

    CONTAINER_FOR(onKeyUp(key_id));
    
    return UNHANDLED;
}


int Container::onKeyDown(int key_id) {
    if (elements.getSize() == 0) return UNHANDLED;

    CONTAINER_FOR(onKeyDown(key_id));
    
    return UNHANDLED;
}


int Container::onTimer(float delta_time) {
    for (size_t i = 0; i < elements.getSize(); i++)
        elements[i]->onTimer(delta_time);
    
    return UNHANDLED;
}


int Container::onParentResize() {
    for (size_t i = 0; i < elements.getSize(); i++)
        elements[i]->onParentResize();
    
    return UNHANDLED;
}


#undef CONTAINER_FOR


void Container::checkChildren() {
    size_t curr = 0;

    // ELEMENT OF THE LIST CAN BE DELETED IN ITERATION

    while(curr < elements.getSize()) {
        switch(elements[curr]->getStatus()) {
            case PASS: 
                elements[curr]->checkChildren();
                curr++;
                break;
            case DELETE:
                removeElement(curr);
                break;
            default:
                ASSERT(0, "Unknown status!\n");
        }
    }
}


Container::~Container() {
    for (size_t i = 0; i < elements.getSize(); i++) {
        ASSERT(elements[i], "Pointer to UI element is nullptr!\n");
        delete elements[i];
    }
}


/// Sets UI element status as DELETE
class CloseAction : public ButtonAction {
private:
    BaseUI *element;

public:
    CloseAction(BaseUI *element_) : element(element_) {}


    virtual void operator () () {
        element->setStatus(BaseUI::DELETE);
    }
};


/// Maximizes UI element size
class ExpandAction : public ButtonAction {
private:
    BaseUI *element;

public:
    ExpandAction(BaseUI *element_) : element(element_) {}


    virtual void operator () () {
        Transform new_transform = element->transform;
        new_transform.offset = Vector2D();
        element->tryTransform(new_transform);
        element->tryResize(Vector2D(SCREEN_W, SCREEN_H));
    }
};


#define ADD_RESIZE_BUTTON(POSITION, SIZE, TYPE) \
buttons.addChild(new ResizeButton(              \
    BaseUI::AUTO_ID,                            \
    Transform(POSITION),                        \
    SIZE,                                       \
    nullptr,                                    \
    *this,                                      \
    TYPE                                        \
))


Window::Window(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_, 
    const sf::String &title_, const WindowStyle &style_
) :
    BaseUI(id_, transform_, size_, z_index_, parent_),
    title(title_), style(style_),
    buttons(1, Transform(), size, 1, this, false), 
    container(2, Transform(), Vector2D(), 1, this)
{
    container.transform.offset = getAreaPosition() - transform.offset;
    container.size = getAreaSize();

    float offset = style.outline;

    buttons.addChild(new MoveButton(
        BaseUI::AUTO_ID,
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
        new CloseAction(this),
        nullptr,
        style.asset[WindowAsset::BUTTON_NORMAL],
        style.asset[WindowAsset::BUTTON_HOVER],
        style.asset[WindowAsset::BUTTON_PRESSED],
        style.asset[WindowAsset::CLOSE_ICON]
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

    buttons.addChild(new TextureIconButton(
        EXPAND_BUTTON_ID,
        Transform(Vector2D(size.x, 0) + EXPAND_OFFSET),
        2,
        nullptr,
        new ExpandAction(this),
        nullptr,
        style.asset[WindowAsset::BUTTON_NORMAL],
        style.asset[WindowAsset::BUTTON_HOVER],
        style.asset[WindowAsset::BUTTON_PRESSED],
        style.asset[WindowAsset::EXPAND_ICON]
    ));
}


#undef ADD_RESIZE_BUTTON


Vector2D Window::getAreaPosition() const {
    return transform.offset + style.tl_offset;
}


Vector2D Window::getAreaSize() const {
    return size - style.tl_offset - style.br_offset;
}


#define DRAW_TEXTURE(TEXTURE_ID, POSITION, TEXTURE_RECT_SIZE)                   \
do {                                                                            \
    tool_sprite.setTexture(style.asset[TEXTURE_ID]);                            \
    tool_sprite.setTextureRect(sf::IntRect(Vector2D(), TEXTURE_RECT_SIZE));     \
    tool_sprite.setPosition(POSITION + transforms[0].offset);                   \
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
    text.setPosition(transforms[0].offset + style.title_offset);
    text.setFillColor(style.title_color);

    result.draw(text);

    container.draw(result, transforms);
    buttons.draw(result, transforms);
}


#undef DRAW_TEXTURE


BaseUI *Window::findElement(size_t element_id) {
    BaseUI *result = container.findElement(element_id);
    if (result) return result;
    return BaseUI::findElement(element_id);
}


size_t Window::addChild(BaseUI *child) {
    return container.addChild(child);
}


void Window::removeChild(size_t child_id) {
    container.removeChild(child_id);
}


#define BROADCAST_MOUSE_EVENT(CALL_FUNC)                                            \
do {                                                                                \
    if (buttons.CALL_FUNC == HANDLED)                                               \
        return HANDLED;                                                             \
    else if (container.CALL_FUNC == HANDLED)                                        \
        return HANDLED;                                                             \
    else if (isInsideRect(transforms[0].offset, size, Vector2D(mouse_x, mouse_y)))  \
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
    buttons.onKeyUp(key_id);
    container.onKeyUp(key_id);
    return UNHANDLED;
}


int Window::onKeyDown(int key_id) {
    buttons.onKeyDown(key_id);
    container.onKeyDown(key_id);
    return UNHANDLED;
}


int Window::onTimer(float delta_time) {
    buttons.onTimer(delta_time);
    container.onTimer(delta_time);
    return UNHANDLED;
}


void Window::tryResize(const Vector2D &new_size) {
    BaseUI::tryResize(new_size);

    buttons.size = size;
    buttons.onParentResize();

    container.size = getAreaSize();
    container.onParentResize();

    buttons.findElement(CLOSE_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + CLOSE_OFFSET;
    buttons.findElement(EXPAND_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + EXPAND_OFFSET;
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

    buttons.findElement(CLOSE_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + CLOSE_OFFSET;
    buttons.findElement(EXPAND_BUTTON_ID)->transform.offset = Vector2D(size.x, 0) + EXPAND_OFFSET;
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
    size_t id_, const Transform &transform_, const Vector2D &size_, BaseUI *parent_, 
    Window &window_
) :
    BaseButton(id_, transform_, size_, 0, parent_),
    window(window_), prev_mouse(Vector2D()), is_moving(false)
{}


void MoveButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    /* DEBUG DRAWING
    TransformApplier add_transform(transforms, transform);

    sf::RectangleShape rect(size);
    rect.setPosition(transforms[0].offset);
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

    if (isInsideRect(transforms[0].offset, size, Vector2D(mouse_x, mouse_y))) {
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
    size_t id_, const Transform &transform_, const Vector2D &size_, BaseUI *parent_, 
    Window &window_, RESIZE_DIRECTION resize_dir_
) :
    BaseButton(id_, transform_, size_, 0, parent_),
    window(window_), prev_mouse(Vector2D()), is_moving(false), resize_dir(resize_dir_)
{}


void ResizeButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    /* DEBUG DRAWING
    TransformApplier add_transform(transforms, transform);

    sf::RectangleShape rect(size);
    rect.setPosition(transforms[0].offset);
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

    if (isInsideRect(transforms[0].offset, size, Vector2D(mouse_x, mouse_y))) {
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
