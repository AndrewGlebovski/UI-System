/**
 * \file
 * \brief Contains window class and protypes of their functions 
*/


/// Controls how window looks
class WindowStyle {
public:
    sf::Color title_color;      ///< Title color
    Vector2D title_offset;      ///< Title offset from top-left corner
    unsigned font_size;         ///< Title font size
    const sf::Font &font;       ///< Title font
    const WindowAsset &asset;   ///< Textures used in window
    float outline;              ///< Describes size of window resizing buttons
    Vector2D tl_offset;         ///< Window inner area offset from top-left corner
    Vector2D br_offset;         ///< Window inner area offset from bottom-right corner


    WindowStyle(
        const sf::Color &title_color_, const Vector2D &title_offset_,
        unsigned font_size_, const sf::Font &font_,
        const WindowAsset &asset_, float outline_, 
        const Vector2D &tl_offset_, const Vector2D &br_offset_
    ) :
        title_color(title_color_), title_offset(title_offset_),
        font_size(font_size_), font(font_),
        asset(asset_), outline(outline_),
        tl_offset(tl_offset_), br_offset(br_offset_)
    {}
};


/// Frame with widgets inside
class Window : public Widget {
protected:
    sf::String title;           ///< Window title
    WindowStyle style;          ///< Window style
    Container buttons;          ///< Window title bar and resize buttons
    Container container;        ///< Window content manager
    Menu *menu;                 ///< Window menu

private:
    /**
     * \brief Adds buttons for managing window
    */
    void addButtons(bool can_resize, bool can_move, bool can_close);

    /**
     * \brief Adds close button to window
    */
    void addCloseButton();

    /**
     * \brief Adds expand button to window
    */
    void addExpandButton();

    /**
     * \brief Adds move button to window
    */
    void addMoveButton();

    /**
     * \brief Adds resize buttons to window
    */
    void addResizeButtons();

public:
    /**
     * \brief Contstructs window
     * \note Position and size consider title bar and frame
    */
    Window(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
        const sf::String &title_,
        const WindowStyle &style_,
        bool can_resize = true,
        bool can_move = true,
        bool can_close = true
    );


    Window(const Window &window) = delete;
    Window &operator = (const Window &window) = delete;


    /**
     * \brief Returns position of the window inside area
    */
    Vector2D getAreaPosition() const;


    /**
     * \brief Returns position of the window inside area
    */
    Vector2D getAreaSize() const;


    /**
     * \brief Adds menu to window or replaces existing one
     * \warning Menu must be allocated using new and will be deleted by window
    */
    void setMenu(Menu *menu_);


    /**
     * \brief Returns pointer to menu
    */
    Menu *getMenu();


    /**
     * \brief Finds pointer to widget inside window container
    */
    virtual Widget *findWidget(size_t widget_id) override;


    /**
     * \brief Adds new widget to window container
     * \warning Widgets should be allocated using new and will be deleted by window
    */
    virtual size_t addChild(Widget *child) override;


    virtual void removeChild(size_t child_id) override;


    virtual void tryResize(const Vector2D &new_size) override;


    const WindowStyle &getStyle() const { return style; }


    /**
     * \brief Draws window frame, title bar and its content
    */
    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;


    virtual EVENT_STATUS onMouseMove(const Vector2D &mouse, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onKeyUp(int key_id) override;
    virtual EVENT_STATUS onKeyDown(int key_id) override;
    virtual EVENT_STATUS onTimer(float delta_time) override;
    virtual EVENT_STATUS onParentResize() override;
    virtual void checkChildren() override;

    virtual ~Window() override;
};


/// MainWindow of the application
class MainWindow : public Window {
public:
    MainWindow(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_,
        const sf::String &title_, const WindowStyle &style_
    );


    virtual void tryResize(const Vector2D &new_size) override;


    virtual void tryTransform(const Transform &new_transform) override;


    /**
     * \brief Parses SFML event into my own event system
    */
    void parseEvent(const sf::Event &event, List<Transform> &transforms);
};
