/**
 * \file
 * \brief Contains interface of TestFilter
*/


#ifndef _SIMPLE_FILTER_H_
#define _SIMPLE_FILTER_H_


#include "canvas/plugin.hpp"


extern "C" plug::Plugin *loadPlugin();


class TestFilterData : public plug::PluginData {
    virtual const char *getName() const override {
        return "Test plug::Filter";
    }

    virtual const char *getTexturePath() const override {
        return nullptr;
    }
};


class TestFilter : public BasicFilter {
public:
    TestFilter();

    virtual void applyFilter(plug::Canvas &canvas) const override;

    virtual const plug::PluginData *getPluginData() const override;
 
private:
    TestFilterData my_data;
};


#endif
