/**
 * \file
 * \brief Contains implementation of DeltaFilter
*/


#include "plugins/DeltaFilter.hpp"


// ============================================================================


extern "C" plug::Plugin *loadPlugin() {
    return new DeltaFilter(5);
}


// ============================================================================


const plug::PluginData *DeltaFilter::getPluginData() const {
    return static_cast<const plug::PluginData*>(&my_data);
}


unsigned char DeltaFilter::clip(int channel) const {
    if (channel + delta < 0) return 0;
    if (channel + delta > 255) return 255;
    return channel + delta;
}


DeltaFilter::DeltaFilter(char intensity_) :
    delta(intensity_), my_data() {}


void DeltaFilter::applyFilter(plug::Canvas &canvas) const {
    plug::SelectionMask &mask = canvas.getSelectionMask();
    plug::Texture texture(canvas.getTexture());

    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixel(x, y)) {
                plug::Color origin(texture.getPixel(x, y));
                
                texture.setPixel(x, y, plug::Color(
                    clip(origin.r),
                    clip(origin.g),
                    clip(origin.b)
                ));
            }
        }
    }

    TextureShape(texture).draw(canvas, plug::Vec2d(), canvas.getSize());
}
