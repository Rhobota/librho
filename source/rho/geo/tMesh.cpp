#include <rho/geo/tMesh.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace rho;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::vector;


namespace rho
{
namespace geo
{


///////////////////////////////////////////////////////////////////////////////
// tMeshFace implementation
///////////////////////////////////////////////////////////////////////////////

tMesh::tMeshFace::tMeshFace(i32 materialIndex)
    : m_materialIndex(materialIndex)
{
}

void tMesh::tMeshFace::add(i32 vertexIndex, i32 texcoordIndex, i32 normalIndex)
{
    m_vertexIndices.push_back(vertexIndex);
    m_texcoordIndices.push_back(texcoordIndex);
    m_normalIndices.push_back(normalIndex);
}

i32 tMesh::tMeshFace::getMaterialIndex() const
{
    return m_materialIndex;
}

vector<i32>& tMesh::tMeshFace::getVertexIndices()
{
    return m_vertexIndices;
}

vector<i32>& tMesh::tMeshFace::getTextureCoordIndices()
{
    return m_texcoordIndices;
}

vector<i32>& tMesh::tMeshFace::getNormalIndices()
{
    return m_normalIndices;
}

const vector<i32>& tMesh::tMeshFace::getVertexIndices() const
{
    return m_vertexIndices;
}

const vector<i32>& tMesh::tMeshFace::getTextureCoordIndices() const
{
    return m_texcoordIndices;
}

const vector<i32>& tMesh::tMeshFace::getNormalIndices() const
{
    return m_normalIndices;
}

bool meshFaceComparator(const tMesh::tMeshFace& a, const tMesh::tMeshFace& b)
{
    i32 ma = a.getMaterialIndex();
    i32 mb = b.getMaterialIndex();
    if (ma != mb)
        return ma < mb;
    return (&a) < (&b);
}


///////////////////////////////////////////////////////////////////////////////
// tMeshMaterial implementation
///////////////////////////////////////////////////////////////////////////////

tMesh::tMeshMaterial::tMeshMaterial()
{
    for (int i = 0; i < 4; i++)
    {
        ka[i] = kd[i] = ks[i] = 1.0;
        ke[i] = 0.0;
    }
    ns = 0.0;
    d  = 0.0;
}


///////////////////////////////////////////////////////////////////////////////
// tMesh implementation
///////////////////////////////////////////////////////////////////////////////

static
string findDirName(string path)
{
    if (path.length() == 0)
        return "";
    int i = (int) path.length()-1;
    for (; i >= 0; i--)
        if (path[i] == '/' || path[i] == '\\')
            break;
    return path.substr(0, i+1);
}

static
string stripComment(string str)
{
    size_t p = str.find("#");
    if (p == string::npos)
        return str;
    return str.substr(0, p);
}

static
string trim(string str)
{
    // Note: isspace() returns true on all whitespace

    size_t start = 0;
    while (start < str.length() && isspace(str[start])) start++;
    if (start == str.length()) return "";

    size_t end = str.length()-1;
    while (isspace(str[end])) end--;

    return str.substr(start, end+1-start);
}

static
vector<string> split(string str, char delim)
{
    istringstream in(str);
    vector<string> parts;
    string part;
    while (getline(in, part, delim))
        parts.push_back(part);
    return parts;
}

static
vector<string> removeEmptyParts(vector<string> parts)
{
    vector<string> parts2;
    for (size_t i = 0; i < parts.size(); i++)
        if (parts[i].length() > 0)
            parts2.push_back(parts[i]);
    return parts2;
}

static
f64 toDouble(string str, bool* errorFlag)
{
    istringstream in(str);
    f64 d = 0.0;
    if (!(in >> d))
        *errorFlag = true;
    return d;
}

static
i32 toInt(string str, bool* errorFlag)
{
    istringstream in(str);
    i32 i = 0;
    if (!(in >> i))
        *errorFlag = true;
    return i;
}

static
void readFacePart(string str, bool* errorFlag,
        i32* vertexIndex, i32* texcoordIndex, i32* normalIndex)
{
    if (str.length() == 0)
    {
        *errorFlag = true;
        return;
    }
    vector<string> parts = split(str, '/');
    *vertexIndex = toInt(parts[0], errorFlag);
    *texcoordIndex = (parts.size() > 1 && parts[1].length() > 0) ?
        toInt(parts[1], errorFlag) : 0;    // If the texcoord or normal is
    *normalIndex = (parts.size() > 2 && parts[2].length() > 0) ?
        toInt(parts[2], errorFlag) : 0;    // unspecified, we'll set it to 0,
                                           // which is out of the allowed range.
}

static
void throwErrorOnLine(string error, string filename, int lineNum)
{
    ostringstream out;
    out << error << " (" << filename << ":" << lineNum << ")";
    throw eResourceAcquisitionError(out.str());
}

static
vector<tMesh::tMeshMaterial> readMtlFile(string filename)
{
    vector<tMesh::tMeshMaterial> mats;
    ifstream f(filename.c_str());
    if (!f)
        throwErrorOnLine("Cannot open file!", filename, 0);

    string dirname = findDirName(filename);

    string line;
    tMesh::tMeshMaterial currMaterial;
    bool started = false;

    for (int lineNum = 1; getline(f, line); lineNum++)
    {
        line = stripComment(line);
        line = trim(line);
        if (line.length() == 0)
            continue;
        vector<string> parts = split(line, ' ');
        parts = removeEmptyParts(parts);

        // New material...
        if (parts[0] == "newmtl")
        {
            if (parts.size() != 2)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            if (started)
                mats.push_back(currMaterial);
            currMaterial = tMesh::tMeshMaterial();
            currMaterial.name = parts[1];
            started = true;
        }

        // Ambient color...
        else if (parts[0] == "Ka")
        {
            if (!started)
                throwErrorOnLine("A material has not been started!", filename, lineNum);
            if (parts.size() != 4)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            bool errorFlag = false;
            f64 r = toDouble(parts[1], &errorFlag);
            f64 g = toDouble(parts[2], &errorFlag);
            f64 b = toDouble(parts[3], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            currMaterial.ka[0] = (f32)r;
            currMaterial.ka[1] = (f32)g;
            currMaterial.ka[2] = (f32)b;
        }

        // Diffuse color...
        else if (parts[0] == "Kd")
        {
            if (!started)
                throwErrorOnLine("A material has not been started!", filename, lineNum);
            if (parts.size() != 4)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            bool errorFlag = false;
            f64 r = toDouble(parts[1], &errorFlag);
            f64 g = toDouble(parts[2], &errorFlag);
            f64 b = toDouble(parts[3], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            currMaterial.kd[0] = (f32)r;
            currMaterial.kd[1] = (f32)g;
            currMaterial.kd[2] = (f32)b;
        }

        // Emission color...
        else if (parts[0] == "Ke")
        {
            if (!started)
                throwErrorOnLine("A material has not been started!", filename, lineNum);
            if (parts.size() != 4)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            bool errorFlag = false;
            f64 r = toDouble(parts[1], &errorFlag);
            f64 g = toDouble(parts[2], &errorFlag);
            f64 b = toDouble(parts[3], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            currMaterial.ke[0] = (f32)r;
            currMaterial.ke[1] = (f32)g;
            currMaterial.ke[2] = (f32)b;
        }

        // Specular color...
        else if (parts[0] == "Ks")
        {
            if (!started)
                throwErrorOnLine("A material has not been started!", filename, lineNum);
            if (parts.size() != 4)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            bool errorFlag = false;
            f64 r = toDouble(parts[1], &errorFlag);
            f64 g = toDouble(parts[2], &errorFlag);
            f64 b = toDouble(parts[3], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            currMaterial.ks[0] = (f32)r;
            currMaterial.ks[1] = (f32)g;
            currMaterial.ks[2] = (f32)b;
        }

        // Specular "shininess"...
        else if (parts[0] == "Ns")
        {
            if (!started)
                throwErrorOnLine("A material has not been started!", filename, lineNum);
            if (parts.size() != 2)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            bool errorFlag = false;
            f64 s = toDouble(parts[1], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            currMaterial.ns = (f32)s;
        }

        // Dissolved (transparency) ...
        else if (parts[0] == "d" || parts[0] == "Tr")
        {
            if (!started)
                throwErrorOnLine("A material has not been started!", filename, lineNum);
            if (parts.size() != 2)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            bool errorFlag = false;
            f64 d = toDouble(parts[1], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            currMaterial.d = (f32)d;
        }

        // Ambient texture map...
//         else if (parts[0] == "map_Ka")
//         {
//             // todo
//         }

        // Diffuse texture map...
        else if (parts[0] == "map_Kd")
        {
            if (!started)
                throwErrorOnLine("A material has not been started!", filename, lineNum);
            if (parts.size() != 2)
                throwErrorOnLine("Incorrect number of parts!", filename, lineNum);
            currMaterial.td = dirname + parts[1];
        }

        // Specular color texture map...
//         else if (parts[0] == "map_Ks")
//         {
//             // todo
//         }

        // Specular highlight texture map...
//         else if (parts[0] == "map_Ns")
//         {
//             // todo
//         }

        // Alpha texture map...
//         else if (parts[0] == "map_d")
//         {
//             // todo
//         }

        // Illumination model...
//         else if (parts[0] == "illum")
//         {
//             // todo
//         }

        // Reflection map...?
//         else if (parts[0] == "map_refl")
//         {
//             // todo
//         }

        // Other stuff...?
        else
        {
            cerr << "Unsupported mtl line: " << line << endl;
            //throwErrorOnLine("Unsupported feature!", filename, lineNum);
        }
    }

    if (started)
        mats.push_back(currMaterial);

    return mats;
}

tMesh::tMesh(string filename)
{
    ifstream f(filename.c_str());
    if (!f)
        throwErrorOnLine("Cannot open file!", filename, 0);

    m_materials.push_back(tMesh::tMeshMaterial());  // the default material
    i32 currMaterialIndex = 0;

    string dirname = findDirName(filename);

    string line;
    for (int lineNum = 1; getline(f, line); lineNum++)
    {
        line = stripComment(line);
        line = trim(line);
        if (line.length() == 0)
            continue;
        vector<string> parts = split(line, ' ');
        parts = removeEmptyParts(parts);

        // Vertex...
        if (parts[0] == "v")
        {
            if (parts.size() != 4 && parts.size() != 5)
                throwErrorOnLine("Incorrect number of parts!",filename,lineNum);
            bool errorFlag = false;
            f64 x = toDouble(parts[1], &errorFlag);
            f64 y = toDouble(parts[2], &errorFlag);
            f64 z = toDouble(parts[3], &errorFlag);
            f64 w = (parts.size() == 5) ?
                toDouble(parts[4], &errorFlag) : 1.0;
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            m_vertices.push_back(tVector(x/w, y/w, z/w, 1.0));
        }

        // Texture coordinate...
        else if (parts[0] == "vt")
        {
            if (parts.size() != 2 && parts.size() != 3 && parts.size() != 4)
                throwErrorOnLine("Incorrect number of parts!",filename,lineNum);
            bool errorFlag = false;
            f64 u = toDouble(parts[1], &errorFlag);
            f64 v = (parts.size() > 2) ?
                toDouble(parts[2], &errorFlag) : 0.0;
            f64 w = (parts.size() > 3) ?
                toDouble(parts[3], &errorFlag) : 0.0;
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            m_texcoords.push_back(tVector(u, v, w, 1.0));
        }

        // Normal vector...
        else if (parts[0] == "vn")
        {
            if (parts.size() != 4)
                throwErrorOnLine("Incorrect number of parts!",filename,lineNum);
            bool errorFlag = false;
            f64 x = toDouble(parts[1], &errorFlag);
            f64 y = toDouble(parts[2], &errorFlag);
            f64 z = toDouble(parts[3], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            m_normals.push_back(tVector(x, y, z, 1.0));
        }

        // Face definition...
        else if (parts[0] == "f")
        {
            if (parts.size() < 4) // there must be at least 3 vertices in a face
                throwErrorOnLine("Incorrect number of parts!",filename,lineNum);

            tMesh::tMeshFace face(currMaterialIndex);
            bool errorFlag = false;

            for (size_t i = 1; i < parts.size(); i++)
            {
                i32 vertexIndex = -1, texcoordIndex = -1, normalIndex = -1;
                readFacePart(parts[i], &errorFlag,
                        &vertexIndex, &texcoordIndex, &normalIndex);

                vertexIndex -= 1;       // Obj face defs use one-indexed arrays.
                texcoordIndex -= 1;     // This converts to zero-indexed arrays.
                normalIndex -= 1;       //

                if (vertexIndex < -1 || texcoordIndex < -1 || normalIndex < -1)
                    throwErrorOnLine("Bad face def!", filename, lineNum);

                if (vertexIndex >= (i32)m_vertices.size() ||
                    texcoordIndex >= (i32)m_texcoords.size() ||
                    normalIndex >= (i32)m_normals.size())
                    throwErrorOnLine("Bad face def(2)!", filename, lineNum);

                face.add(vertexIndex, texcoordIndex, normalIndex);
            }

            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            m_faces.push_back(face);
        }

        // Library file...
        else if (parts[0] == "mtllib")
        {
            if (parts.size() != 2)
                throwErrorOnLine("Incorrect number of parts!",filename,lineNum);
            vector<tMesh::tMeshMaterial> newMaterials =
                readMtlFile(dirname + parts[1]);
            for (size_t i = 0; i < newMaterials.size(); i++)
                m_materials.push_back(newMaterials[i]);
        }

        // Library use...
        else if (parts[0] == "usemtl")
        {
            if (parts.size() != 2)
                throwErrorOnLine("Incorrect number of parts!",filename,lineNum);
            size_t i;
            for (i = 0; i < m_materials.size(); i++)
                if (m_materials[i].name == parts[1])
                    break;
            if (i == m_materials.size())
                throwErrorOnLine("Unknown material name!", filename, lineNum);
            currMaterialIndex = (i32)i;
        }

        // Named object...
        else if (parts[0] == "o")
        {
            // todo
        }

        // Polygon group...
        else if (parts[0] == "g")
        {
            // todo
        }

        // Smooth shading across polygons...
        else if (parts[0] == "s")
        {
            // todo
        }

        // Parameter space vertices...
        else if (parts[0] == "vp")
        {
            throwErrorOnLine("Unsupported feature!", filename, lineNum);
        }

        // Other stuff...?
        else
        {
            throwErrorOnLine("Unsupported feature!", filename, lineNum);
        }
    }

    // Sometimes there are faces which have no normals associated with them...
    // We will calculate our own normals for such faces!
    for (size_t i = 0; i < m_faces.size(); i++)
    {
        vector<i32>& vertexIndices = m_faces[i].getVertexIndices();
        vector<i32>& normalIndices = m_faces[i].getNormalIndices();
        for (size_t j = 0; j < vertexIndices.size(); j++)
        {
            if (normalIndices[j] != -1)
                continue;
            tVector& a = (j>0) ? m_vertices[vertexIndices[j-1]] :
                                 m_vertices[vertexIndices.back()];
            tVector& b = m_vertices[vertexIndices[j]];
            tVector& c = (j<vertexIndices.size()-1) ?
                                 m_vertices[vertexIndices[j+1]] :
                                 m_vertices[vertexIndices.front()];
            tVector normal = (c-b).cross(a-b).unit();
            normalIndices[j] = (i32)m_normals.size();
            m_normals.push_back(normal);
        }
    }

    // Sort the faces by material index.
    //std::sort(m_faces.begin(), m_faces.end(), meshFaceComparator);
           // this crashes on my mac sometimes... :(  ?

    // Print summary.
    cout << "Loaded mesh: " << filename << "  ("
         << m_vertices.size() << " vertices, "
         << m_faces.size() << " faces)" << endl;
}

vector<tMesh::tMeshMaterial>& tMesh::getMaterials()
{
    return m_materials;
}

vector<tVector>& tMesh::getVertices()
{
    return m_vertices;
}

vector<tVector>& tMesh::getTextureCoords()
{
    return m_texcoords;
}

vector<tVector>& tMesh::getNormals()
{
    return m_normals;
}

vector<tMesh::tMeshFace>& tMesh::getFaces()
{
    return m_faces;
}

const vector<tMesh::tMeshMaterial>& tMesh::getMaterials() const
{
    return m_materials;
}

const vector<tVector>& tMesh::getVertices() const
{
    return m_vertices;
}

const vector<tVector>& tMesh::getTextureCoords() const
{
    return m_texcoords;
}

const vector<tVector>& tMesh::getNormals() const
{
    return m_normals;
}

const vector<tMesh::tMeshFace>& tMesh::getFaces() const
{
    return m_faces;
}


void pack(iWritable* out, const tMesh& mesh)
{
    pack(out, mesh.getMaterials());
    pack(out, mesh.getVertices());
    pack(out, mesh.getTextureCoords());
    pack(out, mesh.getNormals());
    pack(out, mesh.getFaces());
}

void unpack(iReadable* in, tMesh& mesh)
{
    unpack(in, mesh.getMaterials());
    unpack(in, mesh.getVertices());
    unpack(in, mesh.getTextureCoords());
    unpack(in, mesh.getNormals());
    unpack(in, mesh.getFaces());
}


void pack(iWritable* out, const tMesh::tMeshMaterial& mat)
{
    pack(out, mat.name);
    pack(out, mat.td);
    for (int i = 0; i < 4; i++) pack(out, mat.ka[i]);
    for (int i = 0; i < 4; i++) pack(out, mat.kd[i]);
    for (int i = 0; i < 4; i++) pack(out, mat.ke[i]);
    for (int i = 0; i < 4; i++) pack(out, mat.ks[i]);
    pack(out, mat.ns);
    pack(out, mat.d);
}

void unpack(iReadable* in, tMesh::tMeshMaterial& mat)
{
    unpack(in, mat.name);
    unpack(in, mat.td);
    for (int i = 0; i < 4; i++) unpack(in, mat.ka[i]);
    for (int i = 0; i < 4; i++) unpack(in, mat.kd[i]);
    for (int i = 0; i < 4; i++) unpack(in, mat.ke[i]);
    for (int i = 0; i < 4; i++) unpack(in, mat.ks[i]);
    unpack(in, mat.ns);
    unpack(in, mat.d);
}


void pack(iWritable* out, const tMesh::tMeshFace& face)
{
    pack(out, face.getMaterialIndex());
    pack(out, face.getVertexIndices());
    pack(out, face.getTextureCoordIndices());
    pack(out, face.getNormalIndices());
}

void unpack(iReadable* in, tMesh::tMeshFace& face)
{
    i32 matIndex;
    unpack(in, matIndex); face.m_materialIndex = matIndex;
    unpack(in, face.getVertexIndices());
    unpack(in, face.getTextureCoordIndices());
    unpack(in, face.getNormalIndices());
}


}   // namespace geo
}   // namespace rho
