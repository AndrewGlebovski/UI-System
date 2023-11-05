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
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    bool focus_enabled_
) :
    Widget(id_, transform_, size_, z_index_, parent_),
    widgets(), focused(0), focus_enabled(focus_enabled_)
{}


void Container::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    size_t count = widgets.size();
    if (count == 0) return;

    TransformApplier add_transform(transforms, transform);

    for (size_t i = count - 1; i > focused; i--)
        widgets[i]->draw(result, transforms);
    
    for (size_t i = focused - 1; i < count; i--)
        widgets[i]->draw(result, transforms);
    
    widgets[focused]->draw(result, transforms);
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
    child->parent = this;
    // Container is empty
    if (widgets.size() == 0) {
        widgets.push_back(child);
        focused = 0;
        return child -> getId();
    }
    // Find place for widget according to z_index
    for (size_t i = 0; i < widgets.size(); i++) {
        if (child->z_index > widgets[i]->z_index) {
            widgets.insert(i, child);
            focused = i;
            return child -> getId();
        }
    }
    // Widget has the biggest z_index
    widgets.push_back(child);
    focused = widgets.size() - 1;
    return child -> getId();
}


void Container::removeWidget(size_t index) {
    ASSERT(index < widgets.size(), "Index is out of range!\n");

    if (index < focused) focused--;
    else if (index == focused) focused = widgets.size() - 2;

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


int Container::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    if (widgets.size() == 0) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    CONTAINER_FOR(onMouseMove(mouse_x, mouse_y, transforms));
    
    return UNHANDLED;
}


int Container::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (widgets.size() == 0) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    CONTAINER_FOR(onMouseButtonUp(mouse_x, mouse_y, button_id, transforms));
    
    return UNHANDLED;
}


int Container::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (widgets.size() == 0) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    CONTAINER_FOR(onMouseButtonDown(mouse_x, mouse_y, button_id, transforms), focused = i);
    
    return UNHANDLED;
}


int Container::onKeyUp(int key_id) {
    if (widgets.size() == 0) return UNHANDLED;

    CONTAINER_FOR(onKeyUp(key_id));
    
    return UNHANDLED;
}


int Container::onKeyDown(int key_id) {
    if (widgets.size() == 0) return UNHANDLED;

    CONTAINER_FOR(onKeyDown(key_id));
    
    return UNHANDLED;
}


int Container::onTimer(float delta_time) {
    for (size_t i = 0; i < widgets.size(); i++)
        widgets[i]->onTimer(delta_time);
    
    return UNHANDLED;
}


int Container::onParentResize() {
    for (size_t i = 0; i < widgets.size(); i++)
        widgets[i]->onParentResize();
    
    return UNHANDLED;
}


#undef CONTAINER_FOR


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
