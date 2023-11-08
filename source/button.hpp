/**
 * \file
 * \brief Contains button classes and protypes of their functions 
*/


/// Base class for all buttons
class BaseButton : public Widget {
public:
    BaseButton(size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_) :
        Widget(id_, transform_, size_, z_index_, parent_) {}


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override = 0;


    virtual ~BaseButton() = default;
};


/// Base class for all button actions
class ButtonAction {
public:
    ButtonAction() = default;


    /**
     * \brief Does something
    */
    virtual void operator () () = 0;


    /**
     * \brief Clones current action
    */
    virtual ButtonAction *clone() = 0;


    virtual ~ButtonAction() = default;
};


class ButtonGroup;


/// Button that holds action, can be in group and have status
class ActionButton : public BaseButton {
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
        size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_,
        ButtonAction *action_, ButtonGroup *group_
    );


    /**
     * \note AUTO_ID used for generating id
    */
    ActionButton(const ActionButton &button);


    ActionButton &operator = (const ActionButton &button);


    /**
     * \brief Sets button status
    */
    void setButtonStatus(BUTTON_STATUS new_status);


    /**
     * \note Point position is relative to button position
    */
    virtual bool isInsideButton(const Vec2d &point) = 0;


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override = 0;


    virtual EVENT_STATUS onMouseMove(const Vec2d &mouse, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vec2d &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vec2d &mouse, int button_id, List<Transform> &transforms) override;


    virtual ~ActionButton() override;
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
    */
    void removeButton(ActionButton *button);


    /**
     * \brief Checks if the button is in this group
    */
    bool isInGroup(ActionButton *button) const;
};


/// Contains rect button style
class ButtonStyle {
public:
    const sf::Font &font;       ///< Font
    unsigned font_size;         ///< Font size
    sf::Color font_normal;      ///< Font color normal
    sf::Color font_hover;       ///< Font color hover
    sf::Color font_pressed;     ///< Font color pressed


    ButtonStyle(
        const sf::Font &font_,
        unsigned font_size_,
        const sf::Color &font_normal_,
        const sf::Color &font_hover_,
        const sf::Color &font_pressed_
    ) :
        font(font_), font_size(font_size_),
        font_normal(font_normal_),
        font_hover(font_hover_),
        font_pressed(font_pressed_)
    {}
};


/// Rectangle button with some text
class RectButton : public ActionButton {
protected:
    sf::String text;
    ButtonStyle style;
    sf::Color normal_color;
    sf::Color hover_color;
    sf::Color pressed_color;

public:
    RectButton(
        size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_,
        ButtonAction *action_, ButtonGroup *group_,
        const sf::String &text_, const ButtonStyle &style_,
        const sf::Color &normal_, const sf::Color &hover_, const sf::Color &pressed_
    );


    virtual bool isInsideButton(const Vec2d &point) override;


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;
};


/// Texture button
class TextureButton : public ActionButton {
protected:
    const sf::Texture &normal;
    const sf::Texture &hover;
    const sf::Texture &pressed;

public:
    TextureButton(
        size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
        ButtonAction *action_, ButtonGroup *group_,
        const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_
    );


    virtual bool isInsideButton(const Vec2d &point) override;


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;
};


/// Draws TextureButton with icon in front of it
class TextureIconButton : public TextureButton {
protected:
    const sf::Texture &icon;

public:
    /**
     * \note Icon must be the same size as the button texture
    */
    TextureIconButton(
        size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
        ButtonAction *action_, ButtonGroup *group_,
        const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_,
        const sf::Texture &icon_
    );


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;
};
