#include <rho/gl/tLight.h>
#include <rho/eRho.h>

#include <GL/gl.h>


namespace rho
{
namespace gl
{


static
const int kNumLights = 8;

static
bool gEnabled[kNumLights] = { false };


int intToEnum(int i)
{
    switch (i)
    {
        case 0: return GL_LIGHT0;
        case 1: return GL_LIGHT1;
        case 2: return GL_LIGHT2;
        case 3: return GL_LIGHT3;
        case 4: return GL_LIGHT4;
        case 5: return GL_LIGHT5;
        case 6: return GL_LIGHT6;
        case 7: return GL_LIGHT7;
        default:
            throw eImpossiblePath();
    }
}


tLight::tLight()
    : m_loc(geo::tVector::origin()),
      m_ambient(geo::tVector(1.0, 1.0, 1.0, 1.0)),
      m_diffuse(geo::tVector(1.0, 1.0, 1.0, 1.0)),
      m_specular(geo::tVector(1.0, 1.0, 1.0, 1.0)),
      m_index(-1)
{
}

void tLight::enable()
{
    if (m_index != -1)
        throw eLogicError("Cannot enable a light that is not disabled.");

    for (m_index = 0; m_index < kNumLights; m_index++)
    {
        if (gEnabled[m_index] == false)
        {
            gEnabled[m_index] = true;
            glEnable(intToEnum(m_index));
            break;
        }
    }

    if (m_index == kNumLights)
    {
        m_index = -1;
        throw eLogicError("Cannot enable light. Too many lights are enabled!");
    }

    float dColor[] = { (float)m_diffuse.x,  (float)m_diffuse.y,
                       (float)m_diffuse.z,  (float)m_diffuse.w };
    float sColor[] = { (float)m_specular.x, (float)m_specular.y,
                       (float)m_specular.z, (float)m_specular.w };
    float aColor[] = { (float)m_ambient.x,  (float)m_ambient.y,
                       (float)m_ambient.z,  (float)m_ambient.w };
    float loc[]    = { (float)m_loc.x,      (float)m_loc.y,
                       (float)m_loc.z,      (float)m_loc.w };
    glLightfv(intToEnum(m_index), GL_DIFFUSE, dColor);
    glLightfv(intToEnum(m_index), GL_SPECULAR, sColor);
    glLightfv(intToEnum(m_index), GL_AMBIENT, aColor);
    glLightfv(intToEnum(m_index), GL_POSITION, loc);
}

void tLight::disable()
{
    if (m_index == -1)
        throw eLogicError("Cannot disable a light that is not enabled.");
    gEnabled[m_index] = false;
    glDisable(intToEnum(m_index));
    m_index = -1;
}

void tLight::setLocation(geo::tVector p)
{
    m_loc = p;
    if (m_index != -1)
    {
        float loc[]    = { (float)m_loc.x,      (float)m_loc.y,
                           (float)m_loc.z,      (float)m_loc.w };
        glLightfv(intToEnum(m_index), GL_POSITION, loc);
    }
}

void tLight::setAmbientColor(geo::tVector c)
{
    m_ambient = c;
    if (m_index != -1)
    {
        float aColor[] = { (float)m_ambient.x,  (float)m_ambient.y,
                           (float)m_ambient.z,  (float)m_ambient.w };
        glLightfv(intToEnum(m_index), GL_AMBIENT, aColor);
    }
}

void tLight::setDiffuseColor(geo::tVector c)
{
    m_diffuse = c;
    if (m_index != -1)
    {
        float dColor[] = { (float)m_diffuse.x,  (float)m_diffuse.y,
                           (float)m_diffuse.z,  (float)m_diffuse.w };
        glLightfv(intToEnum(m_index), GL_DIFFUSE, dColor);
    }
}

void tLight::setSpecularColor(geo::tVector c)
{
    m_specular = c;
    if (m_index != -1)
    {
        float sColor[] = { (float)m_specular.x, (float)m_specular.y,
                           (float)m_specular.z, (float)m_specular.w };
        glLightfv(intToEnum(m_index), GL_SPECULAR, sColor);
    }
}


}     // namespace gl
}     // namespace rho
