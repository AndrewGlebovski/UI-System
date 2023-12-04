/**
 * \file
 * \brief Contains sources of container class functions
*/


#include "basic/container.hpp"


Container::Container(
    size_t id_, const LayoutBox &layout_,
    bool focus_enabled_
) :
    Widget(id_, layout_),
    widgets(), focus_enabled(focus_enabled_)
{}


void Container::draw(RenderTarget &result, TransformStack &stack) {
# ifdef DEBUG_DRAW
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    RectShape rect(global_position, global_size, Color(0));
    rect.setBorder(1, Magenta);
    rect.draw(result);
#endif

    TransformApplier add_transform(stack, getTransform());

    for (size_t i = 0; i < widgets.size(); i++)
        widgets[i]->draw(result, stack);
}


Widget *Container::findWidget(size_t widget_id) {
    Widget *result = nullptr;

    for (size_t i = 0; i < widgets.size(); i++)
        if ((result = widgets[i]->findWidget(widget_id))) return result;

    return Widget::findWidget(widget_id);
}


size_t Container::addChild(Widget *child) {
    ASSERT(child, "Child is nullptr!\n");

    // Set this container as child's parent
    child->setParent(this);

    widgets.push_back(child);

    return child->getId();
}


void Container::removeWidget(size_t index) {
    ASSERT(index < widgets.size(), "Index is out of range!\n");

    delete widgets[index];
    widgets.remove(index);
}


void Container::setFocused(size_t index) {
    if (!focus_enabled) return;

    ASSERT(index < widgets.size(), "Index is out of range!\n");

    Widget *widget = widgets[index];
    widgets.remove(index);
    
    widgets.push_back(widget);
}


void Container::removeChild(size_t child_id) {
    for (size_t i = 0; i < widgets.size(); i++) {
        if (widgets[i]->getId() == child_id) {
            removeWidget(i);
            break;
        }
    }
}


size_t Container::getChildCount() const { return widgets.size(); }


void Container::onEvent(const Event &event, EHC &ehc) {
    TransformApplier add_transform(ehc.stack, getTransform());

    for (size_t i = widgets.size() - 1; i < widgets.size(); i--) {
        widgets[i]->onEvent(event, ehc);

        if (ehc.stopped) {
            if (event.getType() == MousePressed) setFocused(i);
            return;
        }
    }
}


void Container::onParentUpdate(const LayoutBox &parent_layout) {
    Widget::onParentUpdate(parent_layout);

    for (size_t i = 0; i < widgets.size(); i++)
        widgets[i]->onParentUpdate(*layout);
}


void Container::checkChildren() {
    size_t curr = 0;

    // ELEMENT OF THE LIST CAN BE DELETED IN ITERATION

    while(curr < widgets.size()) {
        switch(widgets[curr]->getStatus()) {
            case PASS: 
                widgets[curr]->checkChildren();
                curr++;
                break;
            case DELETE:
                removeWidget(curr);
                break;
            default:
                ASSERT(0, "Unknown status!\n");
        }
    }
}


Container::~Container() {
    for (size_t i = 0; i < widgets.size(); i++) {
        ASSERT(widgets[i], "Pointer to widget is nullptr!\n");
        delete widgets[i];
    }
}
