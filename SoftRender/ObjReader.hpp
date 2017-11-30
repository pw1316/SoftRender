#pragma once
#include <exception>
#include <map>
#include <string>
#include <vector>
#include "Math.hpp"

namespace FileReader
{
    struct ObjMaterial
    {
        std::string m_name;
        PWdouble m_ambient[4];
        PWdouble m_diffuse[4];
        PWdouble m_specular[4];
        PWdouble m_shine;
    };

    struct ObjTriangle
    {
        PWint m_vertexIndex[3];
        PWint m_normalIndex[3];
        PWint m_texcoordIndex[3];
        PWint m_faceNormalIndex;
    };

    struct ObjGroup
    {
        std::vector<PWint> m_triangleIndices;
        PWint m_materialIndex;
    };

    class ObjModel
    {
    public:
        void clear()
        {
            m_path.clear();
            m_matpath.clear();
            m_vertices.clear();
            m_vertices.emplace_back(0, 0, 0);
            m_normals.clear();
            m_normals.emplace_back(0, 0, 0);
            m_texcoords.clear();
            m_texcoords.emplace_back(0, 0);
            //m_facetnorms.clear();
            //m_facetnorms.emplace_pack();
            m_triangles.clear();
            m_triangles.emplace_back();
            m_materials.clear();
            m_materials.emplace_back();
            m_groups.clear();
        }
        PWbool readObj(const std::string &path);
        PWbool readMtl(const std::string &path);
        PWbool writeObj(const std::string &path, PWint mode);

        std::string m_path;
        std::string m_matpath;
        std::vector<Math::Vector3d> m_vertices;
        std::vector<Math::Vector3d> m_normals;
        std::vector<Math::Vector2d> m_texcoords;
        //std::vector<PWdouble> m_facetnorms;
        std::vector<ObjTriangle> m_triangles;
        std::vector<ObjMaterial> m_materials;
        std::map<std::string, ObjGroup> m_groups;
    private:
        PWint findMaterial(const std::string &name)
        {
            for (size_t idx = 1; idx < m_materials.size(); ++idx)
            {
                if (m_materials[idx].m_name == name)
                {
                    return static_cast<PWint>(idx);
                }
            }
            throw std::out_of_range("No Such Material");
        }
        std::map<std::string, ObjGroup>::iterator findAndAddGroup(const std::string &name)
        {
            std::map<std::string, ObjGroup>::iterator it = m_groups.find(name);
            if (it == m_groups.end())
            {
                m_groups[name] = ObjGroup();
                it = m_groups.find(name);
            }
            return it;
        }
    };
}