/**
 * \file
 * \brief Contains canvas view and canvas group interface
*/


#ifndef _CANVAS_VIEW_H_
#define _CANVAS_VIEW_H_


#include "canvas/canvas.hpp"
#include "widget/widget.hpp"


class CanvasView;


class CanvasGroup {
private:
    List<CanvasView*> canvases;     ///< Canvases in this group
    size_t active;              ///< Currently active canvas


    /**
     * \brief Returns index of the canvas in list
    */
    size_t getIndex(CanvasView *canvas) const;

public:
    /**
     * \brief Creates empty group
    */
    CanvasGroup();


    /**
     * \brief Sets active canvas
    */
    void setActive(CanvasView *new_active);


    /**
     * \brief Returns active canvas
    */
    CanvasView *getActive();


    /**
     * \brief Adds canvas to this group
    */
    void addCanvas(CanvasView *new_canvas);


    /**
     * \brief Removes canvas from this group
    */
    void removeCanvas(CanvasView *canvas);


    /**
     * \brief Checks if the canvas is in this group
    */
    bool isInGroup(CanvasView *canvas) const;
};


/// Draws canvas and supply events to tools
class CanvasView : public Widget {
public:
    /**
     * \brief Creates empty canvas
    */
    CanvasView(
        size_t id_, const LayoutBox &layout_,
        CanvasGroup &group_
    );

    CanvasView(const CanvasView &canvas) = default;

    CanvasView &operator = (const CanvasView &canvas) = default;

    /**
     * \brief Creates image with the given size filled with background color
    */
    bool createImage(size_t width, size_t height);

    /**
     * \brief Opens image file
     * \note If file fails to open, nothing happens
    */
    bool openImage(const char *filename_);

    /**
     * \brief Saves texture to current image file
     * \warning Assert will be called if image is not open
    */
    void saveImage();

    /**
     * \brief Saves texture to path
     * \note Changes filename field to filename_
    */
    void saveImageAs(const char *filename_);

    /**
     * \brief Returns path to current image
     * \note Returns nullptr if image is not open
    */
    const char *getFilename() const;

    /**
     * \brief Returns true if image is open
    */
    bool isImageOpen() const;

    /**
     * \brief Returns size of the texture
    */
    Vec2d getTextureSize() const;

    /**
     * \brief Returns reference to canvas
    */
    Canvas &getCanvas();

    /**
     * \brief Returns true if canvas is active in his group
    */
    bool isActive() const;

    /**
     * \brief Returns texture offset
    */
    const Vec2d &getTextureOffset() const;

    /**
     * \brief Sets texture offset
    */
    void setTextureOffset(const Vec2d &texture_offset_);

    /**
     * \brief Draws canvas inner texture
    */
    virtual void draw(RenderTarget &result, TransformStack &stack) override;

    /**
     * \brief Broadcast events to tool widget
    */
    virtual void onEvent(const Event &event, EHC &ehc) override;

    /**
     * \brief Removes canvas from his group
    */
    virtual ~CanvasView() override;

protected:
    virtual void onMouseMove(const MouseMoveEvent &event, EHC &ehc) override;
    
    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) override;
    
    virtual void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) override;
    
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) override;
    
    virtual void onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) override;

    SFMLCanvas canvas;
    Vec2d texture_offset;
    CanvasGroup *group;
    std::string filename;
};


#endif
