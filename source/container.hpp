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
    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;


    virtual Widget *findWidget(size_t widget_id) override;


    /**
     * \brief Adds new widget to manager
     * \warning Widgets should be allocated using new and will be deleted by manager
    */
    virtual size_t addChild(Widget *child) override;


    virtual void removeChild(size_t child_id) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onKeyUp(int key_id) override;
    virtual int onKeyDown(int key_id) override;
    virtual int onTimer(float delta_time) override;
    virtual int onParentResize() override;
    virtual void checkChildren() override;


    ~Container();
};
