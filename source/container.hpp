/**
 * \file
 * \brief Contains container class and protypes of his functions 
*/


/// Manages widgets
class Container : public Widget {
private:
    List<Widget*> widgets;          ///< List of widgets sorted by z-index
    size_t focused;                 ///< Focused widget draws on top and gets events first
    bool focus_enabled;             ///< Container set focus between widgets

    /**
     * \brief Remvoes widget by its index in widgets array
    */
    void removeWidget(size_t index);

public:
    /**
     * \brief Constructs container
     * \note If focus_enabled is true, container always has some child in focus
    */
    Container(
        size_t id_, const LayoutBox &layout_,
        bool focus_enabled_ = true
    );

    /**
     * \brief Draws widgets under his management
    */
    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override;

    /**
     * \brief Searches for widget among its children
    */
    virtual Widget *findWidget(size_t widget_id) override;

    /**
     * \brief Adds new widget to manager
     * \warning Widgets should be allocated using new and will be deleted by manager
    */
    virtual size_t addChild(Widget *child) override;

    /**
     * \brief Removes child from container
    */
    virtual void removeChild(size_t child_id) override;

    virtual EVENT_STATUS onMouseMove(const Vec2d &mouse, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vec2d &mouse, int button_id, TransformStack &stack) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vec2d &mouse, int button_id, TransformStack &stack) override;
    virtual EVENT_STATUS onKeyUp(int key_id) override;
    virtual EVENT_STATUS onKeyDown(int key_id) override;
    virtual EVENT_STATUS onTimer(float delta_time) override;

    /**
     * \brief Allows widget to change its position and size according to parent
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    /**
     * \brief Checks children statuses
    */
    virtual void checkChildren() override;


    ~Container();
};
