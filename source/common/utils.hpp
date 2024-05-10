/**
 * \file
 * \brief Contains functions for converting sf structures to plug
*/


#ifndef _UTILS_H_
#define _UTILS_H_


#include <SFML/Graphics.hpp>
#include "standart/Color.h"
#include "standart/Math/Vec2d.h"
#include "standart/Graphics/VertexArray.h"


sf::Vector2f getSfmlVector2f(const plug::Vec2d &vec);


sf::Vector2i getSfmlVector2i(const plug::Vec2d &vec);


sf::Vector2u getSfmlVector2u(const plug::Vec2d &vec);


plug::Vec2d getPlugVector(const sf::Vector2f &vec);


plug::Vec2d getPlugVector(const sf::Vector2i &vec);


plug::Vec2d getPlugVector(const sf::Vector2u &vec);


sf::Color getSfmlColor(plug::Color color);


plug::Color getPlugColor(sf::Color color);


bool isEqual(double a, double b);


bool isEqual(const plug::Vec2d &a, const plug::Vec2d &b);


bool isEqual(plug::Color a, plug::Color b);


plug::Color hex2Color(uint32_t hex);


const plug::Color Black(0, 0, 0);
const plug::Color White(255, 255, 255);
const plug::Color Red(255, 0, 0);
const plug::Color Green(0, 255, 0);
const plug::Color Blue(0, 0, 255);
const plug::Color Yellow(255, 255, 0);
const plug::Color Magenta(255, 0, 255);
const plug::Color Cyan(0, 255, 255);


#endif
