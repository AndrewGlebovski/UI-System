/**
 * \file
 * \brief Contains UI classes and protypes of their functions 
*/


/// Base class for all UI elements
class BaseUI {
public:
    Vector2D position;      ///< Element position relative to its parent
    int z;                  ///< Shows order in which UI elements are drawn
    BaseUI &parent;         ///< Parent that holds this UI element


    /// Shows if event was handled or not
    enum EVENT_STATUS {
        UNHANDLED   = 0,    ///< Event was not handled (father broadcasting is required)
        HANDLED     = 1     ///< Event was handled (father broadcasting is not required)
    };


    BaseUI(Vector2D pos, int z_, BaseUI &parent_) :
        position(pos), z(z_), parent(parent_) {}


    /**
     * \brief Draws red rectangle for debug purposes
    */
    virtual void draw(sf::RenderTexture &result) {
        sf::RectangleShape rect(Vector2D(25, 25));
        rect.setFillColor(sf::Color::Red);
        rect.setPosition(position);
        result.draw(rect);
    }


    /**
     * \brief Returns UI element absolute position on screen
    */
    virtual Vector2D getAbsolutePosition() const {
        return position + parent.getAbsolutePosition();
    }


    virtual int onMouseMove(int mouse_x, int mouse_y) { return UNHANDLED; }
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id) { return UNHANDLED; }
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id) { return UNHANDLED; }
    virtual int onKeyUp(int key_id) { return UNHANDLED; }
    virtual int onKeyDown(int key_id) { return UNHANDLED; }
    virtual int onTimer(float delta_time) { return UNHANDLED; }


    virtual ~BaseUI() {};
};


/// Manage UI elements
class Container : public BaseUI {
private:
    List<BaseUI*> elements;         ///< List of UI elements sorted by z-index

public:
    Vector2D size;                  ///< Container size


    Container(const Vector2D &pos, int z_, BaseUI &parent_, const Vector2D &size_);


    /**
     * \brief Adds new UI element to manager
     * \warning Elements should be allocated using new and will be deleted by manager
    */
    void addElement(BaseUI *control);


    /**
     * \brief Draws elements under his management
    */
    virtual void draw(sf::RenderTexture &result) override;


    virtual int onMouseMove(int mouse_x, int mouse_y) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id) override;
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
    void draw_frame(sf::RenderTexture &result);

public:
    Window(const Vector2D &pos, int z_, BaseUI &parent_, const Vector2D &size_, const sf::String &title_, const WindowStyle &style_);


    /**
     * \brief Adds new UI element to window
     * \warning Elements should be allocated using new and will be deleted by manager
    */
    void addElement(BaseUI *control);


    /**
     * \brief Draws window frame, title bar and its content
    */
    virtual void draw(sf::RenderTexture &result) override;


    virtual int onMouseMove(int mouse_x, int mouse_y) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id) override;
    virtual int onKeyUp(int key_id) override;
    virtual int onKeyDown(int key_id) override;
    virtual int onTimer(float delta_time) override;
};


/// MainWindow of the application
class MainWindow : public Window {
public:
    MainWindow(const Vector2D &pos, int z_, const Vector2D &size_, const sf::String &title_, const WindowStyle &style_) :
        Window(pos, z_, *this, size_, title_, style_) {}
    

    virtual Vector2D getAbsolutePosition() const {
        return position;
    }
};


/**
 * \brief Checks if point is inside rectangle
*/
bool isInsideRect(Vector2D position, Vector2D size, Vector2D point);


/**
 * \brief Parses SFML event into my own event system
*/
void parseEvent(const sf::Event &event, Window &window);
