#ifndef __rho_geo_tMesh_h__
#define __rho_geo_tMesh_h__


#include <rho/geo/tVector.h>

#include <string>
#include <vector>


namespace rho
{
namespace geo
{


class tMesh
{
    public:

        class tMeshFace
        {
            public:

                tMeshFace();

                void add(int vertexIndex, int texcoordIndex, int normalIndex);

                std::vector<int>& getVertexIndices();
                std::vector<int>& getTextureCoordIndices();
                std::vector<int>& getNormalIndices();

                const std::vector<int>& getVertexIndices() const;
                const std::vector<int>& getTextureCoordIndices() const;
                const std::vector<int>& getNormalIndices() const;

            private:

                std::vector<int> m_vertexIndices;
                std::vector<int> m_texcoordIndices;
                std::vector<int> m_normalIndices;
        };

    public:

        /**
         * Reads from a .obj file.
         */
        tMesh(std::string filename);

        std::vector<tVector>&   getVertices();
        std::vector<tVector>&   getTextureCoords();
        std::vector<tVector>&   getNormals();
        std::vector<tMeshFace>& getFaces();

        const std::vector<tVector>&   getVertices() const;
        const std::vector<tVector>&   getTextureCoords() const;
        const std::vector<tVector>&   getNormals() const;
        const std::vector<tMeshFace>& getFaces() const;

    private:

        std::vector<tVector>   m_vertices;
        std::vector<tVector>   m_texcoords;
        std::vector<tVector>   m_normals;
        std::vector<tMeshFace> m_faces;
};


}   // namespace geo
}   // namespace rho


#endif   // __rho_geo_tMesh_h__
