/**
 * \file
 * \brief Contains vector class and prototypes of his functions
*/


/// Vector component type
typedef float vec_t;


/// Math vector with x, y components
class Vector2D {
public:
    vec_t x;    ///< Vector X component
    vec_t y;    ///< Vector Y component


    /**
     * \brief Constructs zero vector
    */
    Vector2D();


    /**
     * \brief Constructs vector from initial components
    */
    Vector2D(vec_t init_x, vec_t init_y);


    /**
     * \brief Constructs vector from its SFML analogue
    */
    Vector2D(const sf::Vector2f &vec);


    /**
     * \brief Constructs vector from SFML Vector2u
    */
    Vector2D(const sf::Vector2u &vec);


    /**
     * \brief Fills all fields with NAN
    */
    ~Vector2D();


    /**
     * \brief Scalar product
    */
    vec_t dot(const Vector2D &arg) const;


    /**
     * \brief Vector length
    */
    vec_t length() const;


    /**
     * \brief Returns normalized vector
    */
    Vector2D normalize() const;


    /**
     * \brief Returns vector reflected from a surface with given normal
    */
    Vector2D reflected(const Vector2D &normal) const;


    /**
     * \brief Prints vector components and its length
    */
    void print() const;


    /**
     * \brief Converts Vector2D to its SFML analogue
    */
    operator sf::Vector2f() const;


    /**
     * \brief Converts Vector2D to SFML Vector2i
    */
    operator sf::Vector2i() const;
};


/**
 * \brief Adds B to A
*/
Vector2D &operator += (Vector2D &a, const Vector2D &b);


/**
 * \brief Subtract B from A
*/
Vector2D &operator -= (Vector2D &a, const Vector2D &b);


/**
 * \brief Multiply each coordinate of A by B
*/
Vector2D &operator *= (Vector2D &a, const Vector2D &b);


/**
 * \brief Multiplies each component of A by argument
*/
Vector2D &operator *= (Vector2D &a, vec_t arg);


/**
 * \brief Divides each component of A by argument
*/
Vector2D &operator /= (Vector2D &a, vec_t arg);


/**
 * \brief Returns sum of A and B
*/
Vector2D operator + (const Vector2D &a, const Vector2D &b);


/**
 * \brief Returns A minus B
*/
Vector2D operator - (const Vector2D &a, const Vector2D &b);


/**
 * \brief Multiply each coordinate of A by -1
*/
Vector2D operator - (const Vector2D &a);


/**
 * \brief Returns Vector2D(a.x * b.x, a.y * b.y)
*/
Vector2D operator * (const Vector2D &a, const Vector2D &b);


/**
 * \brief Returns Vector2D(a.x * arg, a.y * arg)
*/
Vector2D operator * (const Vector2D &a, vec_t arg);


/**
 * \brief Returns Vector2D(a.x / arg, a.y / arg)
*/
Vector2D operator / (const Vector2D &a, vec_t arg);
