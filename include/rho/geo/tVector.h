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
         * Returns true if the object represents a vector.
         *
         * This object represents a vector when the w-coord is
         * equal to 0.0.
         */
        bool isVector() const;

        /**
         * Forces this object to represent a vector in 3-space.
         */
        void beVector();

        /**
         * Returns true if the object represents a point.
         *
         * This objects represents a point when the w-coord is
         * greater than 0.0. (Usually the w-coord is 1.0.)
         */
        bool isPoint() const;

        /**
         * Forces this object to represent a point in 3-space.
         */
        void bePoint();

        /**
         * Returns the length (magnitude) of the vector.
         */
        double length() const;

        /**
         * Returns the squared length (magnitude) of the vector.
         */
        double lengthSquared() const;

        /**
         * Scales the vector to the new length (magnitude).
         */
        void setLength(double newLength);

        /**
         * Calculates and returns the vector's angle in the xy plane.
         * The returned value is in the range [-pi, pi].
         */
        double theta() const;

        /**
         * Sets the value of theta.
         */
        void setTheta(double t);

        /**
         * Calculates and returns the vector's altitude angle.
         * The returned value is in the range [-pi/2, pi/2].
         */
        double phi() const;

        /**
         * Sets the value of phi.
         */
        void setPhi(double p);

        /**
         * Returns a vector perpendicular to the receiver.
         */
        tVector perp() const;

        /**
         * Returns the vector rotated around the z-axis by 'angle' radians.
         * (Note that the z-coord will not change.)
         */
        tVector rotatedZ(double angle) const;

        /**
         * Returns the vector rotated around the x-axis by 'angle' radians.
         * (Note that the x-coord will not change.)
         */
        tVector rotatedX(double angle) const;

        /**
         * Returns the vector rotated around the y-axis by 'angle' radians.
         * (Note that the y-coord will not change.)
         */
        tVector rotatedY(double angle) const;

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
