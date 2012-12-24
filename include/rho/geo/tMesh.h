#ifndef __rho_geo_tMesh_h__
#define __rho_geo_tMesh_h__


#include <rho/geo/iDrawable.h>
#include <rho/geo/tVector.h>

#include <string>
#include <vector>


namespace rho
{
namespace geo
{


class tMesh : public iDrawable
{
    public:

        class tMeshFace
        {
            public:

                tMeshFace(int materialIndex);

                void add(int vertexIndex, int texcoordIndex, int normalIndex);

                int getMaterialIndex() const;

                std::vector<int>& getVertexIndices();
                std::vector<int>& getTextureCoordIndices();
                std::vector<int>& getNormalIndices();

                const std::vector<int>& getVertexIndices() const;
                const std::vector<int>& getTextureCoordIndices() const;
                const std::vector<int>& getNormalIndices() const;

                bool operator< (const tMeshFace& other) const;

            private:

                int              m_materialIndex;
                std::vector<int> m_vertexIndices;
                std::vector<int> m_texcoordIndices;
                std::vector<int> m_normalIndices;
        };

        class tMeshMaterial
        {
            public:

                tMeshMaterial();

                std::string name;

                float ka[4];     // ambient color
                float kd[4];     // diffuse color
                float ke[4];     // emission color
                float ks[4];     // specular color
                float ns;        // specular "shininess"
                float d;         // dissolved value (transparency) [0,1]
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

        void drawWithArtist(iArtist& artist);

    private:

        std::vector<tMeshMaterial> m_materials;
        std::vector<tVector>       m_vertices;
        std::vector<tVector>       m_texcoords;
        std::vector<tVector>       m_normals;
        std::vector<tMeshFace>     m_faces;
};


}   // namespace geo
}   // namespace rho


#endif   // __rho_geo_tMesh_h__
