/**
 * \file
 * \brief Contains sources of container class functions
*/


#include <SFML/Graphics.hpp>
#include "basic/container.hpp"


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


#define CHECK_EHC(CALL_FUNC)                    \
do {                                            \
    CALL_FUNC;                                  \
    if (ehc.stopped) {                          \
        if (event.getType() == MousePressed)    \
            focused = i;                        \
        return;                                 \
    }                                           \
} while(0)


void Container::onEvent(const Event &event, EHC &ehc) {
    if (widgets.size() == 0) return;

    TransformApplier add_transform(ehc.stack, getTransform());

    if (focus_enabled) {
        widgets[focused]->onEvent(event, ehc);
        if (ehc.stopped) return;

        for (size_t i = 0; i < focused; i++)
            CHECK_EHC(widgets[i]->onEvent(event, ehc));

        for (size_t i = focused + 1; i < widgets.size(); i++)
            CHECK_EHC(widgets[i]->onEvent(event, ehc));
    }
    else {
        for (size_t i = 0; i < widgets.size(); i++)
            CHECK_EHC(widgets[i]->onEvent(event, ehc));
    }
}


#undef CHECK_EHC


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
