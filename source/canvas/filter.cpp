/**
 * \file
 * \brief Contains implementation of some filters
*/


#include "canvas/filter.hpp"


// ============================================================================


unsigned char IntensityFilter::clip(int channel) const {
    if (channel + intensity < 0) return 0;
    if (channel + intensity > 255) return 255;
    return channel + intensity;
}


IntensityFilter::IntensityFilter(char intensity_) : intensity(intensity_) {}


void IntensityFilter::applyFilter(Canvas &canvas) const {
    SelectionMask &mask = canvas.getSelectionMask();
    sf::Image image = canvas.getTexture().getTexture().copyToImage();

    ASSERT(image.getSize().x == mask.getWidth(), "Invalid mask size!\n");
    ASSERT(image.getSize().y == mask.getHeight(), "Invalid mask size!\n");
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixelMask(x, y)) {
                sf::Color origin(image.getPixel(x, y));
                image.setPixel(x, y, sf::Color(clip(origin.r), clip(origin.g), clip(origin.b)));
            }
        }
    }
    
    sf::Texture tool_texture;
    tool_texture.loadFromImage(image);

    sf::Sprite tool_sprite(tool_texture);
    canvas.getTexture().draw(tool_sprite);
}


// ============================================================================


MonochromeFilter::MonochromeFilter() {}


void MonochromeFilter::applyFilter(Canvas &canvas) const {
    SelectionMask &mask = canvas.getSelectionMask();
    sf::Image image = canvas.getTexture().getTexture().copyToImage();

    ASSERT(image.getSize().x == mask.getWidth(), "Invalid mask size!\n");
    ASSERT(image.getSize().y == mask.getHeight(), "Invalid mask size!\n");
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixelMask(x, y)) {
                sf::Color origin(image.getPixel(x, y));
                unsigned aver = (unsigned(origin.r) + unsigned(origin.g) + unsigned(origin.b)) / 3U;
                image.setPixel(x, y, sf::Color(aver, aver, aver));
            }
        }
    }
    
    sf::Texture tool_texture;
    tool_texture.loadFromImage(image);

    sf::Sprite tool_sprite(tool_texture);
    canvas.getTexture().draw(tool_sprite);
}


// ============================================================================


NegativeFilter::NegativeFilter() {}


void NegativeFilter::applyFilter(Canvas &canvas) const {
    SelectionMask &mask = canvas.getSelectionMask();
    sf::Image image = canvas.getTexture().getTexture().copyToImage();

    ASSERT(image.getSize().x == mask.getWidth(), "Invalid mask size!\n");
    ASSERT(image.getSize().y == mask.getHeight(), "Invalid mask size!\n");
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixelMask(x, y)) {
                sf::Color origin(image.getPixel(x, y));
                image.setPixel(x, y, sf::Color(255 - origin.r, 255 - origin.g, 255 - origin.b));
            }
        }
    }
    
    sf::Texture tool_texture;
    tool_texture.loadFromImage(image);

    sf::Sprite tool_sprite(tool_texture);
    canvas.getTexture().draw(tool_sprite);
}
