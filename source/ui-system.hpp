/**
 * \file
 * \brief Contains UI classes and protypes of their functions 
*/


/// Manage UI elements
class Container : public BaseUI {
private:
    List<BaseUI*> elements;         ///< List of UI elements sorted by z-index
    size_t focused;

public:
    Container(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_);


    /**
     * \brief Adds new UI element to manager
     * \warning Elements should be allocated using new and will be deleted by manager
    */
    void addElement(BaseUI *control);


    /**
     * \brief Draws elements under his management
    */
    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onKeyUp(int key_id) override;
    virtual int onKeyDown(int key_id) override;
    virtual int onTimer(float delta_time) override;


    ~Container();
};


/// Frame with UI elements inside
class Window : public BaseUI {
protected:
    sf::String title;           ///< Window title
    WindowStyle style;          ///< Window style
    Container buttons;          ///< Window title bar and resize buttons
    Container container;        ///< Window content manager


    /**
     * \brief Draws window frame
    */
    void drawFrame(sf::RenderTexture &result, const Vector2D &total_position);

public:
    /**
     * \brief Contstructs window
     * \note Position and size consider title bar and frame
    */
    Window(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_, 
        const sf::String &title_, const WindowStyle &style_
    );


    /**
     * \brief Returns position of the window inside area
    */
    Vector2D getAreaPosition() const;


    /**
     * \brief Returns position of the window inside area
    */
    Vector2D getAreaSize() const;


    /**
     * \brief Adds new UI element to window
     * \warning Elements should be allocated using new and will be deleted by manager
    */
    void addElement(BaseUI *control);


    /**
     * \brief Draws window frame, title bar and its content
    */
    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onKeyUp(int key_id) override;
    virtual int onKeyDown(int key_id) override;
    virtual int onTimer(float delta_time) override;
};


/// MainWindow of the application
class MainWindow : public Window {
public:
    MainWindow(
        const Vector2D &position_, const Vector2D &size_, int z_index_,
        const sf::String &title_, const WindowStyle &style_
    );


    virtual void setSize(const Vector2D &new_size) override;


    virtual void setPosition(const Vector2D &new_position) override;


    /**
     * \brief Parses SFML event into my own event system
    */
    void parseEvent(const sf::Event &event, List<Vector2D> &transforms);
};


/// Invisible button for moving windows
class MoveButton : public BaseButton {
protected:
    Window &window;         ///< Window to move
    Vector2D prev_mouse;    ///< Previous mouse click position
    bool is_moving;         ///< If moving is active

public:
    MoveButton(
        const Vector2D &position_, const Vector2D &size_, BaseUI *parent_, 
        Window &window_
    );


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
};
