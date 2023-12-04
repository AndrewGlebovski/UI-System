/**
 * \file
 * \brief Contains interface of IntensityCurve widget
*/


#ifndef _INTENSITY_CURVE_H_
#define _INTENSITY_CURVE_H_


#include "window/dialog.hpp"
#include "canvas/plugin.hpp"


/// Changes image colors according to curve
class IntensityCurveFilter : public BasicFilter {
public:
    IntensityCurveFilter(const WindowStyle &style_);

    IntensityCurveFilter(const IntensityCurveFilter&) = delete;
    
    IntensityCurveFilter &operator = (const IntensityCurveFilter&) = delete;

    /**
     * \brief Applies curve to canvas
    */
    virtual void applyFilter(Canvas &canvas) const override;

    /**
     * \brief Returns IntensityCurveDialog for curve calibration
    */
    virtual WidgetInterface *getWidget() override;

    /**
     * \brief Returns control point position
    */
    const Vec2d &getPoint(size_t index) const;

    /**
     * \brief Returns number of control points
    */
    size_t getPointCount() const;

    /**
     * \brief If control point already exists returns its index,
     * otherwise inserts new point and redraws curve
    */
    size_t addPoint(double x);

    /**
     * \brief Sets control point height and redraws curve
    */
    void setPointHeight(size_t index, double height);

    /**
     * \brief Returns new intensity based on the curve
    */
    uint8_t getIntensity(uint8_t value) const;

    /**
     * \brief Deletes plot
    */
    ~IntensityCurveFilter();

private:
    /**
     * \brief Redraws curve
    */
    void redrawCurve();

    List<Vec2d> points;     ///< Spline control points
    uint8_t *plot;          ///< Stores curve values
    WindowStyle style;
};


class IntensityCurveDialog : public Dialog {
public:
    IntensityCurveDialog(
        size_t id_, const LayoutBox &layout_,
        const WindowStyle &style_
    );
};


#endif
