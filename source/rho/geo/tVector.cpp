#include <rho/geo/tVector.h>

#include <cmath>


namespace rho
{
namespace geo
{


tVector::tVector()
    : x(0.0), y(0.0), z(0.0), w(0.0)
{
}

tVector::tVector(f64 x, f64 y)
    : x(x), y(y), z(0.0), w(0.0)
{
}

tVector::tVector(f64 x, f64 y, f64 z)
    : x(x), y(y), z(z), w(0.0)
{
}

tVector::tVector(f64 x, f64 y, f64 z, f64 w)
    : x(x), y(y), z(z), w(w)
{
}

tVector tVector::point(f64 x, f64 y)
{
    return tVector(x, y, 0.0, 1.0);
}

tVector tVector::point(f64 x, f64 y, f64 z)
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

f64 tVector::lengthSquared() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return x*x + y*y + z*z;
}

f64 tVector::length() const
{
    /*
     * This returns the length (magnitude) of the vector.
     */
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return sqrt(lengthSquared());
}

f64 tVector::theta() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return atan2(y, x);    // returns [-pi, pi]
}

void tVector::setTheta(f64 t)
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    f64 len = length();
    f64 p = phi();
    x = len * cos(p)*cos(t);
    y = len * cos(p)*sin(t);
    z = len * sin(p);
}

f64 tVector::phi() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return asin(z / length());   // returns [-pi/2, pi/2]
}

void tVector::setPhi(f64 p)
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    f64 len = length();
    f64 t = theta();
    x = len * cos(p)*cos(t);
    y = len * cos(p)*sin(t);
    z = len * sin(p);
}

void tVector::setLength(f64 newLength)
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    f64 ratio = newLength / length();
    x *= ratio;
    y *= ratio;
    z *= ratio;
}

tVector tVector::perp() const
{
    return tVector(-y, x, z);
}

tVector tVector::rotatedZ(f64 angle) const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");

    // First convert the vector to polar coordinates.
    f64 t = atan2(y, x);
    f64 len = sqrt(x*x + y*y);

    // Rotate...
    t += angle;

    // Convert the vector back to Cartesian coordinates.
    tVector vect = *this;
    vect.x = len * cos(t);
    vect.y = len * sin(t);

    return vect;
}

tVector tVector::rotatedX(f64 angle) const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");

    // First convert the vector to polar coordinates.
    f64 t = atan2(z, y);
    f64 len = sqrt(y*y + z*z);

    // Rotate...
    t += angle;

    // Convert the vector back to Cartesian coordinates.
    tVector vect = *this;
    vect.y = len * cos(t);
    vect.z = len * sin(t);

    return vect;
}

tVector tVector::rotatedY(f64 angle) const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");

    // First convert the vector to polar coordinates.
    f64 t = atan2(x, z);
    f64 len = sqrt(x*x + z*z);

    // Rotate...
    t += angle;

    // Convert the vector back to Cartesian coordinates.
    tVector vect = *this;
    vect.z = len * cos(t);
    vect.x = len * sin(t);

    return vect;
}

tVector tVector::unit() const
{
    if (!isVector())
        throw rho::eLogicError("This operation requires a vector.");
    f64 len = length();
    return tVector(x/len, y/len, z/len, 0.0);
}

f64 tVector::distanceSquared(const tVector& other) const
{
    if (!isPoint())
        throw rho::eLogicError("This operation requires a point.");
    if (!other.isPoint())
        throw rho::eLogicError("This operation requires a point.");
    f64 dx = other.x - x;
    f64 dy = other.y - y;
    f64 dz = other.z - z;
    return dx*dx + dy*dy + dz*dz;
}

f64 tVector::distance(const tVector& other) const
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

bool operator<(const tVector& a, const tVector& b)
{
    if (a.x != b.x)
        return a.x < b.x;
    if (a.y != b.y)
        return a.y < b.y;
    if (a.z != b.z)
        return a.z < b.z;
    return a.w < b.w;
}

bool operator==(const tVector& a, const tVector& b)
{
    return (a.x == b.x &&
            a.y == b.y &&
            a.z == b.z &&
            a.w == b.w);
}

tVector operator-(const tVector& a, const tVector& b)
{
    if (a.isVector() && b.isPoint())
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

tVector operator/(const tVector& a, f64 c)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x/c, a.y/c, a.z/c, a.w);
}

void operator/=(tVector& a, f64 c)
{
    a = a / c;
}

tVector operator*(const tVector& a, f64 c)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x*c, a.y*c, a.z*c, a.w);
}

void operator*=(tVector& a, f64 c)
{
    a = a * c;
}

tVector operator*(f64 c, const tVector& a)
{
    if (!a.isVector())
        throw rho::eLogicError("This operation requires a vector.");
    return tVector(a.x*c, a.y*c, a.z*c, a.w);
}

f64 operator*(const tVector& a, const tVector& b)
{
    if (!(a.isVector() && b.isVector()))
        throw rho::eLogicError("This operation requires a vector.");
    return a.x*b.x + a.y*b.y + a.z*b.z;
}


void pack(iWritable* out, const tVector& vect)
{
    pack(out, vect.x);
    pack(out, vect.y);
    pack(out, vect.z);
    pack(out, vect.w);
}

void unpack(iReadable* in, tVector& vect)
{
    unpack(in, vect.x);
    unpack(in, vect.y);
    unpack(in, vect.z);
    unpack(in, vect.w);
}


}    // namespace geo
}    // namespace rho
