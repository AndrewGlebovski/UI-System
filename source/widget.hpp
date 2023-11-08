/**
 * \file
 * \brief Contains widget class and protypes of his functions 
*/


using EVENT_STATUS = bool;              ///< Shows if event was handled or not

const EVENT_STATUS UNHANDLED = false;   ///< Event was not handled (father broadcasting is required)
const EVENT_STATUS HANDLED = true;      ///< Event was handled (father broadcasting is not required)


/// Holds transformation of the widget
struct Transform {
    Vec2d offset;        ///< Offset from parent


    /**
     * \brief Creates transform that does nothing
    */
    Transform();


    /**
     * \brief Creates transform
    */
    Transform(const Vec2d &offset_);


    /**
     * \brief Applies transform to this one
    */
    void apply(const Transform &transform);


    /**
     * \brief Cancels transform
    */
    void cancel(const Transform &transform);


    /**
     * \brief Checks if two transforms are equal 
    */
    bool operator == (const Transform &transform);
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
     * \brief Checks if transform stack is not corrupted
    */
    bool verify() const;


    /**
     * \brief Cancels local transform and pops it from the transform stack
    */
    ~TransformApplier();
};


/// Base class for all widgets
class Widget {
private:
    /**
     * \brief If requested_id != AUTO_ID returns requested_id, otherwise returns unique id
    */
    size_t generateId(size_t requested_id);

protected:
    const size_t id;        ///< Widget ID that can be used for finding this widget in hierarchy
    int status;             ///< Shows parent if some actions requiered


    /**
     * \brief Returns max possible new_size for child
    */
    virtual Vec2d onChildResize(Widget *child, const Vec2d &new_size);


    /**
     * \brief Returns max possible new_transform for child
    */
    virtual Transform onChildTransform(Widget *child, const Transform &new_transform);

public:
    Transform transform;    ///< Widget local transform
    Vec2d size;          ///< Widget size
    int z_index;            ///< Shows order in which widgets are drawn
    Widget *parent;         ///< Parent that holds this widget


    /// Pass into constructor to generate new ID
    static const size_t AUTO_ID = 0;


    /// Shows parent if some actions requiered for this widget
    enum WIDGET_STATUS {
        PASS        = 0,    ///< Nothing to be done for this widget
        DELETE      = 1     ///< This widget should be deleted
    };


    /**
     * \brief Widget constructor
     * \note If id_ != AUTO_ID sets id to id_, otherwise generates "unique" id
    */
    Widget(size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_);


    /**
     * \brief Default copy constructor
    */
    Widget(const Widget&) = default;


    /**
     * \brief Default assignment
    */
    Widget &operator = (const Widget&) = default;


    /**
     * \brief Returns widget id
    */
    size_t getId() const;


    /**
     * \brief Searches widget in hierarchy using id
    */
    virtual Widget *findWidget(size_t widget_id);


    /**
     * \brief Adds child widget for this
     * \warning If widget is not supposed to have children, abort() will be called
    */
    virtual size_t addChild(Widget *child);


    /**
     * \brief Removes child by its id
     * \warning If widget is not supposed to have children, abort() will be called
    */
    virtual void removeChild(size_t child_id);


    /**
     * \brief Returns widget status
    */
    int getStatus() const;


    /**
     * \brief Sets widget status
    */
    void setStatus(WIDGET_STATUS new_status);


    /**
     * \brief Draws red rectangle for debug purposes
    */
    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms);


    /**
     * \brief Sets max possible new_size according to parent
    */
    virtual void tryResize(const Vec2d &new_size);


    /**
     * \brief Sets max possible new_transform according to parent
    */
    virtual void tryTransform(const Transform &new_transform);


    virtual EVENT_STATUS onMouseMove(const Vec2d &mouse, List<Transform> &transforms) { return UNHANDLED; }
    virtual EVENT_STATUS onMouseButtonUp(const Vec2d &mouse, int button_id, List<Transform> &transforms) { return UNHANDLED; }
    virtual EVENT_STATUS onMouseButtonDown(const Vec2d &mouse, int button_id, List<Transform> &transforms) { return UNHANDLED; }
    virtual EVENT_STATUS onKeyUp(int key_id) { return UNHANDLED; }
    virtual EVENT_STATUS onKeyDown(int key_id) { return UNHANDLED; }
    virtual EVENT_STATUS onTimer(float delta_time) { return UNHANDLED; }
    virtual EVENT_STATUS onParentResize() { return UNHANDLED; }
    virtual void checkChildren() {}


    virtual ~Widget() {};
};


/**
 * \brief Checks if point is inside rectangle
*/
bool isInsideRect(Vec2d position, Vec2d size, Vec2d point);
