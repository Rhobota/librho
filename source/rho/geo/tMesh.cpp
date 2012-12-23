#include <rho/geo/tMesh.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace rho;
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

tMesh::tMeshFace::tMeshFace()
{
}

void tMesh::tMeshFace::add(int vertexIndex, int texcoordIndex, int normalIndex)
{
    m_vertexIndices.push_back(vertexIndex);
    m_texcoordIndices.push_back(texcoordIndex);
    m_normalIndices.push_back(normalIndex);
}

vector<int>& tMesh::tMeshFace::getVertexIndices()
{
    return m_vertexIndices;
}

vector<int>& tMesh::tMeshFace::getTextureCoordIndices()
{
    return m_texcoordIndices;
}

vector<int>& tMesh::tMeshFace::getNormalIndices()
{
    return m_normalIndices;
}

const vector<int>& tMesh::tMeshFace::getVertexIndices() const
{
    return m_vertexIndices;
}

const vector<int>& tMesh::tMeshFace::getTextureCoordIndices() const
{
    return m_texcoordIndices;
}

const vector<int>& tMesh::tMeshFace::getNormalIndices() const
{
    return m_normalIndices;
}


///////////////////////////////////////////////////////////////////////////////
// tMesh implementation
///////////////////////////////////////////////////////////////////////////////

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
double toDouble(string str, bool* errorFlag)
{
    istringstream in(str);
    double d = 0.0;
    if (!(in >> d))
        *errorFlag = true;
    return d;
}

static
int toInt(string str, bool* errorFlag)
{
    istringstream in(str);
    int i = 0;
    if (!(in >> i))
        *errorFlag = true;
    return i;
}

static
void readFacePart(string str, bool* errorFlag,
        int* vertexIndex, int* texcoordIndex, int* normalIndex)
{
    if (str.length() == 0)
    {
        *errorFlag = true;
        return;
    }
    vector<string> parts = split(str, '/');
    *vertexIndex = toInt(parts[0], errorFlag);
    *texcoordIndex = (parts.size() > 1) ?
        toInt(parts[1], errorFlag) : -1;
    *normalIndex = (parts.size() > 2) ?
        toInt(parts[2], errorFlag) : -1;
}

static
void throwErrorOnLine(string error, string filename, int lineNum)
{
    ostringstream out;
    out << error << " (" << filename << ":" << lineNum << ")";
    throw eResourceAcquisitionError(out.str());
}

tMesh::tMesh(string filename)
{
    ifstream f(filename.c_str());
    if (!f)
        throwErrorOnLine("Cannot open file!", filename, 0);

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
            double x = toDouble(parts[1], &errorFlag);
            double y = toDouble(parts[2], &errorFlag);
            double z = toDouble(parts[3], &errorFlag);
            double w = (parts.size() == 5) ?
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
            double u = toDouble(parts[1], &errorFlag);
            double v = (parts.size() > 2) ?
                toDouble(parts[2], &errorFlag) : 0.0;
            double w = (parts.size() > 3) ?
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
            double x = toDouble(parts[1], &errorFlag);
            double y = toDouble(parts[2], &errorFlag);
            double z = toDouble(parts[3], &errorFlag);
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            m_normals.push_back(tVector(x, y, z, 1.0));
        }

        // Face definition...
        else if (parts[0] == "f")
        {
            if (parts.size() < 4) // there must be at least 3 vertices in a face
                throwErrorOnLine("Incorrect number of parts!",filename,lineNum);
            tMesh::tMeshFace face;
            bool errorFlag = false;
            for (size_t i = 1; i < parts.size(); i++)
            {
                int vertexIndex, texcoordIndex, normalIndex;
                readFacePart(parts[i], &errorFlag,
                        &vertexIndex, &texcoordIndex, &normalIndex);
                face.add(vertexIndex, texcoordIndex, normalIndex);
            }
            if (errorFlag)
                throwErrorOnLine("Double-format error!", filename, lineNum);
            m_faces.push_back(face);
        }

        // Library file...
        else if (parts[0] == "mtllib")
        {
            // todo
        }

        // Library use...
        else if (parts[0] == "usemtl")
        {
            // todo
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

    // Print summary.
    cout << "Loaded mesh: " << filename << "  ("
         << m_vertices.size() << " vertices, "
         << m_faces.size() << " faces)" << endl;
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


}   // namespace geo
}   // namespace rho
