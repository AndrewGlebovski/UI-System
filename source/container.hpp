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


    void removeWidget(size_t index);

public:
    Container(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
        bool focus_enabled_ = true
    );


    /**
     * \brief Draws widgets under his management
    */
    virtual void draw(sf::RenderTarget &result, List<Transform> &transforms) override;


    virtual Widget *findWidget(size_t widget_id) override;


    /**
     * \brief Adds new widget to manager
     * \warning Widgets should be allocated using new and will be deleted by manager
    */
    virtual size_t addChild(Widget *child) override;


    virtual void removeChild(size_t child_id) override;


    virtual EVENT_STATUS onMouseMove(const Vector2D &mouse, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonUp(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onMouseButtonDown(const Vector2D &mouse, int button_id, List<Transform> &transforms) override;
    virtual EVENT_STATUS onKeyUp(int key_id) override;
    virtual EVENT_STATUS onKeyDown(int key_id) override;
    virtual EVENT_STATUS onTimer(float delta_time) override;
    virtual EVENT_STATUS onParentResize() override;
    virtual void checkChildren() override;


    ~Container();
};
