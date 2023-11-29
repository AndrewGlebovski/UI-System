/**
 * \file
 * \brief Contains implementation of test filter
*/


#include "plugins/test_filter.hpp"


// ============================================================================


extern "C" Plugin *loadPlugin() {
    return new TestFilter();
}


// ============================================================================



TestFilter::TestFilter() :
    my_data() {}


const PluginData *TestFilter::getPluginData() const {
    return static_cast<const PluginData*>(&my_data);
}


void TestFilter::applyFilter(Canvas &canvas) const {
    SelectionMask &mask = canvas.getSelectionMask();
    Texture texture(canvas.getTexture());
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixelMask(x, y)) {
                Color origin(texture.getPixel(x, y));
                texture.setPixel(x, y, Color(255 - origin.r, 255 - origin.g, 255 - origin.b));
            }
        }
    }
    
    TextureShape(texture).draw(canvas, Vec2d(), canvas.getSize());
}
