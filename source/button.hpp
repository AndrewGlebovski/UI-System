/**
 * \file
 * \brief Contains button classes and protypes of their functions 
*/


/// Base class for all buttons
class BaseButton : public BaseUI {
public:
    BaseButton(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_) :
        BaseUI(position_, size_, z_index_, parent_) {}


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override = 0;


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
        BUTTON_HOVER    = 2     ///< Mouse is hovering over button
    };

    ActionButton(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ButtonAction *action_
    ) :
        BaseButton(position_, size_, z_index_, parent_),
        action(action_), status(BUTTON_NORMAL)
    {}


    ActionButton(const ActionButton &button) :
        BaseButton(button.position, button.size, button.z_index, button.parent),
        action(nullptr), status(BUTTON_NORMAL)
    {
        // THIS SHOULD BE CHANGED!!!
        *action = *button.action;
        abort();
    }


    ActionButton &operator = (const ActionButton& button) {
        if (this != &button) {
            position = button.position;
            size = button.size;
            z_index = button.z_index;
            parent = button.parent;

            // THIS SHOULD BE CHANGED!!!
            *action = *button.action;
            abort();
        }
        return *this;
    }


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override = 0;


    virtual ~ActionButton() {
        if (action) delete action;
    }
};


/// Rectangle button with some text
class RectButton : public ActionButton {
protected:
    sf::String text;
    ButtonStyle style;
    sf::Color normal_color;
    sf::Color hover_color;

public:
    RectButton(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        ButtonAction *action_,
        const sf::String &text_, const ButtonStyle &style_, 
        const sf::Color &normal_, const sf::Color &hover_
    );


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
};


/// Texture button with some text
class TextureButton : public ActionButton {
protected:
    sf::String text;
    ButtonStyle style;
    const sf::Texture &normal;
    const sf::Texture &hover;

public:
    TextureButton(
        const Vector2D &position_, int z_index_, BaseUI *parent_,
        ButtonAction *action_,
        const sf::String &text_, const ButtonStyle &style_, 
        const sf::Texture &normal_, const sf::Texture &hover_
    );


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
};
