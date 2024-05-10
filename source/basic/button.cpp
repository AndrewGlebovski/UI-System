/**
 * \file
 * \brief Contains sources of button classes functions
*/


#include "basic/button.hpp"


// ============================================================================


ActionButton::ActionButton(
    size_t id_, const plug::LayoutBox &layout_,
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


void ActionButton::onMouseMove(const plug::MouseMoveEvent &event, plug::EHC &ehc) {
    plug::Vec2d global_position = ehc.stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(ehc.stack, layout->getSize());
    
    if (isInsideButton(event.pos - global_position, global_size)) {
        if (status != BUTTON_PRESSED)
            setButtonStatus(BUTTON_HOVER);

        ehc.overlapped = true;
        return;
    }
    
    if (!isPressedInGroup()) setButtonStatus(BUTTON_NORMAL);
}


void ActionButton::onMousePressed(const plug::MousePressedEvent &event, plug::EHC &ehc) {
    plug::Vec2d global_position = ehc.stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(ehc.stack, layout->getSize());

    if (!isInsideButton(event.pos - global_position, global_size)) return;

    setButtonStatus(BUTTON_PRESSED);
    if (isInGroup()) group->setPressed(this);

    if (action) (*action)();
    ehc.stopped = true;
}


void ActionButton::onMouseReleased(const plug::MouseReleasedEvent &event, plug::EHC &ehc) {
    plug::Vec2d global_position = ehc.stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(ehc.stack, layout->getSize());

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


SimpleRectButton::SimpleRectButton(
    size_t id_, const plug::LayoutBox &layout_,
    ButtonAction *action_,
    const SimpleRectButtonStyle &style_
) :
    ActionButton(id_, layout_, action_),
    style(style_)
{}


bool SimpleRectButton::isInsideButton(const plug::Vec2d &point, const plug::Vec2d &global_size) const {
    return isInsideRect(plug::Vec2d(), global_size, point);
}


void SimpleRectButton::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
    plug::Vec2d global_position = stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(stack, layout->getSize());

    RectShape btn_rect(global_position, global_size, plug::Color());

    switch(status) {
        case BUTTON_NORMAL:
            btn_rect.setColor(style.normal);
            break;
        case BUTTON_HOVER:
            btn_rect.setColor(style.hover);
            break;
        case BUTTON_PRESSED:
            btn_rect.setColor(style.pressed);
            break;
        default: ASSERT(0, "Invalid button status!\n");
    }

    btn_rect.draw(result);
}


// ============================================================================


RectButton::RectButton(
    size_t id_, const plug::LayoutBox &layout_,
    ButtonAction *action_,
    const std::string &text_,
    const RectButtonStyle &style_
) :
    ActionButton(id_, layout_, action_),
    style(style_),
    text_shape(sf::Text(text_, style.font, style.font_size))
{}


bool RectButton::isInsideButton(const plug::Vec2d &point, const plug::Vec2d &global_size) const {
    return isInsideRect(plug::Vec2d(), global_size, point);
}


void RectButton::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
    plug::Vec2d global_position = stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(stack, layout->getSize());

    RectShape btn_rect(global_position, global_size, plug::Color());

    switch(status) {
        case BUTTON_NORMAL:
            btn_rect.setColor(style.normal);
            text_shape.setColor(style.font_normal);
            break;
        case BUTTON_HOVER:
            btn_rect.setColor(style.hover);
            text_shape.setColor(style.font_hover);
            break;
        case BUTTON_PRESSED:
            btn_rect.setColor(style.pressed);
            text_shape.setColor(style.font_pressed);
            break;
        default: ASSERT(0, "Invalid button status!\n");
    }

    btn_rect.draw(result);

    plug::Vec2d text_size = applySize(stack, text_shape.getTextureSize());

    text_shape.draw(
        result,
        global_position + (global_size - text_size) / 2,
        text_size
    );
}


// ============================================================================


TextureButton::TextureButton(
    size_t id_, const plug::LayoutBox &layout_,
    ButtonAction *action_,
    const plug::Texture &normal_, const plug::Texture &hover_, const plug::Texture &pressed_
) :
    ActionButton(id_, layout_, action_),
    normal(normal_), hover(hover_), pressed(pressed_)
{
    layout->setSize(plug::Vec2d(normal.width, normal.height));
}


bool TextureButton::isInsideButton(const plug::Vec2d &point, const plug::Vec2d &global_size) const {
    return isInsideRect(plug::Vec2d(), global_size, point);
}


void TextureButton::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
    plug::Vec2d global_position = stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(stack, layout->getSize());

    switch(status) {
        case BUTTON_NORMAL:
            TextureShape(normal).draw(result, global_position, global_size);
            break;
        
        case BUTTON_HOVER:
            TextureShape(hover).draw(result, global_position, global_size);
            break;
        
        case BUTTON_PRESSED:
            TextureShape(pressed).draw(result, global_position, global_size);
            break;
        
        default:
            ASSERT(0, "Invalid button status!\n");
    }
}


TextureIconButton::TextureIconButton(
    size_t id_, const plug::LayoutBox &layout_,
    ButtonAction *action_,
    const plug::Texture &normal_, const plug::Texture &hover_, const plug::Texture &pressed_,
    const plug::Texture &icon_
) : 
    TextureButton(id_, layout_, action_, normal_, hover_, pressed_),
    icon(icon_)
{}


void TextureIconButton::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
    TextureButton::draw(stack, result);

    plug::Vec2d global_position = stack.apply(layout->getPosition());
    plug::Vec2d global_size = applySize(stack, layout->getSize());

    TextureShape(icon).draw(result, global_position, global_size);
}
