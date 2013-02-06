#ifndef __rho_geo_tMesh_h__
#define __rho_geo_tMesh_h__


#include <rho/iDrawable.h>
#include <rho/iPackable.h>
#include <rho/geo/tVector.h>

#include <string>
#include <vector>


namespace rho
{
namespace geo
{


class tMesh : public bDrawableByArtist
{
    public:

        class tMeshFace
        {
            public:

                tMeshFace();
                tMeshFace(i32 materialIndex);

                void add(i32 vertexIndex, i32 texcoordIndex, i32 normalIndex);

                i32 getMaterialIndex() const;

                std::vector<i32>& getVertexIndices();
                std::vector<i32>& getTextureCoordIndices();
                std::vector<i32>& getNormalIndices();

                const std::vector<i32>& getVertexIndices() const;
                const std::vector<i32>& getTextureCoordIndices() const;
                const std::vector<i32>& getNormalIndices() const;

            private:

                i32              m_materialIndex;
                std::vector<i32> m_vertexIndices;
                std::vector<i32> m_texcoordIndices;
                std::vector<i32> m_normalIndices;

            private:

                friend void unpack(iReadable* in, tMeshFace&);
        };

        class tMeshMaterial
        {
            public:

                tMeshMaterial();

                std::string name;

                std::string td;  // diffuse texture file path

                f32 ka[4];     // ambient color
                f32 kd[4];     // diffuse color
                f32 ke[4];     // emission color
                f32 ks[4];     // specular color
                f32 ns;        // specular "shininess"
                f32 d;         // dissolved value (transparency) [0,1]
        };

    public:

        /**
         * Reads from a .obj file.
         */
        tMesh(std::string filename);

        std::vector<tMeshMaterial>& getMaterials();
        std::vector<tVector>&       getVertices();
        std::vector<tVector>&       getTextureCoords();
        std::vector<tVector>&       getNormals();
        std::vector<tMeshFace>&     getFaces();

        const std::vector<tMeshMaterial>& getMaterials() const;
        const std::vector<tVector>&       getVertices() const;
        const std::vector<tVector>&       getTextureCoords() const;
        const std::vector<tVector>&       getNormals() const;
        const std::vector<tMeshFace>&     getFaces() const;

    private:

        std::vector<tMeshMaterial> m_materials;
        std::vector<tVector>       m_vertices;
        std::vector<tVector>       m_texcoords;
        std::vector<tVector>       m_normals;
        std::vector<tMeshFace>     m_faces;
};


void pack(iWritable* out, const tMesh&);
void unpack(iReadable* in, tMesh&);


void pack(iWritable* out, const tMesh::tMeshMaterial&);
void unpack(iReadable* in, tMesh::tMeshMaterial&);


void pack(iWritable* out, const tMesh::tMeshFace&);
void unpack(iReadable* in, tMesh::tMeshFace&);


}   // namespace geo
}   // namespace rho


#endif   // __rho_geo_tMesh_h__
