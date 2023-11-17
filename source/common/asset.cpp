/**
 * \file
 * \brief Contains sources of AssetClasses functions
*/


#include <cstdlib>
#include <cstring>
#include "common/assert.hpp"
#include "common/asset.hpp"


const size_t MAX_PATH = 256;


Asset::Asset() : textures(nullptr), count(0) {}


Asset::Asset(const Asset &arg) : textures(nullptr), count(arg.count) {
    ASSERT(arg.textures, "Invalid argument!\n");

    textures = new sf::Texture[count];
    ASSERT(textures, "Failed to allocate buffer!\n");

    for (size_t i = 0; i < count; i++)
        textures[i] = arg.textures[i];
}


void Asset::loadTextures(const char *rootpath, const char *files[], size_t files_size) {
    textures = new sf::Texture[files_size];
    ASSERT(textures, "Failed to allocate buffer!\n");

    count = files_size;

    char *path = (char *) calloc(strlen(rootpath) + MAX_PATH, 1);
    ASSERT(path, "Failed to allocate buffer!\n");

    for (size_t i = 0; i < files_size; i++) {
        sprintf(path, "%s/%s.png", rootpath, files[i]);
        ASSERT(textures[i].loadFromFile(path), "Failed to load texture %s!\n", path);
    }

    free(path);
}


Asset &Asset::operator = (const Asset &arg) {
    ASSERT(arg.textures, "Invalid argument!\n");

    if (this != &arg) {
        if (textures) delete[] textures;

        count = arg.count;

        textures = new sf::Texture[count];
        ASSERT(textures, "Failed to allocate buffer!\n");

        for (size_t i = 0; i < count; i++)
            textures[i] = arg.textures[i];
    }

    return *this;
}


const sf::Texture &Asset::getTexture(int id) const {
    ASSERT(textures, "Texture buffer is nullptr!\n");
    ASSERT(0 <= id && size_t(id) < count, "Index is out of range!\n");

    return textures[id];
}


Asset::~Asset() {
    ASSERT(textures, "Texture buffer is nullptr");
    
    delete[] textures;
}


WindowAsset::WindowAsset(const char *rootpath) {
    static const char *FILES[] = {
        "title",
        "top_left",
        "left",
        "bottom_left",
        "bottom",
        "bottom_right",
        "right",
        "top_right",
        "center",
        "button_normal",
        "button_hover",
        "button_pressed",
        "expand_icon",
        "close_icon"
    };

    loadTextures(rootpath, FILES, sizeof(FILES) / 8);

    // SET REPEAT FLAG

    textures[TITLE].setRepeated(true);
    textures[FRAME_CENTER].setRepeated(true);
    textures[FRAME_B].setRepeated(true);
    textures[FRAME_L].setRepeated(true);
    textures[FRAME_R].setRepeated(true);
}


const sf::Texture &WindowAsset::operator [] (TEXTURE_ID id) const {
    return getTexture(id);
}


PaletteViewAsset::PaletteViewAsset(const char *rootpath) {
    static const char *FILES[] = {
        "bucket",
        "eraser",
        "line",
        "pencil",
        "picker",
        "rect",
        "polygon",
        "normal",
        "selected",
        "text"
    };

    loadTextures(rootpath, FILES, sizeof(FILES) / 8);
}


const sf::Texture &PaletteViewAsset::operator [] (TEXTURE_ID id) const {
    return getTexture(id);
}
