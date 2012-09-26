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
        tMatrix()
        {
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    m[i][j] = 0.0;
            for (int i = 0; i < 4; i++)
                m[i][i] = 1.0;
        }

    public:

        /**
         * Builds and returns the identity matrix.
         */
        static tMatrix identity()
        {
            return tMatrix();
        }

        /**
         * Builds and returns the zero matrix.
         */
        static tMatrix zero()
        {
            tMatrix mat;
            for (int i = 0; i < 4; i++)
                mat[i][i] = 0.0;
            return mat;
        }

        /**
         * Builds and returns a translation matrix.
         *
         * Any point, when multiplied by this matrix, will be translated
         * by 'dx', 'dy', and 'dz'. This matrix will have no effect
         * on vectors.
         */
        static tMatrix translate(double dx, double dy, double dz)
        {
            tMatrix mat;
            mat[0][3] = dx;
            mat[1][3] = dy;
            mat[2][3] = dz;
            return mat;
        }

        /**
         * Builds and returns a translation matrix.
         */
        static tMatrix translate(const tVector& v)
        {
            return translate(v.x, v.y, v.z);
        }

        /**
         * Builds and returns a scaling matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be scaled by 'sx', 'sy', and 'sz'.
         */
        static tMatrix scale(double sx, double sy, double sz)
        {
            tMatrix mat;
            mat[0][0] = sx;
            mat[1][1] = sy;
            mat[2][2] = sz;
            return mat;
        }

        /**
         * Builds and returns a scaling matrix.
         */
        static tMatrix scale(const tVector& v)
        {
            return scale(v.x, v.y, v.z);
        }

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the z-axis by 'angle' radians.
         */
        static tMatrix rotateZ(double angle)
        {
            tMatrix mat;
            double c = cos(angle);
            double s = sin(angle);
            mat[0][0] = c;
            mat[0][1] = -s;
            mat[1][0] = s;
            mat[1][1] = c;
            return mat;
        }

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the x-axis by 'angle' radians.
         */
        static tMatrix rotateX(double angle)
        {
            tMatrix mat;
            double c = cos(angle);
            double s = sin(angle);
            mat[1][1] = c;
            mat[1][2] = -s;
            mat[2][1] = s;
            mat[2][2] = c;
            return mat;
        }

        /**
         * Builds and returns a rotation matrix.
         *
         * Any point or vector, when multiplied by this matrix,
         * will be rotated about the y-axis by 'angle' radians.
         */
        static tMatrix rotateY(double angle)
        {
            tMatrix mat;
            double c = cos(angle);
            double s = sin(angle);
            mat[0][0] = c;
            mat[0][2] = s;
            mat[2][0] = -s;
            mat[2][2] = c;
            return mat;
        }

        /**
         * Builds and returns a matrix that converts from one frame
         * of reference to another frame of reference.
         *
         * TODO - NEED MORE COMMENTS HERE!
         */
        static tMatrix frame(const tVector& vx, const tVector& vy,
                             const tVector& vz, const tVector& o)
        {
            tMatrix result;
            result[0][0] = vx.x;
            result[1][0] = vx.y;
            result[2][0] = vx.z;
            result[3][0] = 0.0;

            result[0][1] = vy.x;
            result[1][1] = vy.y;
            result[2][1] = vy.z;
            result[3][1] = 0.0;

            result[0][2] = vz.x;
            result[1][2] = vz.y;
            result[2][2] = vz.z;
            result[3][2] = 0.0;

            result[0][3] = o.x;
            result[1][3] = o.y;
            result[2][3] = o.z;
            result[3][3] = 1.0;

            return result;
        }

    public:

        double m[4][4];

        double* operator[](unsigned int i)
        {
            if (!(i < 4))
                throw rho::eLogicError("(!(i < 4))");
            return m[i];
        }

        const double* operator[](unsigned int i) const
        {
            if (!(i < 4))
                throw rho::eLogicError("(!(i < 4))");
            return m[i];
        }

    public:

        /**
         * Calculates and returns the transpose matrix.
         */
        tMatrix transpose() const
        {
            tMatrix result;

            for (int r = 0; r < 4; r++)
                for (int c = 0; c < 4; c++)
                    result.m[r][c] = m[c][r];

            return result;
        }

        /**
         * Calculates and returns the inverse matrix.
         */
        tMatrix inverse() const
        {
            tMatrix result = tMatrix::identity();
            tMatrix m = *this;

            int r, c, i;

            // Try to make m into the identity matrix.
            // (Perform corresponding operations on result.)
            for (r = 0; r < 4; r++)
            {
                int pr = r;

                // Find the best pivot (i.e. largest magnitude).
                for (i = r + 1; i < 4; i++)
                    if (fabs(m[i][r]) > fabs(m[pr][r]))
                        pr = i;

                // Swap rows to put the best pivot on the diagonal.
                for (i = 0; i < 4; i++)
                {
                    std::swap(m[r][i], m[pr][i]);
                    std::swap(result[r][i], result[pr][i]);
                }

                // Normalize the pivot row (i.e. put a one on the diagonal).
                double pivot = m[r][r];
                for (c = 0; c < 4; c++)
                {
                    m[r][c] /= pivot;
                    result[r][c] /= pivot;
                }

                // Introduce zeros above and below the pivot.
                for (i = 0; i < 4; i++)
                {
                    if (i != r)
                    {
                        double scale = m[i][r];
                        for (c = 0; c < 4; c++)
                        {
                            m[i][c] -= scale * m[r][c];
                            result[i][c] -= scale * result[r][c];
                        }
                    }
                }
            }

            return result;
        }
};


/**
 * Matrices are printable.
 */
std::ostream& operator<<(std::ostream& stream, const tMatrix& m)
{
    stream << "| " << m[0][0] << ", " << m[0][1] << ", "
                   << m[0][2] << ", " << m[0][3] << " |"
                   << std::endl;
    stream << "| " << m[1][0] << ", " << m[1][1] << ", "
                   << m[1][2] << ", " << m[1][3] << " |"
                   << std::endl;
    stream << "| " << m[2][0] << ", " << m[2][1] << ", "
                   << m[2][2] << ", " << m[2][3] << " |"
                   << std::endl;
    stream << "| " << m[3][0] << ", " << m[3][1] << ", "
                   << m[3][2] << ", " << m[3][3] << " |"
                   << std::endl;
    stream << std::endl;
    return stream;
}

/**
 * Matrix * Vector = Vector
 * Matrix * Point = Point
 *
 * Vectors can be scaled and rotated.
 * Points can be scaled, rotated, and translated.
 */
tVector operator*(const tMatrix& m, const tVector& a)
{
    return tVector(m[0][0]*a.x + m[0][1]*a.y + m[0][2]*a.z + m[0][3]*a.w,
                   m[1][0]*a.x + m[1][1]*a.y + m[1][2]*a.z + m[1][3]*a.w,
                   m[2][0]*a.x + m[2][1]*a.y + m[2][2]*a.z + m[2][3]*a.w,
                   m[3][0]*a.x + m[3][1]*a.y + m[3][2]*a.z + m[3][3]*a.w);
}

/**
 * Matrix * Matrix = Matrix
 *
 * Two transformations can be combined by multiplying the
 * transformation matrices together!
 */
tMatrix operator*(const tMatrix& a, const tMatrix& b)
{
    tMatrix mat = tMatrix::zero();
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            for (int i = 0; i < 4; i++)
                mat[r][c] += a[r][i] * b[i][c];
    return mat;
}

/**
 * Matrix * Matrix = Matrix
 */
void operator*=(tMatrix& a, const tMatrix& b)
{
    a = a * b;
}


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tMatrix_h__
