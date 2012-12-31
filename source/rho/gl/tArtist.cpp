#include <rho/gl/tArtist.h>

#include <rho/geo/tBox.h>
#include <rho/geo/tRect.h>
#include <rho/geo/tMesh.h>
#include <rho/img/tImage.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <algorithm>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;
using std::map;
using std::string;


namespace rho
{
namespace gl
{


static
void drawBox(geo::tBox b, nRenderMode rm)
{
    // Normalize b so that we can assume things about b.p1 and b.p2.
    b = b.normalize();

    // Begin.
    if (rm == kFilled)
        glBegin(GL_QUADS);
    else if (rm == kWireframe)
        glBegin(GL_LINE_STRIP);
    else
        throw eLogicError("Invalid nRenderMode.");

    // The following comments assumes you're looking at the cube down
    // the negative z-axis. Positive x is to the right. Positive y is up.

    // Front
    glNormal3d(0.0, 0.0, 1.0);
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);

    // Right
    glNormal3d(1.0, 0.0, 0.0);
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);

    // Top
    glNormal3d(0.0, 1.0, 0.0);
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);

    // Unfortunately, we need to lift the pen here for wireframe drawing.
    if (rm == kWireframe)
    {
        glVertex3d(b.p2.x, b.p2.y, b.p2.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
    }

    // Back
    glNormal3d(0.0, 0.0, -1.0);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);

    // Left
    glNormal3d(-1.0, 0.0, 0.0);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);

    // Bottom
    glNormal3d(0.0, -1.0, 0.0);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);

    // Another annoying wireframe thing.
    if (rm == kWireframe)
    {
        glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    }

    // End.
    glEnd();
}


static
void drawRect(geo::tRect r, nRenderMode rm)
{
    // Begin.
    if (rm == kFilled)
        glBegin(GL_QUADS);
    else if (rm == kWireframe)
        glBegin(GL_LINE_STRIP);
    else
        throw eLogicError("Invalid nRenderMode.");

    // One one face...
    glVertex2d(r.x, r.y);
    glVertex2d(r.x+r.width, r.y);
    glVertex2d(r.x+r.width, r.y+r.height);
    glVertex2d(r.x, r.y+r.height);
    if (rm == kWireframe)
        glVertex2d(r.x, r.y);

    // End.
    glEnd();
}


static
void setMaterial(const geo::tMesh::tMeshMaterial& mat, tArtist* artist)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, const_cast<float*>(mat.ka));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, const_cast<float*>(mat.kd));
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, const_cast<float*>(mat.ke));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, const_cast<float*>(mat.ks));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.ns);
    artist->setTexture(mat.td);
}


static
void drawMesh(const geo::tMesh& m, nRenderMode rm, tArtist* artist)
{
    const vector<geo::tMesh::tMeshMaterial>& mm = m.getMaterials();
    const vector<geo::tVector>&              mv = m.getVertices();
    const vector<geo::tVector>&              mt = m.getTextureCoords();
    const vector<geo::tVector>&              mn = m.getNormals();
    const vector<geo::tMesh::tMeshFace>&     mf = m.getFaces();

    int primativeType = (rm == kFilled) ? GL_POLYGON : GL_LINE_LOOP;

    int lastFm = -1;

    artist->unsetTexture();

    for (size_t i = 0; i < mf.size(); i++)
    {
        const geo::tMesh::tMeshFace& f = mf[i];
        int fm = f.getMaterialIndex();
        if (lastFm != fm)
        {
            setMaterial(mm[fm], artist);
            lastFm = fm;
        }
        const vector<int>& fv = f.getVertexIndices();
        const vector<int>& ft = f.getTextureCoordIndices();
        const vector<int>& fn = f.getNormalIndices();
        glBegin(primativeType);
        for (size_t j = 0; j < fv.size(); j++)
        {
            int v = fv[j];
            int t = ft[j];
            int n = fn[j];
            if (t >= 0) glTexCoord3d(mt[t].x, mt[t].y, mt[t].z);
            if (n >= 0) glNormal3d(mn[n].x, mn[n].y, mn[n].z);
            glVertex3d(mv[v].x, mv[v].y, mv[v].z);
        }
        glEnd();
    }

    artist->unsetTexture();
}


tArtist::tArtist(nRenderMode rm)
    : m_rm(rm)
{
}

void tArtist::draw(const iDrawable& drawable)
{
    const iDrawable* drawablePtr = &drawable;

    const geo::tBox* boxPtr = dynamic_cast<const geo::tBox*>(drawablePtr);
    if (boxPtr != NULL)
    {
        drawBox(*boxPtr, m_rm);
        return;
    }

    const geo::tRect* rectPtr = dynamic_cast<const geo::tRect*>(drawablePtr);
    if (rectPtr != NULL)
    {
        drawRect(*rectPtr, m_rm);
        return;
    }

    const geo::tMesh* meshPtr = dynamic_cast<const geo::tMesh*>(drawablePtr);
    if (meshPtr != NULL)
    {
        drawMesh(*meshPtr, m_rm, this);
        return;
    }

    throw eInvalidArgument("rho::gl::tArtist cannot draw the given object.");
}

void tArtist::setTexture(const string& textureFilePath)
{
    if (textureFilePath == "")
    {
        unsetTexture();
        return;
    }

    map<string, u32>::iterator itr = m_textureFileMap.find(textureFilePath);
    if (itr == m_textureFileMap.end())
    {
        cout << "Loading texture image: " << textureFilePath << endl;
        img::tImage image(textureFilePath, img::kRGBA);
        image.horizontalFlip();   // glTexImage2D() expect row #0 to be on the bottom
        GLuint texId;
        glGenTextures(1, &texId);
        m_textureFileMap[textureFilePath] = (u32)texId;
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                image.width(), image.height(),
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                image.buf());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        GLuint texId = itr->second;
        glBindTexture(GL_TEXTURE_2D, texId);
    }
}

void tArtist::unsetTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

tArtist::~tArtist()
{
    unsetTexture();
    map<string, u32>::iterator itr;
    for (itr = m_textureFileMap.begin(); itr != m_textureFileMap.end(); itr++)
    {
        GLuint texId = itr->second;
        glDeleteTextures(1, &texId);
    }
    m_textureFileMap.clear();
}


}    // namespace gl
}    // namespace rho
