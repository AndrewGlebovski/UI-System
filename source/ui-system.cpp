/**
 * \file
 * \brief Contains sources of UI classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "action.hpp"
#include "style.hpp"
#include "configs.hpp"
#include "ui-system.hpp"


BaseUI::BaseUI(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_) :
    position(position_), size(size_), z_index(z_index_), parent(parent_) {}


void BaseUI::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    Vector2D total_position = transforms[0] + position;

    sf::RectangleShape rect(Vector2D(25, 25));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(total_position);
    result.draw(rect);
}


void BaseUI::setSize(const Vector2D &new_size) {
    if (position.x + new_size.x > parent->size.x)
        size.x = parent->size.x - position.x;
    else
        size.x = new_size.x;
    
    if (position.y + size.y > parent->size.y)
        size.y = parent->size.y - position.y;
    else
        size.y = new_size.y;
}


void BaseUI::setPosition(const Vector2D &new_position) {
    if (new_position.x < 0)
        position.x = 0;
    else if (new_position.x + size.x > parent->size.x)
        position.x = parent->size.x - size.x;
    else
        position.x = new_position.x;
    
    if (new_position.y < 0)
        position.y = 0;
    else if (new_position.y + size.y > parent->size.y)
        position.y = parent->size.y - size.y;
    else
        position.y = new_position.y;
}


void BaseUI::apply_local_transform(List<Vector2D> &transforms) const {
    transforms[0] += position;
    transforms.push_back(position);
}


void BaseUI::cancel_local_transform(List<Vector2D> &transforms) const {
    transforms[0] -= transforms[transforms.getSize() - 1];
    transforms.pop_back();
}


Container::Container(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_) :
    BaseUI(position_, size_, z_index_, parent_), elements() {}


void Container::addElement(BaseUI *ui_element) {
    ASSERT(ui_element, "UI element is nullptr!\n");
    ASSERT(isInsideRect(position, size, ui_element->position), "UI element is outside of the container!\n");

    // Set ui child
    ui_element->parent = this;

    // Container is empty
    if (elements.getSize() == 0) {
        elements.push_back(ui_element);
        return;
    }
    // Find place for UI element according to z_index
    /// TODO: Replace linear search with binary search
    for (size_t i = 0; i < elements.getSize(); i++) {
        if (ui_element->z_index < elements[i]->z_index) {
            elements.insert(i, ui_element);
            return;
        }
    }
    // Element has the biggest z_index
    elements.push_back(ui_element);
}


void Container::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    apply_local_transform(transforms);

    for (size_t i = 0; i < elements.getSize(); i++)
        elements[i]->draw(result, transforms);
    
    cancel_local_transform(transforms);
}


int Container::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    apply_local_transform(transforms);
    size_t controls_size = elements.getSize();

    for (size_t i = controls_size - 1; i < controls_size; i--)
        if (elements[i]->onMouseMove(mouse_x, mouse_y, transforms) == HANDLED) {
            cancel_local_transform(transforms);
            return HANDLED;
        }

    cancel_local_transform(transforms);
    return UNHANDLED;
}


int Container::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    apply_local_transform(transforms);
    EVENT_STATUS status = UNHANDLED;
    size_t controls_size = elements.getSize();

    for (size_t i = controls_size - 1; i < controls_size; i--)
        if (elements[i]->onMouseButtonUp(mouse_x, mouse_y, button_id, transforms) == HANDLED) {
            status = HANDLED;
            break;
        }

    cancel_local_transform(transforms);
    return status;
}


int Container::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    apply_local_transform(transforms);
    EVENT_STATUS status = UNHANDLED;
    size_t controls_size = elements.getSize();

    for (size_t i = controls_size - 1; i < controls_size; i--)
        if (elements[i]->onMouseButtonDown(mouse_x, mouse_y, button_id, transforms) == HANDLED) {
            BaseUI *new_focused = elements[i];
            new_focused->z_index = elements[controls_size - 1]->z_index + 1;    // POSSIBLE Z OVERLFOW
            
            elements.remove(i);
            elements.push_back(new_focused);

            status = HANDLED;
            break;
        }

    cancel_local_transform(transforms);
    return status;
}


int Container::onKeyUp(int key_id) {
    for (size_t i = 0; i < elements.getSize(); i++)
        elements[i]->onKeyUp(key_id);
    
    return UNHANDLED;
}


int Container::onKeyDown(int key_id) {
    for (size_t i = 0; i < elements.getSize(); i++)
        elements[i]->onKeyDown(key_id);
    
    return UNHANDLED;
}


int Container::onTimer(float delta_time) {
    for (size_t i = 0; i < elements.getSize(); i++)
        elements[i]->onTimer(delta_time);
    
    return UNHANDLED;
}


Container::~Container() {
    for (size_t i = 0; i < elements.getSize(); i++) {
        ASSERT(elements[i], "Pointer to UI element is nullptr!\n");
        delete elements[i];
    }
}


void Window::drawFrame(sf::RenderTexture &result, const Vector2D &total_position) {
    sf::RectangleShape frame(container.size);
    frame.setPosition(total_position + (getAreaPosition() - position));
    frame.setOutlineThickness(style.frame_outline);
    frame.setOutlineColor(sf::Color(style.frame_color));
    frame.setFillColor(sf::Color(0));

    sf::RectangleShape title_bar(Vector2D(size.x, style.title_bar_height));
    title_bar.setPosition(total_position);
    title_bar.setFillColor(style.frame_color);

    sf::Text text(title, style.font, style.font_size);

    sf::FloatRect text_rect = text.getLocalBounds();

    Vector2D title_pos = total_position + Vector2D(style.frame_outline, 0);
    title_pos.y += (style.title_bar_height - text_rect.height) / 2;

    text.setPosition(title_pos);
    text.setFillColor(style.title_color);

    result.draw(frame);
    result.draw(title_bar);
    result.draw(text);
}


Window::Window(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_, 
    const sf::String &title_, const WindowStyle &style_
):
    BaseUI(position_, size_, z_index_, parent_),
    title(title_), style(style_),
    buttons(Vector2D(), Vector2D(), z_index_, this), 
    container(Vector2D(), Vector2D(), z_index_, this)
{
    buttons.position = -Vector2D(1, 1) * style.frame_outline;
    buttons.size = size + Vector2D(1, 1) * style.frame_outline;

    container.position = getAreaPosition() - position;
    container.size = getAreaSize();

    buttons.addElement(new MoveButton(
        Vector2D(1, 1) * style.frame_outline,
        Vector2D(container.size.x, style.title_bar_height - style.frame_outline),
        &buttons,
        *this
    ));
}


Vector2D Window::getAreaPosition() const {
    return position + Vector2D(style.frame_outline, style.title_bar_height);
}


Vector2D Window::getAreaSize() const {
    return size - Vector2D(style.frame_outline * 2, style.title_bar_height + style.frame_outline);
}


void Window::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    apply_local_transform(transforms);

    container.draw(result, transforms);
    buttons.draw(result, transforms);

    drawFrame(result, transforms[0]);

    cancel_local_transform(transforms);
}


void Window::addElement(BaseUI *ui_element) {
    ASSERT(ui_element, "UI element is nullptr!\n");
    container.addElement(ui_element);
}


int Window::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    apply_local_transform(transforms);
    EVENT_STATUS status = UNHANDLED;

    if (buttons.onMouseMove(mouse_x, mouse_y, transforms) == HANDLED)
        status = HANDLED;
    else if (container.onMouseMove(mouse_x, mouse_y, transforms) == HANDLED)
        status = HANDLED;
    else if (isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y)))
        status = HANDLED;

    cancel_local_transform(transforms);
    return status;
}


int Window::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    apply_local_transform(transforms);
    EVENT_STATUS status = UNHANDLED;

    if (buttons.onMouseButtonUp(mouse_x, mouse_y, button_id, transforms) == HANDLED)
        status = HANDLED;
    else if (container.onMouseButtonUp(mouse_x, mouse_y, button_id, transforms) == HANDLED)
        status = HANDLED;
    else if (isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y)))
        status = HANDLED;

    cancel_local_transform(transforms);
    return status;
}


int Window::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    apply_local_transform(transforms);
    EVENT_STATUS status = UNHANDLED;

    if (buttons.onMouseButtonDown(mouse_x, mouse_y, button_id, transforms) == HANDLED)
        status = HANDLED;
    else if (container.onMouseButtonDown(mouse_x, mouse_y, button_id, transforms) == HANDLED)
        status = HANDLED;
    else if (isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y))) {
        status = HANDLED;
    }

    cancel_local_transform(transforms);
    return status;
}


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


MainWindow::MainWindow(
    const Vector2D &position_, const Vector2D &size_, int z_index_,
    const sf::String &title_, const WindowStyle &style_
) :
    Window(position_, size_, z_index_, nullptr, title_, style_)
{}


void MainWindow::setSize(const Vector2D &new_size) {
    if (position.x + new_size.x > SCREEN_W)
        size.x = SCREEN_W - position.x;
    else
        size.x = new_size.x;
    
    if (position.y + size.y > SCREEN_H)
        size.y = SCREEN_H - position.y;
    else
        size.y = new_size.y;
}


void MainWindow::setPosition(const Vector2D &new_position) {
    if (new_position.x < 0)
        position.x = 0;
    else if (new_position.x + size.x > SCREEN_W)
        position.x = SCREEN_W - size.x;
    else
        position.x = new_position.x;
    
    if (new_position.y < 0)
        position.y = 0;
    else if (new_position.y + size.y > SCREEN_H)
        position.y = SCREEN_H - size.y;
    else
        position.y = new_position.y;
}


MoveButton::MoveButton(
    const Vector2D &position_, const Vector2D &size_, BaseUI *parent_, 
    Window &window_
) :
    BaseButton(position_, size_, 0, parent_),
    window(window_), prev_mouse(Vector2D()), is_moving(false)
{}


void MoveButton::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {}


int MoveButton::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    if (!is_moving) return UNHANDLED;

    // WE CHANGED WINDOW POSITION SO CURRENT TRANSFORM WILL BE INCORRECT
    // BUT IT DOESN'T MATTER CAUSE EVENT IS HANDLED
    // AND FARTHER BROADCASTING IS NOT NEEDED

    Vector2D new_position = window.position + (Vector2D(mouse_x, mouse_y) - prev_mouse);
    prev_mouse = Vector2D(mouse_x, mouse_y);

    window.setPosition(new_position);

    return HANDLED;
}


int MoveButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    apply_local_transform(transforms);
    EVENT_STATUS status = UNHANDLED;

    if (isInsideRect(transforms[0], size, Vector2D(mouse_x, mouse_y))) {
        status = HANDLED;
        is_moving = true;
        prev_mouse = Vector2D(mouse_x, mouse_y);
    }

    cancel_local_transform(transforms);
    return status;
}


int MoveButton::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    is_moving = false;
    return UNHANDLED;
}


bool isInsideRect(Vector2D position, Vector2D size, Vector2D point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}


void parseEvent(const sf::Event &event, Window &window, List<Vector2D> &transforms) {
    switch (event.type) {
        case sf::Event::KeyPressed:
            window.onKeyDown(event.key.code); break;
        case sf::Event::KeyReleased:
            window.onKeyUp(event.key.code); break;
        case sf::Event::MouseButtonPressed:
            window.onMouseButtonDown(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button, transforms); break;
        case sf::Event::MouseButtonReleased:
            window.onMouseButtonUp(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button, transforms); break;
        case sf::Event::MouseMoved:
            window.onMouseMove(event.mouseMove.x, event.mouseMove.y, transforms); break;
        default:
            return;
    }
}


#pragma GCC diagnostic pop
