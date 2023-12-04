/**
 * \file
 * \brief Contains implementation of FilterPalette
*/


#include "canvas/palettes/filter_palette.hpp"
#include "canvas/filter.hpp"


// ============================================================================


FilterPalette::FilterPalette() : filters(FILTERS_SIZE, nullptr), last_filter(0) {
    filters[LIGHTEN_FILTER] = new IntensityFilter(20);
    filters[DARKEN_FILTER] = new IntensityFilter(-20);
    filters[MONOCHROME_FILTER] = new MonochromeFilter();
    filters[NEGATIVE_FILTER] = new NegativeFilter();
}


Filter *FilterPalette::getLastFilter() { return filters[last_filter]; }


void FilterPalette::setLastFilter(size_t filter_id) {
    ASSERT(filter_id < getFilterCount(), "Index is out of range!\n");

    last_filter = filter_id;
}


Filter *FilterPalette::getFilter(size_t filter_id) {
    ASSERT(filter_id < getFilterCount(), "Index is out of range!\n");

    return filters[filter_id];
}


size_t FilterPalette::getFilterCount() const { return filters.size(); }


void FilterPalette::addFilter(Filter &new_filter) {
    filters.push_back(&new_filter);
}


FilterPalette::~FilterPalette() {
    // Delete predefined filters
    for (size_t i = 0; i < FILTERS_SIZE; i++)
        delete filters[i];
    
    // Release additional filters
    for (size_t i = FILTERS_SIZE; i < filters.size(); i++)
        filters[i]->release();
}
