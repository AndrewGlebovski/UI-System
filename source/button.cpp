/**
 * \file
 * \brief Contains sources of button classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "asset.hpp"
#include "widget.hpp"
#include "button.hpp"


ActionButton::ActionButton(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    ButtonAction *action_, ButtonGroup *group_
) :
    BaseButton(id_, transform_, size_, z_index_, parent_),
    action(action_), group(group_), status(BUTTON_NORMAL)
{
    if (group) group->addButton(this);
}


ActionButton::ActionButton(const ActionButton &button) :
    BaseButton(AUTO_ID, button.transform, button.size, button.z_index, button.parent),
    action(button.action->clone()), group(button.group), status(BUTTON_NORMAL)
{
    if (group) group->addButton(this);
}


ActionButton &ActionButton::operator = (const ActionButton &button)  {
    if (this != &button) {
        if (action) delete action;

        transform = button.transform;
        size = button.size;
        z_index = button.z_index;
        parent = button.parent;
        action = button.action->clone();
        group = button.group;
    }

    return *this;
}


bool ActionButton::isInGroup() const { return (group); }


bool ActionButton::isPressedInGroup() const { return isInGroup() && group->getPressed() == this; }


void ActionButton::setButtonStatus(BUTTON_STATUS new_status) { status = new_status; };


int ActionButton::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideButton(Vector2D(mouse_x, mouse_y) - transforms.front().offset)) {
        if (status != BUTTON_PRESSED)
            setButtonStatus(BUTTON_HOVER);

        return HANDLED;
    }
    
    if (!isPressedInGroup()) setButtonStatus(BUTTON_NORMAL);

    return UNHANDLED;
}


int ActionButton::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (!isInsideButton(Vector2D(mouse_x, mouse_y) - transforms.front().offset)) return UNHANDLED;

    setButtonStatus(BUTTON_PRESSED);
    if (isInGroup()) group->setPressed(this);

    if (action) (*action)();
    return HANDLED;
}


int ActionButton::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideButton(Vector2D(mouse_x, mouse_y) - transforms.front().offset)) {
        if (!isPressedInGroup()) setButtonStatus(BUTTON_HOVER);

        return HANDLED;
    }
    
    if (!isPressedInGroup()) setButtonStatus(BUTTON_NORMAL);

    return UNHANDLED;
}


ActionButton::~ActionButton() {
    if (action) delete action;
}


size_t ButtonGroup::getIndex(ActionButton *button) const {
    for (size_t i = 0; i < buttons.size(); i++)
        if (buttons[i] == button) return i;
    
    return buttons.size();
}


ButtonGroup::ButtonGroup() : buttons(), pressed(0) {}


void ButtonGroup::setPressed(ActionButton *new_pressed) {
    size_t index = getIndex(new_pressed);
    if (index < buttons.size()) {
        buttons[pressed]->setButtonStatus(ActionButton::BUTTON_NORMAL);
        buttons[index]->setButtonStatus(ActionButton::BUTTON_PRESSED);
        pressed = index;
    }
}


ActionButton *ButtonGroup::getPressed() { return buttons[pressed]; }


void ButtonGroup::addButton(ActionButton *new_button) {
    if (!isInGroup(new_button)) {
        buttons.push_back(new_button);

        if (buttons.size() == 1) setPressed(new_button);
    }
}


void ButtonGroup::removeButton(ActionButton *button) {
    size_t index = getIndex(button);
    if (index < buttons.size()) {
        if (index == pressed) setPressed(buttons[0]);
        buttons.remove(index);
    }
}


bool ButtonGroup::isInGroup(ActionButton *button) const {
    return (getIndex(button) < buttons.size()); 
}


RectButton::RectButton(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    ButtonAction *action_, ButtonGroup *group_,
    const sf::String &text_, const ButtonStyle &style_,
    const sf::Color &normal_, const sf::Color &hover_, const sf::Color &pressed_
) :
    ActionButton(id_, transform_, size_, z_index_, parent_, action_, group_),
    text(text_), style(style_),
    normal_color(normal_), hover_color(hover_), pressed_color(pressed_)
{}


bool RectButton::isInsideButton(const Vector2D &point) {
    return isInsideRect(Vector2D(), size, point);
}


void RectButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    sf::Text btn_text(text, style.font, style.font_size);
    sf::FloatRect text_rect = btn_text.getLocalBounds();
    Vector2D text_offset((size.x - text_rect.width) / 2, (size.y - text_rect.height) / 2);

    btn_text.setPosition(transforms.front().offset + text_offset);

    sf::RectangleShape btn_rect(size);

    switch(status) {
        case BUTTON_NORMAL:
            btn_rect.setFillColor(normal_color);
            btn_text.setFillColor(style.font_normal);
            break;
        case BUTTON_HOVER:
            btn_rect.setFillColor(hover_color);
            btn_text.setFillColor(style.font_hover);
            break;
        case BUTTON_PRESSED:
            btn_rect.setFillColor(pressed_color);
            btn_text.setFillColor(style.font_pressed);
            break;
        default: ASSERT(0, "Invalid button status!\n");
    }

    btn_rect.setPosition(transforms.front().offset);

    result.draw(btn_rect);
    result.draw(btn_text);
}


TextureButton::TextureButton(
    size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
    ButtonAction *action_, ButtonGroup *group_,
    const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_
) :
    ActionButton(id_, transform_, Vector2D(), z_index_, parent_, action_, group_),
    normal(normal_), hover(hover_), pressed(pressed_)
{
    size = Vector2D(normal.getSize().x, normal.getSize().y);
}


bool TextureButton::isInsideButton(const Vector2D &point) {
    return isInsideRect(Vector2D(), size, point);
}


void TextureButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    sf::Sprite btn_sprite;

    switch(status) {
        case BUTTON_NORMAL: btn_sprite.setTexture(normal); break;
        case BUTTON_HOVER: btn_sprite.setTexture(hover); break;
        case BUTTON_PRESSED: btn_sprite.setTexture(pressed); break;
        default: ASSERT(0, "Invalid button status!\n");
    }
    
    btn_sprite.setPosition(transforms.front().offset);

    result.draw(btn_sprite);
}


TextureIconButton::TextureIconButton(
    size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
    ButtonAction *action_, ButtonGroup *group_,
    const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_,
    const sf::Texture &icon_
) : 
    TextureButton(id_, transform_, z_index_, parent_, action_, group_, normal_, hover_, pressed_),
    icon(icon_)
{}


void TextureIconButton::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TextureButton::draw(result, transforms);

    sf::Sprite icon_sprite(icon);
    icon_sprite.setPosition(transforms.front().offset + transform.offset);
    result.draw(icon_sprite);
}


#pragma GCC diagnostic pop
