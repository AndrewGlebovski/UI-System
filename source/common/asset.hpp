/**
 * \file
 * \brief Contains AssetClasses and protypes of their functions 
*/


#ifndef _ASSET_H_
#define _ASSET_H_


#include <cstdio>
#include "common/list.hpp"
#include "widget/render_target.hpp"


/// Base class for all assets
class Asset {
protected:
    List<plug::Texture*> textures;    ///< Textures array

    /**
     * \brief Contructs empty asset
    */
    Asset();

    /**
     * \brief Init texture buffer and copies assets
    */
    Asset(const Asset &arg);

    /**
     * \brief Loads textures from files located in the rootpath directory
    */
    void loadTextures(const char *rootpath, const char *files[], size_t assets_count);

    /**
     * \brief Copies assets
    */
    Asset &operator = (const Asset &arg);

    /**
     * \brief Returns texture by its id
    */
    const plug::Texture &getTexture(int id) const;

    /**
     * \brief Free textures
    */
    ~Asset();
};


/// Holds textures required for window
class WindowAsset : public Asset {
public:
    enum TEXTURE_ID {
        TITLE,                  ///< Title texture (will be repeated)
        FRAME_TL,               ///< Top-left corner texture
        FRAME_L,                ///< Left side texture (will be repeated)
        FRAME_BL,               ///< Bottom-right corner texture
        FRAME_B,                ///< Bottom texture (will be repeated)
        FRAME_BR,               ///< Bottom-right corner texture
        FRAME_R,                ///< Right side texture (will be repeated)
        FRAME_TR,               ///< Top-right corner texture
        FRAME_CENTER,           ///< Center texture (will be repeated)
        BUTTON_NORMAL,          ///< Window title buttons normal texture
        BUTTON_HOVER,           ///< Window title buttons hover texture
        BUTTON_PRESSED,         ///< Window title buttons pressed texture
        EXPAND_ICON,            ///< Window expand button icon
        CLOSE_ICON              ///< Window close button icon
    };

    WindowAsset(const char *rootpath);

    const plug::Texture &operator [] (TEXTURE_ID id) const;
};


/// Holds textures required for palette
class PaletteViewAsset : public Asset {
public:
    enum TEXTURE_ID {
        BUCKET_TEXTURE,
        ERASER_TEXTURE,
        LINE_TEXTURE,
        PENCIL_TEXTURE,
        PICKER_TEXTURE,
        RECT_TEXTURE,
        POLYGON_TEXTURE,
        NORMAL_TEXTURE,
        SELECTED_TEXTURE,
        TEXT_TEXTURE
    };

    PaletteViewAsset(const char *rootpath);

    const plug::Texture &operator [] (TEXTURE_ID id) const;
};


#endif
