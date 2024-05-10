/**
 * \file
 * \brief Contains button classes and protypes of their functions 
*/


#ifndef _BUTTONS_H_
#define _BUTTONS_H_


#include "common/asset.hpp"
#include "widget/widget.hpp"


/// Base class for all buttons
class BaseButton : public Widget {
public:
    /**
     * \brief Constructs button
    */
    BaseButton(size_t id_, const plug::LayoutBox &layout_) :
        Widget(id_, layout_) {}

    /**
     * \brief Draws button
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override = 0;
};


/// Base class for all button actions
class ButtonAction {
public:
    /**
     * \brief Does something
    */
    virtual void operator () () = 0;

    /**
     * \brief Clones current action
    */
    virtual ButtonAction *clone() = 0;

    /**
     * \brief Default destructor
    */
    virtual ~ButtonAction() = default;
};


class ButtonGroup;


/// Button that holds action, can be in group and have status
class ActionButton : public BaseButton {
public:
    /// Button status
    enum BUTTON_STATUS {
        BUTTON_NORMAL   = 1,    ///< Normal button status
        BUTTON_HOVER    = 2,    ///< Mouse is hovering over button
        BUTTON_PRESSED  = 3     ///< Button was clicked but haven't been released yet 
    };

    /**
     * \brief Creates new button with action
     * \note If group_ != nullptr then button will join the group_
    */
    ActionButton(
        size_t id_, const plug::LayoutBox &layout_,
        ButtonAction *action_
    );

    /**
     * \note AUTO_ID used for generating id
    */
    ActionButton(const ActionButton &button);

    /**
     * \brief Copies layout, action and joins the same group
    */
    ActionButton &operator = (const ActionButton &button);

    /**
     * \brief Sets button status
    */
    void setButtonStatus(BUTTON_STATUS new_status);

    /**
     * \brief Adds button to group
    */
    void setButtonGroup(ButtonGroup *group_);

    /**
     * \brief Returns button group or nullptr
    */
    ButtonGroup *getButtonGroup();

    /**
     * \note Point position is relative to button position
    */
    virtual bool isInsideButton(const plug::Vec2d &point, const plug::Vec2d &global_size) const = 0;

    /**
     * \brief Draws button
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override = 0;

    /**
     * \brief Delete action if button has one
    */
    virtual ~ActionButton() override;

protected:
    ButtonAction *action;
    ButtonGroup *group;
    int status;

    /**
     * \brief Checks if group != nullptr
    */
    bool isInGroup() const;

    /**
     * \brief Checks if group != nullptr and button is pressed
    */
    bool isPressedInGroup() const;

    virtual void onMouseMove(const plug::MouseMoveEvent &event, plug::EHC &ehc) override;
    virtual void onMousePressed(const plug::MousePressedEvent &event, plug::EHC &ehc) override;
    virtual void onMouseReleased(const plug::MouseReleasedEvent &event, plug::EHC &ehc) override;
};


/// Only one button is always pressed in group
class ButtonGroup {
private:
    List<ActionButton*> buttons;        ///< Buttons in this group
    size_t pressed;                     ///< Currently pressed button

    /**
     * \brief Returns index of the button in buttons
    */
    size_t getIndex(ActionButton *button) const;

public:
    /**
     * \brief Creates empty group
    */
    ButtonGroup();

    /**
     * \brief Set pressed button
    */
    void setPressed(ActionButton *new_pressed);

    /**
     * \brief Get pressed button
    */
    ActionButton *getPressed();

    /**
     * \brief Adds button to this group
    */
    void addButton(ActionButton *new_button);

    /**
     * \brief Removes button from this group
     * \note If active element got removed, first one will become active 
    */
    void removeButton(ActionButton *button);

    /**
     * \brief Checks if the button is in this group
    */
    bool isInGroup(ActionButton *button) const;
};


/// Contains rect button style
struct RectButtonStyle {
    plug::Color normal;           ///< Button pressed color
    plug::Color hover;            ///< Button hover color
    plug::Color pressed;          ///< Button pressed color
    const sf::Font &font;       ///< Font
    unsigned font_size;         ///< Font size
    plug::Color font_normal;      ///< Font color normal
    plug::Color font_hover;       ///< Font color hover
    plug::Color font_pressed;     ///< Font color pressed

    /**
     * \brief Constructs style
    */
    RectButtonStyle(
        plug::Color normal_, plug::Color hover_, plug::Color pressed_,
        const sf::Font &font_, unsigned font_size_,
        plug::Color font_normal_,
        plug::Color font_hover_,
        plug::Color font_pressed_
    ) :
        normal(normal_), hover(hover_), pressed(pressed_),
        font(font_), font_size(font_size_),
        font_normal(font_normal_),
        font_hover(font_hover_),
        font_pressed(font_pressed_)
    {}

    /**
     * \brief Constructs style but font doesn't change color
    */
    RectButtonStyle(
        plug::Color normal_, plug::Color hover_, plug::Color pressed_,
        const sf::Font &font_, unsigned font_size_, plug::Color font_color_
    ) :
        normal(normal_), hover(hover_), pressed(pressed_),
        font(font_), font_size(font_size_),
        font_normal(font_color_),
        font_hover(font_color_),
        font_pressed(font_color_)
    {}
};


/// Contains rect button style
struct SimpleRectButtonStyle {
    plug::Color normal;           ///< Button pressed color
    plug::Color hover;            ///< Button hover color
    plug::Color pressed;          ///< Button pressed color

    /**
     * \brief Constructs style
    */
    SimpleRectButtonStyle(plug::Color normal_, plug::Color hover_, plug::Color pressed_) :
        normal(normal_), hover(hover_), pressed(pressed_) {}
};


/// Rectangle button
class SimpleRectButton : public ActionButton {
protected:
    SimpleRectButtonStyle style;

public:
    SimpleRectButton(
        size_t id_, const plug::LayoutBox &layout_,
        ButtonAction *action_,
        const SimpleRectButtonStyle &style_
    );

    virtual bool isInsideButton(const plug::Vec2d &point, const plug::Vec2d &global_size) const override;

    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;
};


/// Rectangle button with some text
class RectButton : public ActionButton {
protected:
    RectButtonStyle style;
    TextShape text_shape;

public:
    RectButton(
        size_t id_, const plug::LayoutBox &layout_,
        ButtonAction *action_,
        const std::string &text_,
        const RectButtonStyle &style_
    );

    virtual bool isInsideButton(const plug::Vec2d &point, const plug::Vec2d &global_size) const override;

    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;
};


/// plug::Texture button
class TextureButton : public ActionButton {
protected:
    const plug::Texture &normal;
    const plug::Texture &hover;
    const plug::Texture &pressed;

public:
    TextureButton(
        size_t id_, const plug::LayoutBox &layout_,
        ButtonAction *action_,
        const plug::Texture &normal_, const plug::Texture &hover_, const plug::Texture &pressed_
    );

    virtual bool isInsideButton(const plug::Vec2d &point, const plug::Vec2d &global_size) const override;

    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;
};


/// Draws TextureButton with icon in front of it
class TextureIconButton : public TextureButton {
protected:
    const plug::Texture &icon;

public:
    /**
     * \note Icon must be the same size as the button texture
    */
    TextureIconButton(
        size_t id_, const plug::LayoutBox &layout_,
        ButtonAction *action_,
        const plug::Texture &normal_, const plug::Texture &hover_, const plug::Texture &pressed_,
        const plug::Texture &icon_
    );

    /**
     * \brief Draws texture button and icon over it
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;
};


#endif
