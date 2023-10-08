/**
 * \file
 * \brief Contains canvas and his tools classes and protypes of their functions 
*/


/// Base class for canvas tool
class CanvasTool {
protected:
    bool is_drawing;            ///< Tool is in drawing mode

public:
    CanvasTool() : is_drawing(false) {}


    void setDrawing(bool new_value) { is_drawing = new_value; }


    virtual void onMouseMove(int mouse_x, int mouse_y, sf::RenderTexture &result) = 0;
    virtual void onMouseButtonDown(int mouse_x, int mouse_y, sf::RenderTexture &result) = 0;
    virtual void onMouseButtonUp(int mouse_x, int mouse_y, sf::RenderTexture &result) = 0;

    virtual ~CanvasTool() = default;
};


/// Draws line of canvas
class PencilTool : public CanvasTool {
protected:
    Vector2D mouse_prev;        ///< Previous mouse click position
    sf::Color color;

public:
    PencilTool(const sf::Color &color_);

    void setColor(const sf::Color &new_color);

    virtual void onMouseMove(int mouse_x, int mouse_y, sf::RenderTexture &result) override;
    virtual void onMouseButtonDown(int mouse_x, int mouse_y, sf::RenderTexture &result) override;
    virtual void onMouseButtonUp(int mouse_x, int mouse_y, sf::RenderTexture &result) override;
};


/// Clears part of canvas
class EraserTool : public CanvasTool {
protected:
    Vector2D mouse_prev;        ///< Previous mouse click position
    vec_t radius;

public:
    EraserTool(vec_t radius_);

    void setRadius(vec_t new_radius);

    virtual void onMouseMove(int mouse_x, int mouse_y, sf::RenderTexture &result) override;
    virtual void onMouseButtonDown(int mouse_x, int mouse_y, sf::RenderTexture &result) override;
    virtual void onMouseButtonUp(int mouse_x, int mouse_y, sf::RenderTexture &result) override;
};


/// Holds texture to draw on
class Canvas : public BaseUI {
protected:
    sf::RenderTexture texture;
    List<CanvasTool*> tools;
    size_t current_tool;

    void clear_canvas();

public:
    /**
     * \brief Creates empty canvas or opens image specified by path
    */
    Canvas(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        const char *image_path
    );


    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) override;
    virtual int onKeyDown(int key_id) override;


    virtual ~Canvas();
};
