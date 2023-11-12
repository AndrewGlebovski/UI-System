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
    List<Widget*> buttons;      ///< Option buttons
    bool is_opened;             ///< Shows if menu option list is visible

public:
    MenuButton(
        size_t id_, const LayoutBox &layout_,
        const std::string &text_, const ButtonStyle &style_,
        const sf::Color &normal_, const sf::Color &hover_, const sf::Color &pressed_
    );

    /**
     * \brief Adds another option button to menu
    */
    void addButton(const std::string &text_, ButtonAction *action_);

    /**
     * \brief Opens or closes list of option buttons
    */
    void setOpened(bool is_opened_);

    /**
     * \brief Draws menu button and list option buttons if it's open
    */
    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override;

    virtual EVENT_STATUS onMouseMove(const Vec2d &mouse, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vec2d &mouse, int button_id, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vec2d &mouse, int button_id, TransformStack &stack) override;

    /**
     * \brief Deletes option buttons
    */
    virtual ~MenuButton() override;
};


/// Container for menu buttons
class Menu : public Widget {
protected:
    List<MenuButton*> buttons;      ///< Menu buttons
    MenuStyle style;                ///< Menu style
    size_t opened;                  ///< Index of opened menu button

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
        size_t id_, const LayoutBox &layout_,
        const MenuStyle &style_
    );

    /**
     * \brief Adds menu button
    */
    virtual void addMenuButton(const std::string &text);

    /**
     * \brief Adds option button to menu button
    */
    virtual void addButton(size_t menu_id, const std::string &text, ButtonAction *action);

    /**
     * \brief Draws menu rect and menu buttons
    */
    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override;

    virtual EVENT_STATUS onMouseMove(const Vec2d &mouse, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vec2d &mouse, int button_id, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vec2d &mouse, int button_id, TransformStack &stack) override;

    /**
     * \brief Deletes menu buttons
    */
    virtual ~Menu() override;
};
