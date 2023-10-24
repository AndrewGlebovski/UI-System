/**
 * \file
 * \brief Contains sources of widget class functions
*/


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "widget.hpp"


Transform::Transform() : offset(Vector2D()) {}


Transform::Transform(const Vector2D &offset_) : offset(offset_) {}


void Transform::apply(const Transform &transform) {
    offset += transform.offset;
}


void Transform::cancel(const Transform &transform) {
    offset -= transform.offset;
}


bool Transform::operator == (const Transform &transform) {
    return (offset == transform.offset);
}


TransformApplier::TransformApplier(List<Transform> &transforms_, const Transform &local_transform) :
    transforms(transforms_)
{
    ASSERT(verify(), "Transform stack is corrupted!\n");
    transforms.front().apply(local_transform);
    transforms.push_back(local_transform);
}


bool TransformApplier::verify() const {
    Transform top;

    for (size_t i = 1; i < transforms.size(); i++)
        top.apply(transforms[i]);
    
    return (top == transforms.front());
}


TransformApplier::~TransformApplier() {
    ASSERT(verify(), "Transform stack is corrupted!\n");
    transforms.front().cancel(transforms.back());
    transforms.pop_back();
}


Widget::Widget(size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_) :
    id(generateId(id_)), status(PASS), transform(transform_), size(size_), z_index(z_index_), parent(parent_) {}


size_t Widget::generateId(size_t requested_id) {
    if (requested_id != AUTO_ID) return requested_id;

    return size_t(this);
}


size_t Widget::getId() const {
    return id;
}


Widget *Widget::findWidget(size_t widget_id) {
    return (widget_id == getId()) ? this : nullptr;
}


size_t Widget::addChild(Widget *child) {
    printf("This widget does not support children!\n");
    abort();
}


void Widget::removeChild(size_t child_id) {
    printf("This widget does not support children!\n");
    abort();
}


int Widget::getStatus() const {
    return status;
}


void Widget::setStatus(WIDGET_STATUS new_status) {
    status = new_status;
}


void Widget::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);
    /* DEBUG DRAWING
    sf::RectangleShape rect(Vector2D(25, 25));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(transforms.front().offset);
    result.draw(rect);
    */
}


Vector2D Widget::onChildResize(Widget *child, const Vector2D &new_size) {
    Vector2D allowed_size = new_size;

    if (new_size.x < 0)
        allowed_size.x = 0;
    else if (child->transform.offset.x + new_size.x > size.x)
        allowed_size.x = size.x - child->transform.offset.x;
    
    if (new_size.y < 0)
        allowed_size.y = 0;
    else if (child->transform.offset.y + new_size.y > size.y)
        allowed_size.y = size.y - child->transform.offset.y;
    
    return allowed_size;
}


Transform Widget::onChildTransform(Widget *child, const Transform &new_transform) {
    Transform allowed_transform = new_transform;

    if (new_transform.offset.x < 0)
        allowed_transform.offset.x = 0;
    else if (new_transform.offset.x + child->size.x > size.x)
        allowed_transform.offset.x = size.x - child->size.x;
    
    if (new_transform.offset.y < 0)
        allowed_transform.offset.y = 0;
    else if (new_transform.offset.y + child->size.y > size.y)
        allowed_transform.offset.y = size.y - child->size.y;
    
    return allowed_transform;
}


void Widget::tryResize(const Vector2D &new_size) {
    size = parent->onChildResize(this, new_size);
}


void Widget::tryTransform(const Transform &new_transform) {
    transform = parent->onChildTransform(this, new_transform);
}


bool isInsideRect(Vector2D position, Vector2D size, Vector2D point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}
