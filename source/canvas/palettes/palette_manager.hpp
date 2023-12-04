/**
 * \file
 * \brief Contains PaletteManager interface
*/


#ifndef _PALETTE_MANAGER_H_
#define _PALETTE_MANAGER_H_


#include "canvas/palettes/color_palette.hpp"
#include "canvas/palettes/tool_palette.hpp"
#include "canvas/palettes/filter_palette.hpp"


/**
 * \brief Holds references to current color, tool and filter palettes
 * \note This class is a singleton (you must use getInstance to get it)
*/
class PaletteManager {
public:
    /**
     * \brief Returns reference to color palette
     * \warning Palette must be set first
    */
    ColorPalette &getColorPalette();

    /**
     * \brief Returns reference to tool palette
     * \warning Palette must be set first
    */
    ToolPalette &getToolPalette();

    /**
     * \brief Returns reference to filter palette
     * \warning Palette must be set first
    */
    FilterPalette &getFilterPalette();

    /**
     * \brief Sets color palette
     * \note If ToolPalette is set, updates ColorPalette for tools
     * \warning Nullptr argument is prohibited
     * \warning Palette must be allocated using 'new' and got delete automatically
    */
    void setColorPalette(ColorPalette *color_palette);

    /**
     * \brief Sets tool palette
     * \note If ColorPalette is set, updates ColorPalette for tools
     * \warning Nullptr argument is prohibited
     * \warning Palette must be allocated using 'new' and got delete automatically
    */
    void setToolPalette(ToolPalette *tool_palette);

    /**
     * \brief Sets filter palette
     * \warning Nullptr argument is prohibited
     * \warning Palette must be allocated using 'new' and got delete automatically
    */
    void setFilterPalette(FilterPalette *filter_palette);

    /**
     * \brief Returns single instance of PaletteManager
    */
    static PaletteManager &getInstance();

    /**
     * \brief Free palettes
    */
    ~PaletteManager();

private:
    PaletteManager();

    PaletteManager(const PaletteManager&) = delete;
    
    PaletteManager &operator = (const PaletteManager&) = delete;

    ColorPalette *color_palette;        ///< Current color palette
    ToolPalette *tool_palette;          ///< Current tool palette
    FilterPalette *filter_palette;      ///< Current filter palette
};


/// Shortcut for getting color palette from manager
#define COLOR_PALETTE PaletteManager::getInstance().getColorPalette()

/// Shortcut for getting tool palette from manager
#define TOOL_PALETTE PaletteManager::getInstance().getToolPalette()

/// Shortcut for getting filter palette from manager
#define FILTER_PALETTE PaletteManager::getInstance().getFilterPalette()


#endif
