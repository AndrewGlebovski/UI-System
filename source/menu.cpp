/**
 * \file
 * \brief Contains sources of menu class functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include <limits>
#include "vector.hpp"
#include "list.hpp"
#include "asset.hpp"
#include "widget.hpp"
#include "button.hpp"
#include "menu.hpp"


const size_t INVALID_OPENED = std::numeric_limits<size_t>::max();   ///< No opened menu at the moment
const Vector2D add_size(20, 20);                                    ///< Additional size to text rect


MenuButton::MenuButton(
    const Transform &transform_, const Vector2D &size_, Widget *parent_,
    const sf::String &text_, const ButtonStyle &style_,
    const sf::Color &normal_, const sf::Color &hover_, const sf::Color &pressed_
) : 
    RectButton(AUTO_ID, transform_, size_, 1, parent_, nullptr, nullptr, text_, style_, normal_, hover_, pressed_), 
    buttons(), is_opened(false)
{}


void MenuButton::addButton(const sf::String &text_, ButtonAction *action_) {
    Widget *last_button = (buttons.size()) ? buttons.back() : nullptr;

    sf::Text btn_text(text_, style.font, style.font_size);
    Vector2D new_size = Vector2D(btn_text.getLocalBounds().width + add_size.x, size.y);

    Transform new_transform(transform);
    new_transform.offset = Vector2D();

    if (last_button) {
        new_transform.offset.y = last_button->transform.offset.y + last_button->size.y;

        if (new_size.x > last_button->size.x) {
            for (size_t i = 0; i < buttons.size(); i++)
                buttons[i]->size.x = new_size.x;
        }
        else new_size.x = last_button->size.x;
    }
    else new_transform.offset.y = size.y;

    buttons.push_back(new RectButton(
        AUTO_ID,
        new_transform,
        new_size,
        0,
        this,
        action_,
        nullptr,
        text_,
        style,
        normal_color,
        hover_color,
        pressed_color
    ));
}


void MenuButton::setOpened(bool is_opened_) {
    is_opened = is_opened_;
}


void MenuButton::draw(sf::RenderTarget &result, List<Transform> &transforms) {
    RectButton::draw(result, transforms);

    TransformApplier add_transform(transforms, transform);

    if (is_opened) {
        for (size_t i = 0; i < buttons.size(); i++)
            buttons[i]->draw(result, transforms);
    }
}


int MenuButton::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    RectButton::onMouseMove(mouse_x, mouse_y, transforms);

    if (!is_opened) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    for (size_t i = 0; i < buttons.size(); i++)
        buttons[i]->onMouseMove(mouse_x, mouse_y, transforms);

    return UNHANDLED;
}


int MenuButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (RectButton::onMouseButtonDown(mouse_x, mouse_y, button_id, transforms) == HANDLED)
        return HANDLED;

    if (!is_opened) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    for (size_t i = 0; i < buttons.size(); i++)
        if (buttons[i]->onMouseButtonDown(mouse_x, mouse_y, button_id, transforms) == HANDLED)
            return HANDLED;

    return UNHANDLED;
}


int MenuButton::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (RectButton::onMouseButtonUp(mouse_x, mouse_y, button_id, transforms) == HANDLED)
        return HANDLED;

    if (!is_opened) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    for (size_t i = 0; i < buttons.size(); i++)
        if (buttons[i]->onMouseButtonDown(mouse_x, mouse_y, button_id, transforms) == HANDLED)
            return HANDLED;

    return UNHANDLED;
}


MenuButton::~MenuButton() {
    for (size_t i = 0; i < buttons.size(); i++) {
        ASSERT(buttons[i], "Button is nullptr!\n");
        delete buttons[i];
    }
}


Menu::Menu(
    size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
    const MenuStyle &style_
) :
    Widget(id_, transform_, Vector2D(), z_index_, parent_),
    buttons(), style(style_), opened(INVALID_OPENED)
{
    sf::Text btn_text("Test", style.font, style.font_size);
    size.y = btn_text.getLocalBounds().height + add_size.y;
}


bool Menu::isMenuOpened() const {
    return (opened < INVALID_OPENED);
}


void Menu::openMenu(size_t menu_id) {
    ASSERT(menu_id < buttons.size(), "Invalid menu id!\n");

    if (opened == menu_id) {
        buttons[opened]->setOpened(false);
        opened = INVALID_OPENED;
    }
    else if (isMenuOpened()) {
        buttons[opened]->setOpened(false);
        buttons[menu_id]->setOpened(true);
        opened = menu_id;
    }
    else {
        buttons[menu_id]->setOpened(true);
        opened = menu_id;
    }
}


void Menu::addMenuButton(const sf::String &text) {
    ButtonStyle button_style(
        style.font,
        style.font_size,
        style.font_normal,
        style.font_hover,
        style.font_pressed
    );

    MenuButton *last_menu_button = buttons.size() ? buttons.back() : nullptr;

    sf::Text btn_text(text, style.font, style.font_size);
    Vector2D new_size(btn_text.getLocalBounds().width + add_size.x, size.y);

    Transform new_transform;

    if (last_menu_button) {
        new_transform = last_menu_button->transform;
        new_transform.offset.x += last_menu_button->size.x;
    }

    buttons.push_back(new MenuButton(
        new_transform,
        new_size,
        this,
        text,
        button_style,
        style.normal,
        style.hover,
        style.pressed
    ));
}


void Menu::addButton(size_t menu_id, const sf::String &text, ButtonAction *action) {
    ASSERT(menu_id < buttons.size(), "Invalid menu id!\n");
    buttons[menu_id]->addButton(text, action);
}


void Menu::draw(sf::RenderTarget &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    sf::RectangleShape rect(size);
    rect.setPosition(transforms.front().offset);
    rect.setFillColor(style.normal);
    result.draw(rect);

    for (size_t i = 0; i < buttons.size(); i++)
        buttons[i]->draw(result, transforms);
}


int Menu::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    for (size_t i = 0; i < buttons.size(); i++)
        buttons[i]->onMouseMove(mouse_x, mouse_y, transforms);

    return UNHANDLED;
}


int Menu::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i]->onMouseButtonDown(mouse_x, mouse_y, button_id, transforms) == HANDLED) {
            openMenu(i);
            return HANDLED;
        }
    }

    if (isMenuOpened()) {
        buttons[opened]->setOpened(false);
        opened = INVALID_OPENED;
    }
    
    return UNHANDLED;
}


int Menu::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    for (size_t i = 0; i < buttons.size(); i++)
        if (buttons[i]->onMouseButtonUp(mouse_x, mouse_y, button_id, transforms) == HANDLED)
            return HANDLED;

    return UNHANDLED;
}


int Menu::onParentResize() {
    tryResize(Vector2D(parent->size.x, size.y));
    return UNHANDLED;
}


Menu::~Menu() {
    for (size_t i = 0; i < buttons.size(); i++) {
        ASSERT(buttons[i], "MenuButton is nullptr!\n");
        delete buttons[i];
    }
}


#pragma GCC diagnostic pop
