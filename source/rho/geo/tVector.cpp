#include <rho/geo/tVector.h>


namespace rho
{
namespace geo
{


tVector::tVector(double x, double y)
    : x(x), y(y), z(0.0), w(0.0)
{
}

tVector::tVector(double x, double y, double z)
    : x(x), y(y), z(z), w(0.0)
{
}

tVector::tVector(double x, double y, double z, double w)
    : x(x), y(y), z(z), w(w)
{
}

tVector tVector::point(double x, double y)
{
    return tVector(x, y, 0.0, 1.0);
}

tVector tVector::point(double x, double y, double z)
{
    return tVector(x, y, z, 1.0);
}

tVector tVector::origin()
{
    return tVector(0.0, 0.0, 0.0, 1.0);
}

void tVector::bePoint()
{
    w = 1.0;
}

void tVector::beVector()
{
    w = 0.0;
}

bool tVector::isPoint() const
{
    return (w > 0.0);
}

bool tVector::isVector() const
{
    return (w == 0.0);
}

double tVector::lengthSquared() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return x*x + y*y + z*z;
}

double tVector::length() const
{
    /*
     * This returns the length (magnitude) of the vector.
     */
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return sqrt(lengthSquared());
}

double tVector::theta() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return atan2(y, x);    // returns [-pi, pi]
}

double tVector::phi() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return asin(z / length());   // returns [-pi/2, pi/2]
}

void tVector::setLength(double newLength)
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    double ratio = newLength / length();
    x *= ratio;
    y *= ratio;
    z *= ratio;
}

tVector tVector::perp() const
{
    return tVector(-y, x, z);
}

void tVector::rotateZ(double angle)
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

void tVector::rotateX(double angle)
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

void tVector::rotateY(double angle)
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

tVector tVector::unit() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    double len = length();
    return tVector(x/len, y/len, z/len, 0.0);
}

double tVector::distanceSquared(const tVector& other) const
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

double tVector::distance(const tVector& other) const
{
    if (!isPoint())
        throw rho::eLogicError("This operation requires a point.");
    if (!other.isPoint())
        throw rho::eLogicError("This operation requires a point.");
    return sqrt(distanceSquared(other));
}

tVector tVector::cross(const tVector& b) const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    if (!b.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(y * b.z - z * b.y,
                  z * b.x - x * b.z,
                  x * b.y - y * b.x, 0.0);
}

std::ostream& operator<<(std::ostream& stream, const tVector& a)
{
    stream << "[ " << a.x << ", " << a.y << ", " << a.z << ", " << a.w << " ]";
    stream << std::endl;
    return stream;
}

tVector operator-(const tVector& a, const tVector& b)
{
    if (!a.isPoint())
        throw rho::eLogicError("This operation requires a point.");
    if (!b.isPoint())
        throw rho::eLogicError("This operation requires a point.");
    return tVector(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}

void operator-=(tVector& a, const tVector& b)
{
    a = a - b;
}

tVector operator-(const tVector& a)
{
    return tVector(-a.x, -a.y, -a.z, a.w);
}

tVector operator+(const tVector& a, const tVector& b)
{
    if (!(a.isVector() || b.isVector()))
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}

void operator+=(tVector& a, const tVector& b)
{
    a = a + b;
}

tVector operator/(const tVector& a, double c)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x/c, a.y/c, a.z/c, a.w);
}

void operator/=(tVector& a, double c)
{
    a = a / c;
}

tVector operator*(const tVector& a, double c)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x*c, a.y*c, a.z*c, a.w);
}

void operator*=(tVector& a, double c)
{
    a = a * c;
}

tVector operator*(double c, const tVector& a)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x*c, a.y*c, a.z*c, a.w);
}

double operator*(const tVector& a, const tVector& b)
{
    if (!(a.isVector() && b.isVector()))
        throw rho::eLogicError("This operation requires a vector.");
    return a.x*b.x + a.y*b.y + a.z*b.z;
}


}    // namespace geo
}    // namespace rho
