/**
 * \file
 * \brief Contains sources of AssetClasses functions
*/


#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "asset.hpp"


const size_t MAX_PATH = 256;


Asset::Asset() : textures(nullptr), count(0) {}


Asset::Asset(const Asset &arg) : textures(nullptr), count(arg.count) {
    assert(arg.textures);

    textures = new sf::Texture[count];
    assert(textures);

    for (size_t i = 0; i < count; i++)
        textures[i] = arg.textures[i];
}


void Asset::loadTextures(const char *rootpath, const char *files[], size_t files_size) {
    textures = new sf::Texture[files_size];
    assert(textures);

    count = files_size;

    char *path = (char *) calloc(strlen(rootpath) + MAX_PATH, 1);
    assert(path);

    for (size_t i = 0; i < files_size; i++) {
        sprintf(path, "%s/%s.png", rootpath, files[i]);
        assert(textures[i].loadFromFile(path));
    }

    free(path);
}


Asset &Asset::operator = (const Asset &arg) {
    assert(arg.textures);

    if (this != &arg) {
        if (textures) delete[] textures;

        count = arg.count;

        textures = new sf::Texture[count];
        assert(textures);

        for (size_t i = 0; i < count; i++)
            textures[i] = arg.textures[i];
    }

    return *this;
}


const sf::Texture &Asset::getTexture(int id) const {
    assert(textures);
    assert(0 <= id && size_t(id) < count);

    return textures[id];
}


Asset::~Asset() {
    assert(textures);
    
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
        "selected"
    };

    loadTextures(rootpath, FILES, sizeof(FILES) / 8);
}


const sf::Texture &PaletteViewAsset::operator [] (TEXTURE_ID id) const {
    return getTexture(id);
}
