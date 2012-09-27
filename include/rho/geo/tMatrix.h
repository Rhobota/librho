#ifndef __rho_geo_tMatrix_h__
#define __rho_geo_tMatrix_h__


#include <rho/geo/tVector.h>
#include <rho/geo/units.h>

#include <cmath>
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
class tMatrix
{
    private:

        /**
         * This creates the identity matrix, but it is private.
         * Use the identity() static method (or any of the other
         * static methods) to create a matrix.
         */
        tMatrix();

    public:

        /**
         * Builds and returns the identity matrix.
         */
        static tMatrix identity();

        /**
         * Builds and returns the zero matrix.
         */
        static tMatrix zero();

        /**
         * Builds and returns a translation matrix.
         *
         * Any point, when multiplied by this matrix, will be translated
         * by 'dx', 'dy', and 'dz'. This matrix will have no effect
         * on vectors.
         */
        static tMatrix translate(double dx, double dy, double dz);

        /**
         * Builds and returns a translation matrix.
         */
        static tMatrix translate(const tVector& v);

        /**
         * Builds and returns a scaling matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be scaled by 'sx', 'sy', and 'sz'.
         */
        static tMatrix scale(double sx, double sy, double sz);

        /**
         * Builds and returns a scaling matrix.
         */
        static tMatrix scale(const tVector& v);

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the z-axis by 'angle' radians.
         */
        static tMatrix rotateZ(double angle);

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the x-axis by 'angle' radians.
         */
        static tMatrix rotateX(double angle);

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the y-axis by 'angle' radians.
         */
        static tMatrix rotateY(double angle);

        /**
         * Builds and returns a matrix that converts from one frame
         * of reference to another frame of reference.
         *
         * TODO - NEED MORE COMMENTS HERE!
         */
        static tMatrix frame(const tVector& vx, const tVector& vy,
                             const tVector& vz, const tVector& o);

    public:

        double m[4][4];

        double*       operator[](unsigned int i);
        const double* operator[](unsigned int i) const;

    public:

        /**
         * Calculates and returns the transpose matrix.
         */
        tMatrix transpose() const;

        /**
         * Calculates and returns the inverse matrix.
         */
        tMatrix inverse() const;
};


/**
 * Matrices are printable.
 */
std::ostream& operator<<(std::ostream& stream, const tMatrix& m);

/**
 * Matrix * Vector = Vector
 * Matrix * Point = Point
 *
 * Vectors can be scaled and rotated.
 * Points can be scaled, rotated, and translated.
 */
tVector operator*(const tMatrix& m, const tVector& a);

/**
 * Matrix * Matrix = Matrix
 *
 * Two transformations can be combined by multiplying the
 * transformation matrices together!
 */
tMatrix operator*(const tMatrix& a, const tMatrix& b);

/**
 * Matrix * Matrix = Matrix
 */
void operator*=(tMatrix& a, const tMatrix& b);


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tMatrix_h__
