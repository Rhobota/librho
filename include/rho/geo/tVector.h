#ifndef __rho_geo_tVector_h__
#define __rho_geo_tVector_h__


#include <rho/eRho.h>
#include <rho/geo/units.h>

#include <ostream>


namespace rho
{
namespace geo
{


/**
 * This class represents a vector or point in 3-space (specifically, it
 * represents a vector or point in "homogeneous coordinates"). It is capable
 * of performing many vector-geometry operations, and it is particularly
 * useful when used alongside the tMatrix class to do affine transformations.
 */
class tVector
{
    public:

        /**
         * Constructs a vector (not a point).
         */
        tVector(double x, double y);

        /**
         * Constructs a vector (not a point).
         */
        tVector(double x, double y, double z);

        /**
         * Constructs a vector (not a point).
         */
        tVector(double x, double y, double z, double w);

        /**
         * Builds a point at the given values.
         */
        static tVector point(double x, double y);

        /**
         * Builds a point at the given values.
         */
        static tVector point(double x, double y, double z);

        /**
         * Builds the origin point.
         */
        static tVector origin();

    public:

        double x, y, z, w;

    public:

        /**
         * Forces this object to represent a point in 3-space.
         *
         * This objects represents a point when the w-coord is
         * greater than 0.0.
         */
        void bePoint();

        /**
         * Forces this object to represent a vector in 3-space.
         *
         * This object represents a vector when the w-coord is
         * equal to 0.0.
         */
        void beVector();

        /**
         * Returns true if the object represents a point.
         */
        bool isPoint() const;

        /**
         * Returns true if the object represents a vector.
         */
        bool isVector() const;

        /**
         * Returns the squared length (magnitude) of the vector.
         */
        double lengthSquared() const;

        /**
         * Returns the length (magnitude) of the vector.
         */
        double length() const;

        /**
         * Calculates and returns the vector's angle in the xy plane.
         * The returned value is in the range [-pi, pi].
         */
        double theta() const;

        /**
         * Calculates and returns the vector's altitude angle.
         * The returned value is in the range [-pi/2, pi/2].
         */
        double phi() const;

        /**
         * Scales the vector to the new length (magnitude).
         */
        void setLength(double newLength);

        /**
         * Returns a vector perpendicular to the receiver.
         */
        tVector perp() const;

        /**
         * Rotates the vector around the z-axis by 'angle' radians.
         * (Note that the z-coord will not change.)
         */
        void rotateZ(double angle);

        /**
         * Rotates the vector around the x-axis by 'angle' radians.
         * (Note that the x-coord will not change.)
         */
        void rotateX(double angle);

        /**
         * Rotates the vector around the y-axis by 'angle' radians.
         * (Note that the y-coord will not change.)
         */
        void rotateY(double angle);

        /**
         * Calculates and returns the unit vector.
         */
        tVector unit() const;

        /**
         * Calculates and returns the squared distance from this point
         * to the other point.
         */
        double distanceSquared(const tVector& other) const;

        /**
         * Calculates and returns the distance from this point
         * to the other point.
         */
        double distance(const tVector& other) const;

        /**
         * Calculates and returns the cross of this vector with
         * the other vector.
         */
        tVector cross(const tVector& b) const;
};


/**
 * Vectors and points are printable.
 */
std::ostream& operator<<(std::ostream& stream, const tVector& a);

/**
 * Vectors are comparable.
 */
bool operator<(const tVector& a, const tVector& b);
bool operator==(const tVector& a, const tVector& b);

/**
 * Point - Point = Vector
 */
tVector operator-(const tVector& a, const tVector& b);

/**
 * Point - Point = Vector
 */
void operator-=(tVector& a, const tVector& b);

/**
 * Negates a vector or point.
 */
tVector operator-(const tVector& a);

/**
 * Vector + Vector = Vector
 * Vector + Point = Point
 * Point + Vector = Point
 */
tVector operator+(const tVector& a, const tVector& b);

/**
 * Vector + Vector = Vector
 * Vector + Point = Point
 * Point + Vector = Point
 */
void operator+=(tVector& a, const tVector& b);

/**
 * Vector / Scalar = Vector
 */
tVector operator/(const tVector& a, double c);

/**
 * Vector / Scalar = Vector
 */
void operator/=(tVector& a, double c);

/**
 * Vector * Scalar = Vector
 */
tVector operator*(const tVector& a, double c);

/**
 * Vector * Scalar = Vector
 */
void operator*=(tVector& a, double c);

/**
 * Scalar * Vector = Vector
 */
tVector operator*(double c, const tVector& a);

/**
 * Vector * Vector = Scalar (dot product)
 */
double operator*(const tVector& a, const tVector& b);


}    // namespace geo
}    // namespace rho


#endif   // __rho_geo_tVector_h__