/**
 * \file
 * \brief Contains sources of UI classes functions
*/


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "action.hpp"
#include "style.hpp"
#include "ui-system.hpp"


Container::Container(const Vector2D &pos, int z_, BaseUI &parent_, const Vector2D &size_) :
    BaseUI(pos, z_, parent_), elements(), size(size_) {}


void Container::addElement(BaseUI *ui_element) {
    ASSERT(ui_element, "UI element is nullptr!\n");
    ASSERT(isInsideRect(position, size, ui_element->position), "UI element is outside of the container!\n");
    // Container is empty
    if (elements.getSize() == 0) {
        elements.push_back(ui_element);
        return;
    }
    // Find place for UI element according to z-index
    /// TODO: Replace linear search with binary search
    for (size_t i = 0; i < elements.getSize(); i++) {
        if (ui_element->z < elements[i]->z) {
            elements.insert(i, ui_element);
            return;
        }
    }
    // Element has the biggest z-index
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
            new_focused->z = elements[controls_size - 1]->z + 1;    // POSSIBLE Z OVERLFOW
            
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
    frame.setPosition(abs_position);
    frame.setOutlineThickness(style.frame_outline);
    frame.setOutlineColor(sf::Color(style.frame_color));
    frame.setFillColor(sf::Color(0));

    Vector2D title_bar_pos = abs_position - Vector2D(style.frame_outline, style.title_bar_height);
    Vector2D title_bar_size = Vector2D(buttons.size.x + 2 * style.frame_outline, style.title_bar_height);

    sf::RectangleShape title_bar(title_bar_size);
    title_bar.setPosition(title_bar_pos);
    title_bar.setFillColor(style.frame_color);

    sf::Text text(title, style.font, style.font_size);

    sf::FloatRect text_rect = text.getLocalBounds();

    Vector2D title_pos = abs_position - Vector2D(0, style.title_bar_height);
    title_pos.y += (style.title_bar_height - text_rect.height) / 2;

    text.setPosition(title_pos);
    text.setFillColor(style.title_color);

    result.draw(frame);
    result.draw(title_bar);
    result.draw(text);
}


Window::Window(const Vector2D &pos, int z_, BaseUI &parent_, const Vector2D &size_, const sf::String &title_, const WindowStyle &style_) :
    BaseUI(pos, z_, parent_),
    title(title_),
    style(style_),
    buttons(Vector2D(), z_, *this, Vector2D(size_.x, 30)), 
    container(Vector2D(), z_, *this, size_) 
{}


void Window::draw(sf::RenderTexture &result) {
    container.draw(result);
    draw_frame(result);
    buttons.draw(result);
}


void Window::addElement(BaseUI *ui_element) {
    ASSERT(ui_element, "UI element is nullptr!\n");
    container.addElement(ui_element);
}


int Window::onMouseMove(int mouse_x, int mouse_y) {
    if (buttons.onMouseMove(mouse_x, mouse_y) == HANDLED)
        return HANDLED;
    if (container.onMouseMove(mouse_x, mouse_y) == HANDLED)
        return HANDLED;
    return UNHANDLED;
}


int Window::onMouseButtonUp(int mouse_x, int mouse_y, int button_id) {
    if (buttons.onMouseButtonUp(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    if (container.onMouseButtonUp(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    return UNHANDLED;
}


int Window::onMouseButtonDown(int mouse_x, int mouse_y, int button_id) {
    Vector2D abs_pos = getAbsolutePosition() - Vector2D(style.frame_outline, style.title_bar_height);
    Vector2D size = size + Vector2D(style.frame_outline * 2, style.title_bar_height + style.frame_outline);

    if (buttons.onMouseButtonDown(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    if (container.onMouseButtonDown(mouse_x, mouse_y, button_id) == HANDLED)
        return HANDLED;
    /*
    if (isInsideRect(abs_pos, size, Vector2D(mouse_x, mouse_y))) {
        printf("[%p]: Window clicked!\n", this);
        return HANDLED;
    }
    */
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
