/**
 * \file
 * \brief Contains implementation of test filter
*/


#include "plugins/test_filter.hpp"


// ============================================================================


extern "C" plug::Plugin *loadPlugin() {
    return new TestFilter();
}


// ============================================================================



TestFilter::TestFilter() :
    my_data() {}


const plug::PluginData *TestFilter::getPluginData() const {
    return static_cast<const plug::PluginData*>(&my_data);
}


void TestFilter::applyFilter(plug::Canvas &canvas) const {
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
    
    TextureShape(texture).draw(canvas, plug::Vec2d(), plug::Vec2d(texture.width, texture.height));
}
