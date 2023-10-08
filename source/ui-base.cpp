/**
 * \file
 * \brief Contains sources of base UI class functions
*/


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "ui-base.hpp"


BaseUI::BaseUI(const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_) :
    position(position_), size(size_), z_index(z_index_), parent(parent_) {}


void BaseUI::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    Vector2D total_position = transforms[0] + position;

    sf::RectangleShape rect(Vector2D(25, 25));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(total_position);
    result.draw(rect);
}


void BaseUI::setSize(const Vector2D &new_size) {
    if (position.x + new_size.x > parent->size.x)
        size.x = parent->size.x - position.x;
    else
        size.x = new_size.x;
    
    if (position.y + size.y > parent->size.y)
        size.y = parent->size.y - position.y;
    else
        size.y = new_size.y;
}


void BaseUI::setPosition(const Vector2D &new_position) {
    if (new_position.x < 0)
        position.x = 0;
    else if (new_position.x + size.x > parent->size.x)
        position.x = parent->size.x - size.x;
    else
        position.x = new_position.x;
    
    if (new_position.y < 0)
        position.y = 0;
    else if (new_position.y + size.y > parent->size.y)
        position.y = parent->size.y - size.y;
    else
        position.y = new_position.y;
}


TransformApplier::TransformApplier(List<Vector2D> &transforms_, const Vector2D &local_transform) :
    transforms(transforms_)
{
    transforms[0] += local_transform;
    transforms.push_back(local_transform);
}


TransformApplier::~TransformApplier() {
    transforms[0] -= transforms[transforms.getSize() - 1];
    transforms.pop_back();
}
