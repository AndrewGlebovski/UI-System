/**
 * \file
 * \brief Contains implementation of some filters
*/


#include "canvas/filters/filters.hpp"


// ============================================================================


unsigned char IntensityFilter::clip(int channel) const {
    if (channel + intensity < 0) return 0;
    if (channel + intensity > 255) return 255;
    return channel + intensity;
}


IntensityFilter::IntensityFilter(char intensity_) : intensity(intensity_) {}


void IntensityFilter::applyFilter(plug::Canvas &canvas) const {
    plug::SelectionMask &mask = canvas.getSelectionMask();
    plug::Texture texture(canvas.getTexture());

    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixel(x, y)) {
                plug::Color origin(texture.getPixel(x, y));
                texture.setPixel(x, y, plug::Color(clip(origin.r), clip(origin.g), clip(origin.b)));
            }
        }
    }

    TextureShape(texture).draw(canvas, plug::Vec2d(), canvas.getSize());
}


// ============================================================================


MonochromeFilter::MonochromeFilter() {}


void MonochromeFilter::applyFilter(plug::Canvas &canvas) const {
    plug::SelectionMask &mask = canvas.getSelectionMask();
    plug::Texture texture(canvas.getTexture());
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixel(x, y)) {
                plug::Color origin(texture.getPixel(x, y));
                unsigned aver = (unsigned(origin.r) + unsigned(origin.g) + unsigned(origin.b)) / 3U;
                texture.setPixel(x, y, plug::Color(aver, aver, aver));
            }
        }
    }
    
    TextureShape(texture).draw(canvas, plug::Vec2d(), canvas.getSize());
}


// ============================================================================


NegativeFilter::NegativeFilter() {}


void NegativeFilter::applyFilter(plug::Canvas &canvas) const {
    plug::SelectionMask &mask = canvas.getSelectionMask();
    plug::Texture texture(canvas.getTexture());
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixel(x, y)) {
                plug::Color origin(texture.getPixel(x, y));
                texture.setPixel(x, y, plug::Color(255 - origin.r, 255 - origin.g, 255 - origin.b));
            }
        }
    }
    
    TextureShape(texture).draw(canvas, plug::Vec2d(), canvas.getSize());
}
