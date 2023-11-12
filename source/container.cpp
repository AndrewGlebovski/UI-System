/**
 * \file
 * \brief Contains sources of container class functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "widget.hpp"
#include "container.hpp"


Container::Container(
    size_t id_, const LayoutBox &layout_,
    bool focus_enabled_
) :
    Widget(id_, layout_),
    widgets(), focused(0), focus_enabled(focus_enabled_)
{}


void Container::draw(sf::RenderTarget &result, TransformStack &stack) {
# ifdef DEBUG_DRAW
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    sf::RectangleShape rect(global_size);
    rect.setPosition(global_position);
    rect.setFillColor(sf::Color(0));
    rect.setOutlineColor(sf::Color::Magenta);
    rect.setOutlineThickness(1);
    result.draw(rect);
#endif

    size_t count = widgets.size();
    if (count == 0) return;

    TransformApplier add_transform(stack, getTransform());

    for (size_t i = count - 1; i > focused; i--)
        widgets[i]->draw(result, stack);
    
    for (size_t i = focused - 1; i < count; i--)
        widgets[i]->draw(result, stack);
    
    widgets[focused]->draw(result, stack);
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
    // Container is empty
    if (widgets.size() == 0) {
        widgets.push_back(child);
        focused = 0;
        return child->getId();
    }
    // Find place for widget according to z-index
    for (size_t i = 0; i < widgets.size(); i++) {
        if (child->getZIndex() > widgets[i]->getZIndex()) {
            widgets.insert(i, child);
            focused = i;
            return child->getId();
        }
    }
    // Widget has the biggest z-index
    widgets.push_back(child);
    focused = widgets.size() - 1;
    return child->getId();
}


void Container::removeWidget(size_t index) {
    ASSERT(index < widgets.size(), "Index is out of range!\n");

    if (widgets.size()) {
        if (index < focused)
            focused--;
        else if (index == focused)
            focused = 0;
    }
    else focused = 0;

    delete widgets[index];
    widgets.remove(index);
}


void Container::removeChild(size_t child_id) {
    for (size_t i = 0; i < widgets.size(); i++) {
        if (widgets[i]->getId() == child_id) {
            removeWidget(i);
            break;
        }
    }
}


/**
 * \brief Special define for short loops inside container's event handler
*/
#define CONTAINER_FOR(CALL_FUNC, ...)                                               \
do {                                                                                \
    if (focus_enabled) {                                                            \
        if (widgets[focused]->CALL_FUNC == HANDLED) return HANDLED;                 \
        \
        for (size_t i = 0; i < focused; i++)                                        \
            if (widgets[i]->CALL_FUNC == HANDLED) { __VA_ARGS__; return HANDLED; }  \
        \
        for (size_t i = focused + 1; i < widgets.size(); i++)                       \
            if (widgets[i]->CALL_FUNC == HANDLED) { __VA_ARGS__; return HANDLED; }  \
    }                                                                               \
    else {                                                                          \
        for (size_t i = 0; i < widgets.size(); i++)                                 \
            if (widgets[i]->CALL_FUNC == HANDLED) return HANDLED;                   \
    }                                                                               \
} while(0)


EVENT_STATUS Container::onMouseMove(const Vec2d &mouse, TransformStack &stack) {
    if (widgets.size() == 0) return UNHANDLED;

    TransformApplier add_transform(stack, getTransform());

    CONTAINER_FOR(onMouseMove(mouse, stack));
    
    return UNHANDLED;
}


EVENT_STATUS Container::onMouseButtonUp(const Vec2d &mouse, int button_id, TransformStack &stack) {
    if (widgets.size() == 0) return UNHANDLED;

    TransformApplier add_transform(stack, getTransform());

    CONTAINER_FOR(onMouseButtonUp(mouse, button_id, stack));
    
    return UNHANDLED;
}


EVENT_STATUS Container::onMouseButtonDown(const Vec2d &mouse, int button_id, TransformStack &stack) {
    if (widgets.size() == 0) return UNHANDLED;

    TransformApplier add_transform(stack, getTransform());

    CONTAINER_FOR(onMouseButtonDown(mouse, button_id, stack), focused = i);
    
    return UNHANDLED;
}


EVENT_STATUS Container::onKeyUp(int key_id) {
    if (widgets.size() == 0) return UNHANDLED;

    CONTAINER_FOR(onKeyUp(key_id));
    
    return UNHANDLED;
}


EVENT_STATUS Container::onKeyDown(int key_id) {
    if (widgets.size() == 0) return UNHANDLED;

    CONTAINER_FOR(onKeyDown(key_id));
    
    return UNHANDLED;
}


#undef CONTAINER_FOR


EVENT_STATUS Container::onTimer(float delta_time) {
    for (size_t i = 0; i < widgets.size(); i++)
        widgets[i]->onTimer(delta_time);
    
    return UNHANDLED;
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


#pragma GCC diagnostic pop
