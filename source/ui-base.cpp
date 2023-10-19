/**
 * \file
 * \brief Contains sources of base UI class functions
*/


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "ui-base.hpp"


Transform::Transform() : offset(Vector2D()) {}


Transform::Transform(const Vector2D &offset_) : offset(offset_) {}


void Transform::apply(const Transform &transform) {
    offset += transform.offset;
}


void Transform::cancel(const Transform &transform) {
    offset -= transform.offset;
}


TransformApplier::TransformApplier(List<Transform> &transforms_, const Transform &local_transform) :
    transforms(transforms_)
{
    transforms[0].apply(local_transform);
    transforms.push_back(local_transform);
}


TransformApplier::~TransformApplier() {
    transforms[0].cancel(transforms[transforms.getSize() - 1]);
    transforms.pop_back();
}


BaseUI::BaseUI(size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_) :
    id(generateId(id_)), transform(transform_), size(size_), z_index(z_index_), parent(parent_) {}


size_t BaseUI::generateId(size_t requested_id) {
    if (requested_id != AUTO_ID) return requested_id;

    static size_t available_id = 1;
    return available_id++;
}


size_t BaseUI::getId() const {
    return id;
}


void BaseUI::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);
    /* DEBUG DRAWING
    sf::RectangleShape rect(Vector2D(25, 25));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(transforms[0].offset);
    result.draw(rect);
    */
}


void BaseUI::setSize(const Vector2D &new_size) {
    if (new_size.x < 0)
        size.x = 0;
    else if (transform.offset.x + new_size.x > parent->size.x)
        size.x = parent->size.x - transform.offset.x;
    else
        size.x = new_size.x;
    
    if (new_size.y < 0)
        size.y = 0;
    else if (transform.offset.y + size.y > parent->size.y)
        size.y = parent->size.y - transform.offset.y;
    else
        size.y = new_size.y;
}


void BaseUI::setPosition(const Vector2D &new_position) {
    if (new_position.x < 0)
        transform.offset.x = 0;
    else if (new_position.x + size.x > parent->size.x)
        transform.offset.x = parent->size.x - size.x;
    else
        transform.offset.x = new_position.x;
    
    if (new_position.y < 0)
        transform.offset.y = 0;
    else if (new_position.y + size.y > parent->size.y)
        transform.offset.y = parent->size.y - size.y;
    else
        transform.offset.y = new_position.y;
}


bool isInsideRect(Vector2D position, Vector2D size, Vector2D point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}
