/**
 * \file
 * \brief Contains UI classes and protypes of their functions 
*/


/// Base class for all UI elements
class BaseUI {
public:
    Vector2D position;      ///< Element position relative to its parent
    Vector2D size;          ///< UI element size
    int z_index;            ///< Shows order in which UI elements are drawn
    BaseUI &parent;         ///< Parent that holds this UI element


    /// Shows if event was handled or not
    enum EVENT_STATUS {
        UNHANDLED   = 0,    ///< Event was not handled (father broadcasting is required)
        HANDLED     = 1     ///< Event was handled (father broadcasting is not required)
    };


    /**
     * \brief Standart constructor
    */
    BaseUI(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI &parent_);


    /**
     * \brief Draws red rectangle for debug purposes
    */
    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms);


    /**
     * \brief Resizes element according to parent area
    */
    virtual void setSize(const Vector2D &new_size);


    /**
     * \brief Sets element to new position according to parent area
    */
    virtual void setPosition(const Vector2D &new_position);


    void apply_local_transform(List<Vector2D> &transforms) const;


    void cancel_local_transform(List<Vector2D> &transforms) const;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) { return UNHANDLED; }
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) { return UNHANDLED; }
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) { return UNHANDLED; }
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
    Container(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI &parent_);


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
    void drawFrame(sf::RenderTexture &result, Vector2D total_position);

public:
    /**
     * \brief Contstructs window
     * \note Position and size consider title bar and frame
    */
    Window(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI &parent_, 
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
};


/**
 * \brief Checks if point is inside rectangle
*/
bool isInsideRect(Vector2D position, Vector2D size, Vector2D point);


/**
 * \brief Parses SFML event into my own event system
*/
void parseEvent(const sf::Event &event, Window &window, List<Vector2D> &transforms);
