/**
 * \file
 * \brief Contains implementation of IntensityCurve widget
*/


#include <cmath>
#include "canvas/filters/intensity_curve.hpp"
#include "canvas/palettes/palette_manager.hpp"
#include "canvas/canvas_view.hpp"


// ============================================================================


const size_t BUTTON_NORMAL_COLOR = 0xd4d0c8ff;
const size_t BUTTON_HOVER_COLOR = 0x000080ff;
const size_t BUTTON_PRESSED_COLOR = 0x000080ff;

const unsigned BUTTON_FONT_SIZE = 20;
const size_t BUTTON_FONT_NORMAL_COLOR = 0x000000ff;
const size_t BUTTON_FONT_HOVER_COLOR = 0xffffffff;
const size_t BUTTON_FONT_PRESSED_COLOR = 0xffffffff;

const double POINT_RADIUS = 2.5;
const double ALPHA = 0.5; 


// ============================================================================


static double getKnotInterval(Vec2d p1, Vec2d p2);


static double lerp(double a, double b, double t);


static Vec2d remap(double a, double b, Vec2d c, Vec2d d, double u);


// ============================================================================


class ApplyFilterAction : public DialogAction {
public:
    ApplyFilterAction() {}

    virtual void operator () () override {
        Filter *filter = FILTER_PALETTE.getFilter(FilterPalette::INTENSITY_CURVE);
        filter->applyFilter(CANVAS_GROUP.getActive()->getCanvas());

        FILTER_PALETTE.setLastFilter(FilterPalette::INTENSITY_CURVE);

        dialog->setStatus(Widget::Status::Delete);
    }

    virtual DialogAction *clone() override {
        return new ApplyFilterAction();
    }
};


// ============================================================================


class CancelAction : public DialogAction {
public:
    virtual void operator () () override {
        dialog->setStatus(Widget::Status::Delete);
    }

    virtual DialogAction *clone() override {
        return new CancelAction();
    }
};


// ============================================================================


class IntensityCurve : public Widget {
public:
    IntensityCurve(size_t id_, IntensityCurveFilter &filter_);

    virtual void draw(TransformStack &stack, RenderTarget &result) override;

protected:
    virtual void onMouseMove(const MouseMoveEvent &event, EHC &ehc) override;
    
    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) override;
    
    virtual void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) override;

private:
    bool isPointMoving() const;

    IntensityCurveFilter &filter;
    size_t moving_point;
};


// ============================================================================


IntensityCurve::IntensityCurve(size_t id_, IntensityCurveFilter &filter_) : 
    Widget(id_, LazyLayoutBox(Vec2d(), Vec2d(255, 255))), filter(filter_), moving_point(filter_.getPointCount())
{}


void IntensityCurve::draw(TransformStack &stack, RenderTarget &result) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = applySize(stack, layout->getSize());

    RectShape background(global_position, global_size, White);
    background.draw(result);

    static VertexArray array(LineStrip, 256);

    for (size_t i = 0; i < 256; i++) {
        array[i] = Vertex(
            Vec2d(i, 255 - filter.getIntensity(i)) + global_position,
            Black
        );
    }

    result.draw(array);
}


void IntensityCurve::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = applySize(ehc.stack, layout->getSize());

    if (isPointMoving()) {
        Vec2d new_point = event.pos - global_position;
        new_point.y = 255 - new_point.y;

        filter.setPointHeight(moving_point, new_point.y);
    }

    if (isInsideRect(global_position, global_size, event.pos))
        ehc.overlapped = true;
}


void IntensityCurve::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = applySize(ehc.stack, layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos)) {
        Vec2d new_point = event.pos - global_position;

        moving_point = filter.addPoint(new_point.x);
        
        ehc.stopped = true;
    }
}


void IntensityCurve::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = applySize(ehc.stack, layout->getSize());

    moving_point = filter.getPointCount();

    if (isInsideRect(global_position, global_size, event.pos))
        ehc.stopped = true;
}


bool IntensityCurve::isPointMoving() const {
    return (moving_point < filter.getPointCount());
}


// ============================================================================


IntensityCurveDialog::IntensityCurveDialog(
    size_t id_, const LayoutBox &layout_,
    const WindowStyle &style_
) :
    Dialog(id_, layout_, "Intensity Curve", style_)
{
    setSize(Vec2d(285, 400));

    ApplyFilterAction *apply_action = new ApplyFilterAction();
    CancelAction *cancel_action = new CancelAction();

    apply_action->setDialog(*this);
    cancel_action->setDialog(*this);

    RectButtonStyle button_style(
        Color(BUTTON_NORMAL_COLOR),
        Color(BUTTON_HOVER_COLOR),
        Color(BUTTON_PRESSED_COLOR),
        style_.font,
        BUTTON_FONT_SIZE,
        Color(BUTTON_FONT_NORMAL_COLOR),
        Color(BUTTON_FONT_HOVER_COLOR),
        Color(BUTTON_FONT_PRESSED_COLOR)
    );

    container.addChild(new RectButton(
        1,
        LazyLayoutBox(Vec2d(0, 275), Vec2d(100, 50)),
        apply_action,
        "Apply",
        button_style
    ));

    container.addChild(new RectButton(
        2,
        LazyLayoutBox(Vec2d(120, 275), Vec2d(100, 50)),
        cancel_action,
        "Cancel",
        button_style
    ));

    container.addChild(new IntensityCurve(
        3,
        *static_cast<IntensityCurveFilter*>(FILTER_PALETTE.getFilter(FilterPalette::INTENSITY_CURVE))
    ));
}


// ============================================================================


static double getKnotInterval(Vec2d p1, Vec2d p2) {
    return pow((p2 - p1).length2(), ALPHA * 0.5);
}


static double lerp(double a, double b, double t) {
    return (b - a) * t + a;
}


static Vec2d remap(double a, double b, Vec2d c, Vec2d d, double u) {
    return (b - u) / (b - a) * c + (u - a) / (b - a) * d;
}


// ============================================================================


IntensityCurveFilter::IntensityCurveFilter(const WindowStyle &style_) :
    points(), plot(new uint8_t[256]), style(style_)
{
    ASSERT(plot, "Failed to allocate plot buffer!\n");

    points.push_back(Vec2d(-1, -1));
    points.push_back(Vec2d(0, 0));
    points.push_back(Vec2d(255, 255));
    points.push_back(Vec2d(256, 256));

    redrawCurve();
}


void IntensityCurveFilter::applyFilter(Canvas &canvas) const {
    SelectionMask &mask = canvas.getSelectionMask();
    Texture texture(canvas.getTexture());
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixel(x, y)) {
                Color origin(texture.getPixel(x, y));
                
                texture.setPixel(x, y, Color(
                    getIntensity(origin.r),
                    getIntensity(origin.g),
                    getIntensity(origin.b)
                ));
            }
        }
    }

    TextureShape(texture).draw(canvas, Vec2d(), canvas.getSize());
}


WidgetInterface *IntensityCurveFilter::getWidget() {
    return new IntensityCurveDialog(
        Widget::AUTO_ID,
        LazyLayoutBox(Vec2d(), Vec2d()),
        style
    );
}


const Vec2d &IntensityCurveFilter::getPoint(size_t index) const {
    ASSERT(index < points.size(), "Index is out of range!\n");

    return points[index];
}


size_t IntensityCurveFilter::getPointCount() const {
    return points.size();
}


size_t IntensityCurveFilter::addPoint(double x) {
    if (x < 0.5 || x > 254.5) return getPointCount();

    for (size_t i = 0; i < getPointCount() - 1; i++) {
        if (fabs(getPoint(i).x - x) < POINT_RADIUS) {
            return i;
        }

        if (x < getPoint(i).x) {
            points.insert(i, Vec2d(x, getIntensity(x)));
            redrawCurve();
            return i;
        }
    }

    return getPointCount();
}


void IntensityCurveFilter::setPointHeight(size_t index, double height) {
    ASSERT(index < points.size(), "Index is out of range!\n");

    if (height < 0.5) points[index].y = 0;
    else if (height > 254.5) points[index].y = 255;
    else points[index].y = height;

    redrawCurve();
}


uint8_t IntensityCurveFilter::getIntensity(uint8_t value) const {
    return plot[value];
}


void IntensityCurveFilter::redrawCurve() {
    for (size_t i = 0; i < getPointCount() - 3; i++) {
        // Get control points
        Vec2d p0 = getPoint(i);
        Vec2d p1 = getPoint(i + 1);
        Vec2d p2 = getPoint(i + 2);
        Vec2d p3 = getPoint(i + 3);

        // Calculate knots
        double t0 = 0;
        double t1 = getKnotInterval(p0, p1) + t0;
        double t2 = getKnotInterval(p1, p2) + t1;
        double t3 = getKnotInterval(p2, p3) + t2;

        // Iterate through pixels between control points
        size_t base = p1.x;
        size_t count = p2.x - p1.x;

        for (size_t j = 1; j < count; j++) {
            // Calculate pixel intensity
            double u = lerp(t1, t2, j / double(count));
            
            Vec2d A1 = remap(t0, t1, p0, p1, u);
            Vec2d A2 = remap(t1, t2, p1, p2, u);
            Vec2d A3 = remap(t2, t3, p2, p3, u);
            Vec2d B1 = remap(t0, t2, A1, A2, u);
            Vec2d B2 = remap(t1, t3, A2, A3, u);
            Vec2d C = remap(t1, t2, B1, B2, u);

            if (C.y < 0.5) plot[base + j] = 0;
            else if (C.y > 254.5) plot[base + j] = 255;
            else plot[base + j] = round(C.y);
        }

        // Set edge pixels
        plot[base] = p1.y;
        plot[base + count] = p2.y;
    }
}


IntensityCurveFilter::~IntensityCurveFilter() {
    ASSERT(plot, "Plot is nullptr!\n");

    delete[] plot;
}
