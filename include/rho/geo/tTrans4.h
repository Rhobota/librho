#ifndef __rho_geo_tTrans4_h__
#define __rho_geo_tTrans4_h__


#include <rho/geo/tVector.h>
#include <rho/geo/units.h>

#include <ostream>


namespace rho
{
namespace geo
{


/**
 * This class represents a 4x4 matrix that can be used to represent affine
 * transformations (rotation, scaling, and translation). Matrices can be
 * multiplied together to combine transformations.
 */
class tTrans4
{
    private:

        /**
         * This creates the identity matrix, but it is private.
         * Use the identity() static method (or any of the other
         * static methods) to create a matrix.
         */
        tTrans4();

    public:

        /**
         * Builds and returns the identity matrix.
         */
        static tTrans4 identity();

        /**
         * Builds and returns the zero matrix.
         */
        static tTrans4 zero();

        /**
         * Builds and returns a translation matrix.
         *
         * Any point, when multiplied by this matrix, will be translated
         * by 'dx', 'dy', and 'dz'. This matrix will have no effect
         * on vectors.
         */
        static tTrans4 translate(double dx, double dy, double dz);

        /**
         * Builds and returns a translation matrix.
         */
        static tTrans4 translate(const tVector& v);

        /**
         * Builds and returns a scaling matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be scaled by 'sx', 'sy', and 'sz'.
         */
        static tTrans4 scale(double sx, double sy, double sz);

        /**
         * Builds and returns a scaling matrix.
         */
        static tTrans4 scale(const tVector& v);

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the z-axis by 'angle' radians.
         */
        static tTrans4 rotateZ(double angle);

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the x-axis by 'angle' radians.
         */
        static tTrans4 rotateX(double angle);

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the y-axis by 'angle' radians.
         */
        static tTrans4 rotateY(double angle);

        /**
         * Builds and returns a matrix that converts from one frame
         * of reference to another frame of reference.
         *
         * TODO - NEED MORE COMMENTS HERE!
         */
        static tTrans4 frame(const tVector& vx, const tVector& vy,
                             const tVector& vz, const tVector& o);

    public:

        double m[4][4];

        double*       operator[](unsigned int i);
        const double* operator[](unsigned int i) const;

    public:

        /**
         * Calculates and returns the transpose matrix.
         */
        tTrans4 transpose() const;

        /**
         * Calculates and returns the inverse matrix.
         */
        tTrans4 inverse() const;
};


/**
 * Matrices are printable.
 */
std::ostream& operator<<(std::ostream& stream, const tTrans4& m);

/**
 * Matrix * Vector = Vector
 * Matrix * Point = Point
 *
 * Vectors can be scaled and rotated.
 * Points can be scaled, rotated, and translated.
 */
tVector operator*(const tTrans4& m, const tVector& a);

/**
 * Matrix * Matrix = Matrix
 *
 * Two transformations can be combined by multiplying the
 * transformation matrices together!
 */
tTrans4 operator*(const tTrans4& a, const tTrans4& b);

/**
 * Matrix * Matrix = Matrix
 */
void operator*=(tTrans4& a, const tTrans4& b);


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tTrans4_h__
