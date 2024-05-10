/**
 * \file
 * \brief Contains interface of DeltaFilter
*/


#ifndef _DELTAFILTER_H_
#define _DELTAFILTER_H_


#include "canvas/plugin.hpp"


extern "C" plug::Plugin *loadPlugin();


/// Contains DeltaFilter name
class DeltaFilterData : public plug::PluginData {
    virtual const char *getName() const override {
        return "Intensity++";
    }

    virtual const char *getTexturePath() const override {
        return nullptr;
    }
};


/// Increases or decreases canvas brightness
class DeltaFilter : public BasicFilter {
public:
    DeltaFilter(char delta_);

    virtual void applyFilter(plug::Canvas &canvas) const override;

    virtual const plug::PluginData *getPluginData() const override;

private:
    unsigned char clip(int channel) const;

    int delta;
    DeltaFilterData my_data;
};


#endif
