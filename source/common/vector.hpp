/**
 * \file
 * \brief Contains vector class and prototypes of his functions
*/


#ifndef _VECTOR_H_
#define _VECTOR_H_


/// Vector component type
typedef double vec_t;


/// Math vector with x, y components
class Vec2d {
public:
    vec_t x;    ///< Vector X component
    vec_t y;    ///< Vector Y component

    /**
     * \brief Constructs zero vector
    */
    Vec2d();

    /**
     * \brief Constructs vector from initial components
    */
    Vec2d(vec_t init_x, vec_t init_y);

    /**
     * \brief Constructs vector from its SFML analogue
    */
    Vec2d(const sf::Vector2f &vec);

    /**
     * \brief Constructs vector from SFML Vector2u
    */
    Vec2d(const sf::Vector2u &vec);

    /**
     * \brief Vector length
    */
    vec_t length() const;

    /**
     * \brief Returns square of length
    */
    vec_t length2() const;

    /**
     * \brief Prints vector components
    */
    void print() const;

    /**
     * \brief Converts Vec2d to its SFML analogue
    */
    operator sf::Vector2f() const;

    /**
     * \brief Converts Vec2d to SFML Vector2i
    */
    operator sf::Vector2i() const;

    /**
     * \brief Fills all fields with NAN
    */
    ~Vec2d();
};


/**
 * \brief Returns normalized vector
*/
Vec2d normalize(const Vec2d &v);


/**
 * \brief Scalar product
*/
vec_t dot(const Vec2d &a, const Vec2d &b);


/**
 * \brief Oriented area in standart basis
*/
vec_t cross(const Vec2d &a, const Vec2d &b);


/**
 * \brief Adds B to A
*/
Vec2d &operator += (Vec2d &a, const Vec2d &b);


/**
 * \brief Subtract B from A
*/
Vec2d &operator -= (Vec2d &a, const Vec2d &b);


/**
 * \brief Multiply each coordinate of A by B
*/
Vec2d &operator *= (Vec2d &a, const Vec2d &b);


/**
 * \brief Divide each coordinate of A by B
*/
Vec2d &operator /= (Vec2d &a, const Vec2d &b);


/**
 * \brief Multiplies each component of A by argument
*/
Vec2d &operator *= (Vec2d &a, vec_t arg);


/**
 * \brief Divides each component of A by argument
*/
Vec2d &operator /= (Vec2d &a, vec_t arg);


/**
 * \brief Returns sum of A and B
*/
Vec2d operator + (const Vec2d &a, const Vec2d &b);


/**
 * \brief Returns A minus B
*/
Vec2d operator - (const Vec2d &a, const Vec2d &b);


/**
 * \brief Multiply each coordinate of A by -1
*/
Vec2d operator - (const Vec2d &a);


/**
 * \brief Returns Vec2d(a.x * b.x, a.y * b.y)
*/
Vec2d operator * (const Vec2d &a, const Vec2d &b);


/**
 * \brief Returns Vec2d(a.x / b.x, a.y / b.y)
*/
Vec2d operator / (const Vec2d &a, const Vec2d &b);


/**
 * \brief Returns Vec2d(a.x * arg, a.y * arg)
*/
Vec2d operator * (const Vec2d &a, vec_t arg);


/**
 * \brief Returns Vec2d(a.x * arg, a.y * arg)
*/
Vec2d operator * (vec_t arg, const Vec2d &a);


/**
 * \brief Returns Vec2d(a.x / arg, a.y / arg)
*/
Vec2d operator / (const Vec2d &a, vec_t arg);


/**
 * \brief Checks if A and B are equal
*/
bool operator == (const Vec2d &a, const Vec2d &b);


/**
 * \brief Checks if two floats are equal
*/
bool isEqual(vec_t a, vec_t b);


#endif
