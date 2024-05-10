/**
 * \file
 * \brief Contains container class and protypes of his functions 
*/


#ifndef _CONTAINER_H_
#define _CONTAINER_H_


#include "widget/widget.hpp"


/// Manages widgets
class Container : public Widget {
public:
    /**
     * \brief Constructs container
     * \note If focus_enabled is true, container always has some child in focus
    */
    Container(
        size_t id_, const plug::LayoutBox &layout_,
        bool focus_enabled_ = true
    );

    /**
     * \brief Draws widgets under his management
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;

    /**
     * \brief Searches for widget among its children
    */
    virtual Widget *findWidget(size_t widget_id) override;

    /**
     * \brief Adds new widget to manager
     * \warning Widgets should be allocated using new and will be deleted by manager
    */
    virtual size_t addChild(Widget *child);

    /**
     * \brief Removes child from container
    */
    virtual void removeChild(size_t child_id);

    /**
     * \brief Returns number of children
    */
    size_t getChildCount() const;

    /**
     * \brief Broadcast events to container children
    */
    virtual void onEvent(const plug::Event &event, plug::EHC &ehc) override;

    /**
     * \brief Allows widget to change its position and size according to parent
    */
    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

    /**
     * \brief Checks children statuses
    */
    virtual void checkChildren() override;

    /**
     * \brief Deletes children
    */
    ~Container();

protected:
    /**
     * \brief Removes widget by its index in widgets array
    */
    void removeWidget(size_t index);

    /**
     * \brief Pushes element to the end of the widgets array
     * \note If focus is not enabled, does nothing
    */
    void setFocused(size_t index);

    List<Widget*> widgets;          ///< List of widgets sorted by z-index
    bool focus_enabled;             ///< Container set focus between widgets
};


#endif
