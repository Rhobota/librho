#include <rho/geo/tMatrix.h>


namespace rho
{
namespace geo
{


tMatrix::tMatrix()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = 0.0;
    for (int i = 0; i < 4; i++)
        m[i][i] = 1.0;
}

tMatrix tMatrix::identity()
{
    return tMatrix();
}

tMatrix tMatrix::zero()
{
    tMatrix mat;
    for (int i = 0; i < 4; i++)
        mat[i][i] = 0.0;
    return mat;
}

tMatrix tMatrix::translate(double dx, double dy, double dz)
{
    tMatrix mat;
    mat[0][3] = dx;
    mat[1][3] = dy;
    mat[2][3] = dz;
    return mat;
}

tMatrix tMatrix::translate(const tVector& v)
{
    return translate(v.x, v.y, v.z);
}

tMatrix tMatrix::scale(double sx, double sy, double sz)
{
    tMatrix mat;
    mat[0][0] = sx;
    mat[1][1] = sy;
    mat[2][2] = sz;
    return mat;
}

tMatrix tMatrix::scale(const tVector& v)
{
    return scale(v.x, v.y, v.z);
}

tMatrix tMatrix::rotateZ(double angle)
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

tMatrix tMatrix::rotateX(double angle)
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

tMatrix tMatrix::rotateY(double angle)
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

tMatrix tMatrix::frame(const tVector& vx, const tVector& vy,
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

double* tMatrix::operator[](unsigned int i)
{
    if (!(i < 4))
        throw rho::eLogicError("(!(i < 4))");
    return m[i];
}

const double* tMatrix::operator[](unsigned int i) const
{
    if (!(i < 4))
        throw rho::eLogicError("(!(i < 4))");
    return m[i];
}

tMatrix tMatrix::transpose() const
{
    tMatrix result;

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            result.m[r][c] = m[c][r];

    return result;
}

tMatrix tMatrix::inverse() const
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

tVector operator*(const tMatrix& m, const tVector& a)
{
    return tVector(m[0][0]*a.x + m[0][1]*a.y + m[0][2]*a.z + m[0][3]*a.w,
                   m[1][0]*a.x + m[1][1]*a.y + m[1][2]*a.z + m[1][3]*a.w,
                   m[2][0]*a.x + m[2][1]*a.y + m[2][2]*a.z + m[2][3]*a.w,
                   m[3][0]*a.x + m[3][1]*a.y + m[3][2]*a.z + m[3][3]*a.w);
}

tMatrix operator*(const tMatrix& a, const tMatrix& b)
{
    tMatrix mat = tMatrix::zero();
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            for (int i = 0; i < 4; i++)
                mat[r][c] += a[r][i] * b[i][c];
    return mat;
}

void operator*=(tMatrix& a, const tMatrix& b)
{
    a = a * b;
}


}    // namespace geo
}    // namespace rho
