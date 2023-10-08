/**
 * \file
 * \brief Contains base UI class and protypes of his functions 
*/


/// Base class for all UI elements
class BaseUI {
public:
    Vector2D position;      ///< Element position relative to its parent
    Vector2D size;          ///< UI element size
    int z_index;            ///< Shows order in which UI elements are drawn
    BaseUI *parent;         ///< Parent that holds this UI element


    /// Shows if event was handled or not
    enum EVENT_STATUS {
        UNHANDLED   = 0,    ///< Event was not handled (father broadcasting is required)
        HANDLED     = 1     ///< Event was handled (father broadcasting is not required)
    };


    /**
     * \brief Standart constructor
    */
    BaseUI(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_);


    /**
     * \brief Default copy constructor
    */
    BaseUI(const BaseUI&) = default;


    /**
     * \brief Default assignment
    */
    BaseUI &operator = (const BaseUI&) = default;


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


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) { return UNHANDLED; }
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) { return UNHANDLED; }
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) { return UNHANDLED; }
    virtual int onKeyUp(int key_id) { return UNHANDLED; }
    virtual int onKeyDown(int key_id) { return UNHANDLED; }
    virtual int onTimer(float delta_time) { return UNHANDLED; }


    virtual ~BaseUI() {};
};


/// Tool class for temporarily applying local transform
class TransformApplier {
private:
    List<Vector2D> &transforms;     ///< Transform stack

public:
    /**
     * \brief Applies local transform and pushes it to the transform stack
    */
    TransformApplier(List<Vector2D> &transforms_, const Vector2D &local_transform);


    /**
     * \brief Cancels local transform and pops it from the transform stack
    */
    ~TransformApplier();
};
