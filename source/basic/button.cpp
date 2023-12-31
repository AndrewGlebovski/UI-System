/**
 * \file
 * \brief Contains sources of button classes functions
*/


#include <SFML/Graphics.hpp>
#include "basic/button.hpp"


// ============================================================================


ActionButton::ActionButton(
    size_t id_, const LayoutBox &layout_,
    ButtonAction *action_
) :
    BaseButton(id_, layout_),
    action(action_), group(nullptr), status(BUTTON_NORMAL)
{
    if (group) group->addButton(this);
}


ActionButton::ActionButton(const ActionButton &button) :
    BaseButton(AUTO_ID, button.getLayoutBox()),
    action(button.action->clone()), group(button.group), status(BUTTON_NORMAL)
{
    if (group) group->addButton(this);
}


ActionButton &ActionButton::operator = (const ActionButton &button)  {
    if (this != &button) {
        if (action) delete action;

        layout = button.getLayoutBox().clone();
        action = button.action->clone();

        if (isInGroup()) group->removeButton(this);

        group = button.group;
        group->addButton(this);
    }

    return *this;
}


bool ActionButton::isInGroup() const { return (group); }


bool ActionButton::isPressedInGroup() const { return isInGroup() && group->getPressed() == this; }


void ActionButton::setButtonStatus(BUTTON_STATUS new_status) { status = new_status; };


void ActionButton::setButtonGroup(ButtonGroup *group_) {
    if (group_ == group) return;

    if (isInGroup()) group->removeButton(this);

    group = group_;

    if (group) group->addButton(this);
}


ButtonGroup *ActionButton::getButtonGroup() { return group; }


void ActionButton::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());
    
    if (isInsideButton(event.pos - global_position, global_size)) {
        if (status != BUTTON_PRESSED)
            setButtonStatus(BUTTON_HOVER);

        ehc.overlapped = true;
        return;
    }
    
    if (!isPressedInGroup()) setButtonStatus(BUTTON_NORMAL);
}


void ActionButton::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());
    
    if (!isInsideButton(event.pos - global_position, global_size)) return;

    setButtonStatus(BUTTON_PRESSED);
    if (isInGroup()) group->setPressed(this);

    if (action) (*action)();
    ehc.stopped = true;
}


void ActionButton::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideButton(event.pos - global_position, global_size)) {
        if (!isPressedInGroup()) setButtonStatus(BUTTON_HOVER);
        ehc.stopped = true;
        return;
    }
    
    if (!isPressedInGroup()) setButtonStatus(BUTTON_NORMAL);
}


ActionButton::~ActionButton() {
    if (action) delete action;
}


// ============================================================================


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
        buttons.remove(index);

        if (buttons.size()) {
            if (index < pressed) {
                pressed--;
                setPressed(buttons[pressed]);
            }
            else if (index == pressed) {
                pressed = 0;
                setPressed(buttons[pressed]);
            }
        }
        else pressed = 0;
    }
}


bool ButtonGroup::isInGroup(ActionButton *button) const {
    return (getIndex(button) < buttons.size()); 
}


// ============================================================================


RectButton::RectButton(
    size_t id_, const LayoutBox &layout_,
    ButtonAction *action_,
    const std::string &text_,
    const RectButtonStyle &style_
) :
    ActionButton(id_, layout_, action_),
    text(text_), style(style_)
{}


bool RectButton::isInsideButton(const Vec2d &point, const Vec2d &global_size) const {
    return isInsideRect(Vec2d(), global_size, point);
}


void RectButton::draw(sf::RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());
    
    sf::Text btn_text(text, style.font, style.font_size);
    sf::FloatRect text_rect = btn_text.getLocalBounds();
    Vec2d text_offset((global_size.x - text_rect.width) / 2, (global_size.y - text_rect.height) / 2);

    btn_text.setPosition(global_position + text_offset);

    sf::RectangleShape btn_rect(global_size);

    switch(status) {
        case BUTTON_NORMAL:
            btn_rect.setFillColor(style.normal);
            btn_text.setFillColor(style.font_normal);
            break;
        case BUTTON_HOVER:
            btn_rect.setFillColor(style.hover);
            btn_text.setFillColor(style.font_hover);
            break;
        case BUTTON_PRESSED:
            btn_rect.setFillColor(style.pressed);
            btn_text.setFillColor(style.font_pressed);
            break;
        default: ASSERT(0, "Invalid button status!\n");
    }

    btn_rect.setPosition(global_position);

    result.draw(btn_rect);
    result.draw(btn_text);
}


// ============================================================================


TextureButton::TextureButton(
    size_t id_, const LayoutBox &layout_,
    ButtonAction *action_,
    const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_
) :
    ActionButton(id_, layout_, action_),
    normal(normal_), hover(hover_), pressed(pressed_)
{
    layout->setSize(Vec2d(normal.getSize().x, normal.getSize().y));
}


bool TextureButton::isInsideButton(const Vec2d &point, const Vec2d &global_size) const {
    return isInsideRect(Vec2d(), global_size, point);
}


void TextureButton::draw(sf::RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());

    sf::Sprite btn_sprite;

    switch(status) {
        case BUTTON_NORMAL: btn_sprite.setTexture(normal); break;
        case BUTTON_HOVER: btn_sprite.setTexture(hover); break;
        case BUTTON_PRESSED: btn_sprite.setTexture(pressed); break;
        default: ASSERT(0, "Invalid button status!\n");
    }
    
    btn_sprite.setPosition(global_position);

    result.draw(btn_sprite);
}


TextureIconButton::TextureIconButton(
    size_t id_, const LayoutBox &layout_,
    ButtonAction *action_,
    const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_,
    const sf::Texture &icon_
) : 
    TextureButton(id_, layout_, action_, normal_, hover_, pressed_),
    icon(icon_)
{}


void TextureIconButton::draw(sf::RenderTarget &result, TransformStack &stack) {
    TextureButton::draw(result, stack);

    Vec2d global_position = stack.apply(layout->getPosition());

    sf::Sprite icon_sprite(icon);
    icon_sprite.setPosition(global_position);
    result.draw(icon_sprite);
}
