/**
 * \file
 * \brief Contains base UI class and protypes of his functions 
*/


/// Holds transformation of the UI element
struct Transform {
    Vector2D offset;        ///< Offset from parent


    /**
     * \brief Creates transform that does nothing
    */
    Transform();


    /**
     * \brief Creates transform
    */
    Transform(const Vector2D &offset_);


    /**
     * \brief Applies transform to this one
    */
    void apply(const Transform &transform);


    /**
     * \brief Cancels transform
    */
    void cancel(const Transform &transform);
};


/// Tool class for temporarily applying local transform
class TransformApplier {
private:
    List<Transform> &transforms;     ///< Transform stack

public:
    /**
     * \brief Applies local transform and pushes it to the transform stack
    */
    TransformApplier(List<Transform> &transforms_, const Transform &local_transform);


    /**
     * \brief Cancels local transform and pops it from the transform stack
    */
    ~TransformApplier();
};


/// Base class for all UI elements
class BaseUI {
private:
    /**
     * \brief If requested_id != AUTO_ID returns requested_id, otherwise returns unique id
    */
    size_t generateId(size_t requested_id);

public:
    const size_t id;        ///< Element ID that can be used for finding this element in hierarchy
    Transform transform;    ///< Element local transform
    Vector2D size;          ///< UI element size
    int z_index;            ///< Shows order in which UI elements are drawn
    BaseUI *parent;         ///< Parent that holds this UI element


    /// Pass into constructor to generate new ID
    static const size_t AUTO_ID = 0;


    /// Shows if event was handled or not
    enum EVENT_STATUS {
        UNHANDLED   = 0,    ///< Event was not handled (father broadcasting is required)
        HANDLED     = 1     ///< Event was handled (father broadcasting is not required)
    };


    /**
     * \brief UI element constructor
     * \note If id_ != AUTO_ID sets id to id_, otherwise generates "unique" id
    */
    BaseUI(size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_);


    /**
     * \brief Default copy constructor
    */
    BaseUI(const BaseUI&) = default;


    /**
     * \brief Default assignment
    */
    BaseUI &operator = (const BaseUI&) = default;


    /**
     * \brief Returns UI element id
    */
    size_t getId() const;


    /**
     * \brief Draws red rectangle for debug purposes
    */
    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms);


    /**
     * \brief Resizes element according to parent area
    */
    virtual void setSize(const Vector2D &new_size);


    /**
     * \brief Sets element to new position according to parent area
    */
    virtual void setPosition(const Vector2D &new_position);


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) { return UNHANDLED; }
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) { return UNHANDLED; }
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) { return UNHANDLED; }
    virtual int onKeyUp(int key_id) { return UNHANDLED; }
    virtual int onKeyDown(int key_id) { return UNHANDLED; }
    virtual int onTimer(float delta_time) { return UNHANDLED; }
    virtual int onParentResize() { return UNHANDLED; } ;


    virtual ~BaseUI() {};
};


/**
 * \brief Checks if point is inside rectangle
*/
bool isInsideRect(Vector2D position, Vector2D size, Vector2D point);
