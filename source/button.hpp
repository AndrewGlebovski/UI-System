/**
 * \file
 * \brief Contains button classes and protypes of their functions 
*/


/// Base class for all buttons
class BaseButton : public BaseUI {
public:
    BaseButton(size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_) :
        BaseUI(id_, transform_, size_, z_index_, parent_) {}


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override = 0;


    virtual ~BaseButton() = default;
};


/// Base class for all button actions
class ButtonAction {
public:
    /**
     * \brief Does something
    */
    virtual void operator () () = 0;


    /**
     * \brief Destroyes action
    */
    virtual ~ButtonAction() = default;
};


/// Button that holds action and have status
class ActionButton : public BaseButton {
protected:
    ButtonAction *action;
    int status;

public:
    /// Button status
    enum BUTTON_STATUS {
        BUTTON_NORMAL   = 1,    ///< Normal button status
        BUTTON_HOVER    = 2,    ///< Mouse is hovering over button
        BUTTON_PRESSED  = 3     ///< Button was clicked but haven't been released yet 
    };

    ActionButton(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ButtonAction *action_
    ) :
        BaseButton(id_, transform_, size_, z_index_, parent_),
        action(action_), status(BUTTON_NORMAL)
    {}


    /**
     * \note Point position is relative to button position
    */
    virtual bool isInsideButton(const Vector2D &point) = 0;


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override = 0;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;


    virtual ~ActionButton() override;
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
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ButtonAction *action_,
        const sf::String &text_, const ButtonStyle &style_,
        const sf::Color &normal_, const sf::Color &hover_, const sf::Color &pressed_
    );


    virtual bool isInsideButton(const Vector2D &point) override;


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;
};


/// Texture button
class TextureButton : public ActionButton {
protected:
    const sf::Texture &normal;
    const sf::Texture &hover;
    const sf::Texture &pressed;

public:
    TextureButton(
        size_t id_, const Transform &transform_, int z_index_, BaseUI *parent_,
        ButtonAction *action_,
        const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_
    );


    virtual bool isInsideButton(const Vector2D &point) override;


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;
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
        size_t id_, const Transform &transform_, int z_index_, BaseUI *parent_,
        ButtonAction *action_,
        const sf::Texture &normal_, const sf::Texture &hover_, const sf::Texture &pressed_,
        const sf::Texture &icon_
    );


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;
};
