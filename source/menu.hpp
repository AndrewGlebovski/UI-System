/**
 * \file
 * \brief Contains menu class and protypes of his functions 
*/


/// Contains menu style
class MenuStyle {
public:
    const sf::Font &font;       ///< Font
    unsigned font_size;         ///< Font size
    sf::Color font_normal;      ///< Font color normal
    sf::Color font_hover;       ///< Font color hover
    sf::Color font_pressed;     ///< Font color pressed
    sf::Color normal;           ///< Menu and buttons background color
    sf::Color hover;            ///< Buttons hover color
    sf::Color pressed;          ///< Button color when button is pressed


    MenuStyle(
        const sf::Font &font_,
        unsigned font_size_,
        const sf::Color &font_normal_,
        const sf::Color &font_hover_,
        const sf::Color &font_pressed_,
        const sf::Color &normal_,
        const sf::Color &hover_,
        const sf::Color &pressed_
    ) :
        font(font_), font_size(font_size_),
        font_normal(font_normal_),
        font_hover(font_hover_),
        font_pressed(font_pressed_),
        normal(normal_),
        hover(hover_),
        pressed(pressed_)
    {}
};


/// Drops down list of options
class MenuButton : public RectButton {
protected:
    List<Widget*> buttons;
    bool is_opened;

public:
    MenuButton(
        const Transform &transform_, const Vector2D &size_, Widget *parent_,
        const sf::String &text_, const ButtonStyle &style_,
        const sf::Color &normal_, const sf::Color &hover_, const sf::Color &pressed_
    );


    void addButton(const sf::String &text_, ButtonAction *action_);


    void setOpened(bool is_opened_);


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;


    virtual EVENT_STATUS onMouseMove(const Vector2D &mouse, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;

    virtual ~MenuButton() override;
};


/// Container for menu buttons
class Menu : public Widget {
protected:
    List<MenuButton*> buttons;
    MenuStyle style;
    size_t opened;


    /**
     * \brief True if some menu button is opened
    */
    bool isMenuOpened() const;


    /**
     * \brief Sets menu button as opened
    */
    void openMenu(size_t menu_id);

public:
    Menu(
        size_t id_, const Transform &transform_, int z_index_, Widget *parent_,
        const MenuStyle &style_
    );


    virtual void addMenuButton(const sf::String &text);


    virtual void addButton(size_t menu_id, const sf::String &text, ButtonAction *action);


    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;


    virtual EVENT_STATUS onMouseMove(const Vector2D &mouse, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onParentResize() override;

    virtual ~Menu() override;
};
