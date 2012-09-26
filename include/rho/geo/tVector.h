#ifndef __rho_geo_tVector_h__
#define __rho_geo_tVector_h__


#include <rho/eRho.h>
#include <rho/geo/units.h>

#include <cmath>
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
        tVector(double x, double y)
            : x(x), y(y), z(0.0), w(0.0)
        {
        }

        /**
         * Constructs a vector (not a point).
         */
        tVector(double x, double y, double z)
            : x(x), y(y), z(z), w(0.0)
        {
        }

        /**
         * Constructs a vector (not a point).
         */
        tVector(double x, double y, double z, double w)
            : x(x), y(y), z(z), w(w)
        {
        }

        /**
         * Builds a point at the given values.
         */
        static tVector point(double x, double y)
        {
            return tVector(x, y, 0.0, 1.0);
        }

        /**
         * Builds a point at the given values.
         */
        static tVector point(double x, double y, double z)
        {
            return tVector(x, y, z, 1.0);
        }

        /**
         * Builds the origin point.
         */
        static tVector origin()
        {
            return tVector(0.0, 0.0, 0.0, 1.0);
        }

    public:

        double x, y, z, w;

    public:

        /**
         * Forces this object to represent a point in 3-space.
         *
         * This objects represents a point when the w-coord is
         * greater than 0.0.
         */
        void bePoint()
        {
            w = 1.0;
        }

        /**
         * Forces this object to represent a vector in 3-space.
         *
         * This object represents a vector when the w-coord is
         * equal to 0.0.
         */
        void beVector()
        {
            w = 0.0;
        }

        /**
         * Returns true if the object represents a point.
         */
        bool isPoint() const
        {
            return (w > 0.0);
        }

        /**
         * Returns true if the object represents a vector.
         */
        bool isVector() const
        {
            return (w == 0.0);
        }

        /**
         * Returns the squared length (magnitude) of the vector.
         */
        double lengthSquared() const
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");
            return x*x + y*y + z*z;
        }

        /**
         * Returns the length (magnitude) of the vector.
         */
        double length() const
        {
            /*
             * This returns the length (magnitude) of the vector.
             */
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");
            return sqrt(lengthSquared());
        }

        /**
         * Calculates and returns the vector's angle in the xy plane.
         * The returned value is in the range [-pi, pi].
         */
        double theta() const
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");
            return atan2(y, x);    // returns [-pi, pi]
        }

        /**
         * Calculates and returns the vector's altitude angle.
         * The returned value is in the range [-pi/2, pi/2].
         */
        double phi() const
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");
            return asin(z / length());   // returns [-pi/2, pi/2]
        }

        /**
         * Scales the vector to the new length (magnitude).
         */
        void setLength(double newLength)
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");
            double ratio = newLength / length();
            x *= ratio;
            y *= ratio;
            z *= ratio;
        }

        /**
         * Returns a vector perpendicular to the receiver.
         */
        tVector perp() const
        {
            return tVector(-y, x, z);
        }

        /**
         * Rotates the vector around the z-axis by 'angle' radians.
         * (Note that the z-coord will not change.)
         */
        void rotateZ(double angle)
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");

            // First convert the vector to polar coordinates.
            double t = atan2(y, x);
            double len = sqrt(x*x + y*y);

            // Rotate...
            t += angle;

            // Convert the vector back to Cartesian coordinates.
            x = len * cos(t);
            y = len * sin(t);
        }

        /**
         * Rotates the vector around the x-axis by 'angle' radians.
         * (Note that the x-coord will not change.)
         */
        void rotateX(double angle)
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");

            // First convert the vector to polar coordinates.
            double t = atan2(z, y);
            double len = sqrt(y*y + z*z);

            // Rotate...
            t += angle;

            // Convert the vector back to Cartesian coordinates.
            y = len * cos(t);
            z = len * sin(t);
        }

        /**
         * Rotates the vector around the y-axis by 'angle' radians.
         * (Note that the y-coord will not change.)
         */
        void rotateY(double angle)
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");

            // First convert the vector to polar coordinates.
            double t = atan2(x, z);
            double len = sqrt(x*x + z*z);

            // Rotate...
            t += angle;

            // Convert the vector back to Cartesian coordinates.
            z = len * cos(t);
            x = len * sin(t);
        }

        /**
         * Calculates and returns the unit vector.
         */
        tVector unit() const
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");
            double len = length();
            return tVector(x/len, y/len, z/len, 0.0);
        }

        /**
         * Calculates and returns the squared distance from this point
         * to the other point.
         */
        double distanceSquared(const tVector& other) const
        {
            if (!isPoint())
                throw rho::eLogicError("This operation requires a point.");
            if (!other.isPoint())
                throw rho::eLogicError("This operation requires a point.");
            double dx = other.x - x;
            double dy = other.y - y;
            double dz = other.z - z;
            return dx*dx + dy*dy + dz*dz;
        }

        /**
         * Calculates and returns the distance from this point
         * to the other point.
         */
        double distance(const tVector& other) const
        {
            if (!isPoint())
                throw rho::eLogicError("This operation requires a point.");
            if (!other.isPoint())
                throw rho::eLogicError("This operation requires a point.");
            return sqrt(distanceSquared(other));
        }

        /**
         * Calculates and returns the cross of this vector with
         * the other vector.
         */
        tVector cross(const tVector& b) const
        {
            if (!isVector())
                throw rho::eLogicError("This operation requires a vector.");
            if (!b.isVector())
                throw rho::eLogicError("This operation requires a vector.");
            return tVector(y * b.z - z * b.y,
                          z * b.x - x * b.z,
                          x * b.y - y * b.x, 0.0);
        }
};


/**
 * Vectors and points are printable.
 */
std::ostream& operator<<(std::ostream& stream, const tVector& a)
{
    stream << "[ " << a.x << ", " << a.y << ", " << a.z << ", " << a.w << " ]";
    stream << std::endl;
    return stream;
}

/**
 * Point - Point = Vector
 */
tVector operator-(const tVector& a, const tVector& b)
{
    if (!a.isPoint())
        throw rho::eLogicError("This operation requires a point.");
    if (!b.isPoint())
        throw rho::eLogicError("This operation requires a point.");
    return tVector(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}

/**
 * Point - Point = Vector
 */
void operator-=(tVector& a, const tVector& b)
{
    a = a - b;
}

/**
 * Negates a vector or point.
 */
tVector operator-(const tVector& a)
{
    return tVector(-a.x, -a.y, -a.z, a.w);
}

/**
 * Vector + Vector = Vector
 * Vector + Point = Point
 * Point + Vector = Point
 */
tVector operator+(const tVector& a, const tVector& b)
{
    if (!(a.isVector() || b.isVector()))
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}

/**
 * Vector + Vector = Vector
 * Vector + Point = Point
 * Point + Vector = Point
 */
void operator+=(tVector& a, const tVector& b)
{
    a = a + b;
}

/**
 * Vector / Scalar = Vector
 */
tVector operator/(const tVector& a, double c)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x/c, a.y/c, a.z/c, a.w);
}

/**
 * Vector / Scalar = Vector
 */
void operator/=(tVector& a, double c)
{
    a = a / c;
}

/**
 * Vector * Scalar = Vector
 */
tVector operator*(const tVector& a, double c)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x*c, a.y*c, a.z*c, a.w);
}

/**
 * Vector * Scalar = Vector
 */
void operator*=(tVector& a, double c)
{
    a = a * c;
}

/**
 * Scalar * Vector = Vector
 */
tVector operator*(double c, const tVector& a)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x*c, a.y*c, a.z*c, a.w);
}

/**
 * Vector * Vector = Scalar (dot product)
 */
double operator*(const tVector& a, const tVector& b)
{
    if (!(a.isVector() && b.isVector()))
        throw rho::eLogicError("This operation requires a vector.");
    return a.x*b.x + a.y*b.y + a.z*b.z;
}


}    // namespace geo
}    // namespace rho


#endif   // __rho_geo_tVector_h__
