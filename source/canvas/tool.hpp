/**
 * \file
 * \brief Declares some canvas tools
*/


#ifndef _TOOL_H_
#define _TOOL_H_


#include "canvas/palette.hpp"
#include "basic/line_edit.hpp"


/// Standart pencil tool
class PencilTool : public CanvasTool {
protected:
    Vec2d prev_position;        ///< Previous mouse click position

public:
    PencilTool();


    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
    virtual void onMove(const Vec2d &mouse) override;
};


/// Tool for drawing rectangle
class RectTool : public CanvasTool {
protected:
    Vec2d draw_start;           ///< Previous mouse click position
    Widget *rect_preview;       ///< Widget that draws preview of the rectangle


    /**
     * \brief Creates white rectangle based on two points 
    */
    RectShape createRect(const Vec2d &p1, const Vec2d &p2) const;

public:
    RectTool();


    RectTool(const RectTool &rect_tool) = delete;
    RectTool &operator = (const RectTool &rect_tool) = delete;


    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
    virtual void onMove(const Vec2d &mouse) override;
    virtual void onConfirm() override;
    virtual Widget *getWidget() override;


    virtual ~RectTool() override;
};


/// Tool for drawing line
class LineTool : public CanvasTool {
protected:
    Widget *line_preview;       ///< Widget that draws preview of the line

public:
    LineTool();


    LineTool(const LineTool &line_tool) = delete;
    LineTool &operator = (const LineTool &line_tool) = delete;


    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
    virtual void onMove(const Vec2d &mouse) override;
    virtual void onConfirm() override;
    virtual Widget *getWidget() override;


    virtual ~LineTool() override;
};


/// Eraser tool
class EraserTool : public CanvasTool {
protected:
    Vec2d prev_position;        ///< Previous mouse click position

public:
    EraserTool();


    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
    virtual void onMove(const Vec2d &mouse) override;
};


/// Changes palette color to color of pixel under mouse
class ColorPicker : public CanvasTool {
public:
    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
};


/// Fill all pixels with the same color to palette color
class BucketTool : public CanvasTool {
public:
    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
};


/// Tool for drawing polygon
class PolygonTool : public CanvasTool {
protected:
    List<Vec2d> points;         ///< Points that form polygon
    Widget *polygon_preview;    ///< Widget that draws preview of the polygon

public:
    PolygonTool();

    PolygonTool(const PolygonTool &polygon_tool) = delete;
    PolygonTool &operator = (const PolygonTool &polygon_tool) = delete;

    List<Vec2d> &getPoints();

    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
    virtual void onMove(const Vec2d &mouse) override;
    virtual void onConfirm() override;
    virtual void onCancel() override;
    virtual Widget *getWidget() override;

    virtual ~PolygonTool() override;
};


/// Draws text on the canvas
class TextTool : public CanvasTool {
protected:
    sf::Font text_font;             ///< Text font
    LineEdit *text_preview;         ///< Widget that draws preview of the polygon

public:
    TextTool();


    TextTool(const TextTool &text_tool) = delete;
    TextTool &operator = (const TextTool &text_tool) = delete;


    virtual void onMainButton(ControlState state, const Vec2d &mouse) override;
    virtual void onConfirm() override;
    virtual void onCancel() override;
    virtual Widget *getWidget() override;


    virtual ~TextTool() override;
};


#endif
