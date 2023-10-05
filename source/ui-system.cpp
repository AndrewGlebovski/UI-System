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


BaseUI::BaseUI(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI &parent_) :
    position(position_), size(size_), z_index(z_index_), parent(parent_) {}


void BaseUI::draw(sf::RenderTexture &result) {
    sf::RectangleShape rect(Vector2D(25, 25));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(position);
    result.draw(rect);
}


Vector2D BaseUI::getAbsolutePosition() const {
    return position + parent.getAbsolutePosition();
}


void BaseUI::setSize(const Vector2D &new_size) {
    if (position.x + new_size.x > parent.size.x)
        size.x = parent.size.x - size.x;
    
    if (position.y + size.y > parent.size.y)
        size.y = parent.size.y - size.y;
}


void BaseUI::setPosition(const Vector2D &new_position) {
    if (new_position.x < 0)
        position.x = 0;
    else if (new_position.x + size.x > parent.size.x)
        position.x = parent.size.x - size.x;
    
    if (new_position.y < 0)
        position.y = 0;
    else if (new_position.y + size.y > parent.size.y)
        position.y = parent.size.y - size.y;
}


Container::Container(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI &parent_) :
    BaseUI(position_, size_, z_index_, parent_), elements() {}


void Container::addElement(BaseUI *ui_element) {
    ASSERT(ui_element, "UI element is nullptr!\n");
    ASSERT(isInsideRect(position, size, ui_element->position), "UI element is outside of the container!\n");
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


void Container::draw(sf::RenderTexture &result) {
    for (size_t i = 0; i < elements.getSize(); i++)
        elements[i]->draw(result);
}


int Container::onMouseMove(int mouse_x, int mouse_y) {
    size_t controls_size = elements.getSize();

    for (size_t i = controls_size - 1; i < controls_size; i--)
        if (elements[i]->onMouseMove(mouse_x, mouse_y) == HANDLED)
            return HANDLED;

    return UNHANDLED;
}


int Container::onMouseButtonUp(int mouse_x, int mouse_y, int button_id) {
    size_t controls_size = elements.getSize();

    for (size_t i = controls_size - 1; i < controls_size; i--)
        if (elements[i]->onMouseButtonUp(mouse_x, mouse_y, button_id) == HANDLED)
            return HANDLED;

    return UNHANDLED;
}


int Container::onMouseButtonDown(int mouse_x, int mouse_y, int button_id) {
    size_t controls_size = elements.getSize();

    for (size_t i = controls_size - 1; i < controls_size; i--)
        if (elements[i]->onMouseButtonDown(mouse_x, mouse_y, button_id) == HANDLED) {
            BaseUI *new_focused = elements[i];
            new_focused->z_index = elements[controls_size - 1]->z_index + 1;    // POSSIBLE Z OVERLFOW
            
            elements.remove(i);
            elements.push_back(new_focused);

            return HANDLED;
        }

    return UNHANDLED;
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


void Window::draw_frame(sf::RenderTexture &result) {
    Vector2D abs_position = getAbsolutePosition();

    sf::RectangleShape frame(container.size);
    frame.setPosition(abs_position - position + getAreaPosition());
    frame.setOutlineThickness(style.frame_outline);
    frame.setOutlineColor(sf::Color(style.frame_color));
    frame.setFillColor(sf::Color(0));

    sf::RectangleShape title_bar(Vector2D(size.x, style.title_bar_height));
    title_bar.setPosition(abs_position);
    title_bar.setFillColor(style.frame_color);

    sf::Text text(title, style.font, style.font_size);

    sf::FloatRect text_rect = text.getLocalBounds();

    Vector2D title_pos = abs_position + Vector2D(style.frame_outline, 0);
    title_pos.y += (style.title_bar_height - text_rect.height) / 2;

    text.setPosition(title_pos);
    text.setFillColor(style.title_color);

    result.draw(frame);
    result.draw(title_bar);
    result.draw(text);
}


Window::Window(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI &parent_, 
    const sf::String &title_, const WindowStyle &style_
):
    BaseUI(position_, size_, z_index_, parent_),
    title(title_), style(style_),
    buttons(Vector2D(), Vector2D(), z_index_, *this), 
    container(Vector2D(), Vector2D(), z_index_, *this)
{
    buttons.position = -Vector2D(1, 1) * style.frame_outline;
    buttons.size = size + Vector2D(1, 1) * style.frame_outline;

    container.position = getAreaPosition() - position;
    container.size = getAreaSize();
}


Vector2D Window::getAreaPosition() const {
    return position + Vector2D(style.frame_outline, style.title_bar_height);
}


Vector2D Window::getAreaSize() const {
    return size - Vector2D(style.frame_outline * 2, style.title_bar_height + style.frame_outline);
}


void Window::draw(sf::RenderTexture &result) {
    container.draw(result);
    draw_frame(result);
    buttons.draw(result);
}


void Window::addElement(BaseUI *ui_element) {
    ASSERT(ui_element, "UI element is nullptr!\n");

    // ui_element->parent = container; DOESN'T WORK FOR SOME REASON
    container.addElement(ui_element);
}


int Window::onMouseMove(int mouse_x, int mouse_y) {
    if (buttons.onMouseMove(mouse_x, mouse_y) == HANDLED)
        return HANDLED;
    if (container.onMouseMove(mouse_x, mouse_y) == HANDLED)
        return HANDLED;
    if (isInsideRect(getAbsolutePosition(), size, Vector2D(mouse_x, mouse_y)))
        return HANDLED;
    
    return UNHANDLED;
}


int Window::onMouseButtonUp(int mouse_x, int mouse_y, int button_id) {
    if (buttons.onMouseButtonUp(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    if (container.onMouseButtonUp(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    if (isInsideRect(getAbsolutePosition(), size, Vector2D(mouse_x, mouse_y)))
        return HANDLED;
    
    return UNHANDLED;
}


int Window::onMouseButtonDown(int mouse_x, int mouse_y, int button_id) {
    if (buttons.onMouseButtonDown(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    if (container.onMouseButtonDown(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    if (isInsideRect(getAbsolutePosition(), size, Vector2D(mouse_x, mouse_y)))
        return HANDLED;
    
    return UNHANDLED;
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
    Window(position_, size_, z_index_, *this, title_, style_)
{}


Vector2D MainWindow::getAbsolutePosition() const {
    return position;
}


void MainWindow::setSize(const Vector2D &new_size) {
    if (position.x + new_size.x > SCREEN_W)
        size.x = SCREEN_W - size.x;
    
    if (position.y + size.y > SCREEN_H)
        size.y = SCREEN_H - size.y;
}


void MainWindow::setPosition(const Vector2D &new_position) {
    if (new_position.x < 0)
        position.x = 0;
    else if (new_position.x + size.x > SCREEN_W)
        position.x = SCREEN_W - size.x;
    
    if (new_position.y < 0)
        position.y = 0;
    else if (new_position.y + size.y > SCREEN_H)
        position.y = SCREEN_H - size.y;
}


bool isInsideRect(Vector2D position, Vector2D size, Vector2D point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}


void parseEvent(const sf::Event &event, Window &window) {
    switch (event.type) {
        case sf::Event::KeyPressed:
            window.onKeyDown(event.key.code); break;
        case sf::Event::KeyReleased:
            window.onKeyUp(event.key.code); break;
        case sf::Event::MouseButtonPressed:
            window.onMouseButtonDown(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button); break;
        case sf::Event::MouseButtonReleased:
            window.onMouseButtonUp(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button); break;
        case sf::Event::MouseMoved:
            window.onMouseMove(event.mouseMove.x, event.mouseMove.y); break;
        default:
            return;
    }
}


#pragma GCC diagnostic pop
