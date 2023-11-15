/**
 * \file
 * \brief Contains widget class and protypes of his functions 
*/


/// Holds transformation
class Transform {
private:
    Vec2d offset;        ///< Offset from parent
    Vec2d scale;         ///< Scale

public:
    /**
     * \brief Creates transform that does nothing
    */
    Transform();

    /**
     * \brief Creates transform
    */
    Transform(const Vec2d &offset_, const Vec2d &scale_ = Vec2d(1, 1));

    /**
     * \brief Returns offset
    */
    Vec2d getOffset() const;

    /**
     * \brief Sets offset
    */
    void setOffset(const Vec2d &offset_);

    /**
     * \brief Returns scale
    */
    Vec2d getScale() const;

    /**
     * \brief Sets scale
    */
    void setScale(const Vec2d &scale_);

    /**
     * \brief Applies transform to position
    */
    Vec2d apply(const Vec2d &vec) const;

    /**
     * \brief Restores position using transform
    */
    Vec2d restore(const Vec2d &vec) const;
};


/// Contains stack of transforms and final transform at the top of it
class TransformStack {
private:
    List<Transform> transforms;     ///< Stack of transforms

public:
    /**
     * \brief Init transform stack with transform that does nothing
    */
    TransformStack();

    /**
     * \brief Pushes last transform to stack and applies it to the stack top
    */
    void enter(const Transform &transform);

    /**
     * \brief Pops last transform from stack
    */
    void leave();

    /**
     * \brief Returns value of the stack first element
    */
    Transform top() const;

    /**
     * \brief Applies final transform to position
    */
    Vec2d apply(const Vec2d &vec) const;

    /**
     * \brief Restore position using final transform
    */
    Vec2d restore(const Vec2d &vec) const;

    /**
     * \brief Applies final transform to size
    */
    Vec2d apply_size(const Vec2d &vec) const;
};


/// Tool class for temporarily applying local transform
class TransformApplier {
private:
    TransformStack &stack;     ///< Transform stack

public:
    /**
     * \brief Applies local transform and pushes it to the transform stack
    */
    TransformApplier(TransformStack &stack_, const Transform &transform);

    /**
     * \brief Cancels local transform and pops it from the transform stack
    */
    ~TransformApplier();
};


/// Contains widget position and size
class LayoutBox {
public:
    /**
     * \brief Returns widget position
    */
    virtual Vec2d getPosition() const = 0;

    /**
     * \brief Sets widget position
     * \return True if position changed, false otherwise
    */
    virtual bool setPosition(const Vec2d& position_) = 0;

    /**
     * \brief Returns widget size
    */
    virtual Vec2d getSize() const = 0;

    /**
     * \brief Sets widget size
     * \return True if size changed, false otherwise
    */
    virtual bool setSize(const Vec2d& size_) = 0;

    /**
     * \brief Changes its position and size according to parent
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) = 0;

    /**
     * \brief Returns copy of this layout box
     * \warning Do not forget to delete it
    */
    virtual LayoutBox *clone() const = 0;

    /**
     * \brief Default destructor
    */
    virtual ~LayoutBox() = default;
};


/// Simply stores given position and size
class LazyLayoutBox : public LayoutBox {
public:
    LazyLayoutBox();

    LazyLayoutBox(const Vec2d &position_, const Vec2d &size_);
    
    virtual Vec2d getPosition() const override;

    virtual bool setPosition(const Vec2d& position_) override;

    virtual Vec2d getSize() const override;

    virtual bool setSize(const Vec2d& size_) override;

    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    virtual LayoutBox *clone() const override;

protected:
    Vec2d position;         ///< Widget position
    Vec2d size;             ///< Widget size
};


/// Respects parent's size and can't go outside of it
class BoundLayoutBox : public LazyLayoutBox {
public:
    /**
     * \brief Zero position and size
     * \note Bound is set to SCREEN SIZE
    */
    BoundLayoutBox();

    /**
     * \brief Sets position and size
     * \note Bound is set to SCREEN SIZE
    */
    BoundLayoutBox(const Vec2d &position_, const Vec2d &size_);

    /**
     * \brief Sets position according to bound
    */
    virtual bool setPosition(const Vec2d& position_) override;

    /**
     * \brief Sets size according to bound
    */
    virtual bool setSize(const Vec2d& size_) override;

    /**
     * \brief Updates bound and changes size and position according to new bound
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    /**
     * \brief Clones position and size but not bounds
    */
    virtual LayoutBox *clone() const override;

protected:
    Vec2d bound;           ///< Parent size
};


/// Anchor implementation
class AnchorLayoutBox : public LazyLayoutBox {
public:
    /**
     * \brief Sets anchor
     * \note One unit is one pixel, max size is Vec2d(SCREEN_W, SCREEN_H)
    */
    AnchorLayoutBox(
        const Vec2d &offset_, const Vec2d &size_,
        const Vec2d &anchor_position_, const Vec2d &anchor_size_
    );

    /**
     * \brief Sets widget offset from anchor
    */
    virtual bool setPosition(const Vec2d& offset_) override;

    /**
     * \brief Sets widget basic size
    */
    virtual bool setSize(const Vec2d& basic_size_) override;

    /**
     * \brief Updates widget position and size
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    /**
     * \brief Copies all information
    */
    virtual LayoutBox *clone() const override;

protected:
    Vec2d offset;               ///< Widget offset from anchor top-left corner
    Vec2d basic_size;           ///< Widget basic size (without scaling)
    Vec2d anchor_position;      ///< Anchor's top-left corner position
    Vec2d anchor_size;          ///< Defines anchor's bottom-right corner position
};


/// Base class for all widgets
class Widget {
public:
    struct EHC {
        TransformStack &stack;
        bool stopped;
        bool overlapped;

        EHC(TransformStack &stack_, bool stopped_, bool overlapped_) :
            stack(stack_), stopped(stopped_), overlapped(overlapped_) {}
    };

    enum EventType {
        Tick,
        MouseMove,
        MousePressed,
        MouseReleased,
        KeyboardPressed,
        KeyboardReleased,
    };

    class Event {
    private:
        const size_t type;
    public:
        Event (size_t type_) : type(type_) {}

        size_t getType() const { return type; }
    };

    struct TickEvent : public Event {
        double delta_time;

        TickEvent(double delta_time_) :
            Event(Tick), delta_time(delta_time_) {}
    };

    struct MouseMoveEvent : public Event {
        bool shift, ctrl, alt;
        Vec2d pos;

        MouseMoveEvent(bool shift_, bool ctrl_, bool alt_, const Vec2d &pos_) :
            Event(MouseMove), shift(shift_), ctrl(ctrl_), alt(alt_), pos(pos_) {}
    };

    struct MousePressedEvent : public Event {
        MOUSE_BUTTON_ID button_id;
        bool shift, ctrl, alt;
        Vec2d pos;

        MousePressedEvent(MOUSE_BUTTON_ID button_id_, bool shift_, bool ctrl_, bool alt_, const Vec2d &pos_) :
            Event(MousePressed), button_id(button_id_), shift(shift_), ctrl(ctrl_), alt(alt_), pos(pos_) {}
    };

    struct MouseReleasedEvent : public Event {
        MOUSE_BUTTON_ID button_id;
        bool shift, ctrl, alt;
        Vec2d pos;

        MouseReleasedEvent(MOUSE_BUTTON_ID button_id_, bool shift_, bool ctrl_, bool alt_, const Vec2d &pos_) :
            Event(MouseReleased), button_id(button_id_), shift(shift_), ctrl(ctrl_), alt(alt_), pos(pos_) {}
    };

    struct KeyboardPressedEvent : public Event {
        KEY_ID key_id;
        bool shift, ctrl, alt;

        KeyboardPressedEvent(KEY_ID key_id_, bool shift_, bool ctrl_, bool alt_) :
            Event(KeyboardPressed), key_id(key_id_), shift(shift_), ctrl(ctrl_), alt(alt_) {}
    };

    struct KeyboardReleasedEvent : public Event {
        KEY_ID key_id;
        bool shift, ctrl, alt;

        KeyboardReleasedEvent(KEY_ID key_id_, bool shift_, bool ctrl_, bool alt_) :
            Event(KeyboardReleased), key_id(key_id_), shift(shift_), ctrl(ctrl_), alt(alt_) {}
    };

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
    Widget(size_t id_, const LayoutBox &layout_);

    /**
     * \brief Default copy constructor
     * \note id is AUTO, z-index is set to 0, parent is set nullptr
    */
    Widget(const Widget &widget);

    /**
     * \brief Default assignment
     * \note z-index is set to 0, parent is set to nullptr
    */
    Widget &operator = (const Widget &widget);

    /**
     * \brief Returns widget id
    */
    size_t getId() const;

    /**
     * \brief Returns current layout box
    */
    LayoutBox &getLayoutBox();

    /**
     * \brief Returns current layout box
    */
    const LayoutBox &getLayoutBox() const;

    /**
     * \brief Sets layout box
    */
    void setLayoutBox(const LayoutBox &layout_);

    /**
     * \brief Generates transform using widget position
    */
    Transform getTransform() const;

    /**
     * \brief Returns z-index
    */
    int getZIndex() const;

    /**
     * \brief Sets z-index
    */
    void setZIndex(int z_index_);

    /**
     * \brief Returns parent
    */
    Widget *getParent();

    /**
     * \brief Returns parent
    */
    const Widget *getParent() const;

    /**
     * \brief Sets parent
    */
    void setParent(Widget *parent_);

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
     * \brief Draws widget on render target
     * \note By default draws red rectangle for debug purposes
    */
    virtual void draw(sf::RenderTarget &result, TransformStack &stack);

    /**
     * \brief Handle all sorts of events event
    */
    virtual void onEvent(const Event &event, EHC &ehc);

    /**
     * \brief Allows widget to change its position and size according to parent
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) { layout->onParentUpdate(parent_layout); }

    /**
     * \brief Checks children statuses
     * \note By default does nothing
    */
    virtual void checkChildren() {}

    /**
     * \brief Delete layout box
    */
    virtual ~Widget() { delete layout; };

protected:
    const size_t id;        ///< Widget ID that can be used for finding this widget in hierarchy
    LayoutBox *layout;      ///< Widget position and size encapsulated
    int z_index;            ///< Shows order in which widgets are drawn
    Widget *parent;         ///< Parent that holds this widget
    int status;             ///< Shows parent if some actions requiered

    /**
     * \brief If requested_id != AUTO_ID returns requested_id, otherwise returns unique id
    */
    size_t generateId(size_t requested_id);

    virtual void onTick(const TickEvent &event, EHC &ehc) {}
    virtual void onMouseMove(const MouseMoveEvent &event, EHC &ehc) {}
    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) {}
    virtual void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {}
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {}
    virtual void onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) {}
};


/**
 * \brief Checks if point is inside rectangle
*/
bool isInsideRect(Vec2d position, Vec2d size, Vec2d point);
