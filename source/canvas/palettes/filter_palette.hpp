/**
 * \file
 * \brief Contains FilterPalette interface
*/


#ifndef _FILTER_PALETTE_H_
#define _FILTER_PALETTE_H_


#include "canvas/plugin.hpp"
#include "window/window.hpp"


/// Contains filters and remembers last used one 
class FilterPalette {
private:
    List<Filter*> filters;        ///< Filter instances
    size_t last_filter;                 ///< Last used filter index

public:
    /// Filters avalaible for the palette
    enum FILTERS {
        LIGHTEN_FILTER,     ///< Increase intensity
        DARKEN_FILTER,      ///< Decrease intensity
        MONOCHROME_FILTER,  ///< Black and white style
        NEGATIVE_FILTER,    ///< Negative colors
        INTENSITY_CURVE,    ///< Intensity curve
        FILTERS_SIZE        ///< Count of predefined filters (this field must always be last!)
    };

    /**
     * \brief Constructs all filters
    */
    FilterPalette(WindowStyle &window_style);

    /**
     * \brief Returns last used filter instance
    */
    Filter *getLastFilter();

    /**
     * \brief Sets last used filter
    */
    void setLastFilter(size_t filter_id);

    /**
     * \brief Returns filter instance by ID
    */
    Filter *getFilter(size_t filter_id);

    /**
     * \brief Returns amount of supported filters
    */
    size_t getFilterCount() const;

    /**
     * \brief Adds filter to palette
    */
    void addFilter(Filter &new_filter);

    /**
     * \brief Delete filters
    */
    ~FilterPalette();
};


#endif
