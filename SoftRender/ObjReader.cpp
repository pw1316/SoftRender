#include "ObjReader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

PWbool FileReader::ObjModel::readObj(const std::string &path)
{
    clear();
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::ios_base::failure("Can't open file");
    }
    m_path = path;
    std::map<std::string, ObjGroup>::iterator it_group = findAndAddGroup("default");
    PWint materialIndex = 0;

    std::string token;
    std::string line;
    std::stringstream lineBuffer;
    PWdouble x, y, z;
    while (std::getline(file, line))
    {
        lineBuffer.str("");
        lineBuffer.clear();
        lineBuffer.sync();
        while (line.length() > 0 && line[line.length() - 1] == '\\')
        {
            line.pop_back();
            lineBuffer << line;
            std::getline(file, line);
        }
        lineBuffer << line;

        /* Each line */
        if (lineBuffer >> token)
        {
            /* Comment */
            if (token[0] == '#')
            {
                /* Ignore */
            }
            /* Material Lib */
            else if (token[0] == 'm')
            {
                lineBuffer >> token;
                readMtl(token);
            }
            /* Group */
            else if (token == "g")
            {
                lineBuffer >> token;
                it_group = findAndAddGroup(token);
                it_group->second.m_materialIndex = materialIndex;
            }
            /* Material */
            else if (token == "usemtl")
            {
                lineBuffer >> token;
                materialIndex = findMaterial(token);
                it_group->second.m_materialIndex = materialIndex;
            }
            /* Face */
            else if (token == "f")
            {
                PWint idx = 0;
                std::stringstream tokenBuf;
                char dummyChar;
                m_triangles.push_back(ObjTriangle());
                it_group->second.m_triangleIndices.push_back(m_triangles.size() - 1);

                while (lineBuffer >> token)
                {
                    tokenBuf.str("");
                    tokenBuf.clear();
                    tokenBuf.sync();
                    tokenBuf << token;

                    /* v */
                    tokenBuf >> x;
                    /* has t or n? */
                    if (tokenBuf >> dummyChar)
                    {
                        /* t */
                        if (tokenBuf >> y)
                        {
                            m_triangles.back().m_texcoordIndex[idx] = y;
                        }
                        else
                        {
                            y = 0;
                            tokenBuf.clear();
                        }
                        /* n */
                        if (tokenBuf >> dummyChar >> z)
                        {
                            m_triangles.back().m_normalIndex[idx] = z;
                        }
                        else
                        {
                            z = 0;
                        }
                    }

                    if (idx < 3)
                    {
                        m_triangles.back().m_vertexIndex[idx] = x;
                        m_triangles.back().m_texcoordIndex[idx] = y;
                        m_triangles.back().m_normalIndex[idx] = z;
                    }
                    else
                    {
                        m_triangles.push_back(ObjTriangle());
                        it_group->second.m_triangleIndices.push_back(m_triangles.size() - 1);
                        m_triangles.back().m_vertexIndex[0] = m_triangles[m_triangles.size() - 2].m_vertexIndex[0];
                        m_triangles.back().m_texcoordIndex[0] = m_triangles[m_triangles.size() - 2].m_texcoordIndex[0];
                        m_triangles.back().m_normalIndex[0] = m_triangles[m_triangles.size() - 2].m_normalIndex[0];
                        m_triangles.back().m_vertexIndex[1] = m_triangles[m_triangles.size() - 2].m_vertexIndex[2];
                        m_triangles.back().m_texcoordIndex[1] = m_triangles[m_triangles.size() - 2].m_texcoordIndex[2];
                        m_triangles.back().m_normalIndex[1] = m_triangles[m_triangles.size() - 2].m_normalIndex[2];
                        m_triangles.back().m_vertexIndex[2] = x;
                        m_triangles.back().m_texcoordIndex[2] = y;
                        m_triangles.back().m_normalIndex[2] = z;
                    }
                    ++idx;
                }
            }
            /* Vertex */
            else if (token == "v")
            {
                lineBuffer >> x >> y >> z;
                m_vertices.emplace_back(x, y, z);
            }
            /* Normal */
            else if (token == "vn")
            {
                lineBuffer >> x >> y >> z;
                m_normals.emplace_back(x, y, z);
            }
            /* Texcoord */
            else if (token == "vt")
            {
                lineBuffer >> x >> y;
                m_texcoords.emplace_back(x, y);
            }
        }
    }

    file.close();
    return true;
}

PWbool FileReader::ObjModel::readMtl(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::ios_base::failure("Can't open file");
    }
    m_matpath = path;

    std::string token;
    std::string line;
    std::stringstream lineBuffer;
    PWdouble x, y, z;
    while (std::getline(file, line))
    {
        lineBuffer.str("");
        lineBuffer.clear();
        lineBuffer.sync();
        while (line.length() > 0 && line[line.length() - 1] == '\\')
        {
            line.pop_back();
            lineBuffer << line;
            std::getline(file, line);
        }
        lineBuffer << line;

        /* Each line */
        if (lineBuffer >> token)
        {
            /* Comment */
            if (token[0] == '#')
            {
                /* Ignore */
            }
            /* New Material */
            else if (token == "newmtl")
            {
                lineBuffer >> token;
                m_materials.emplace_back();
                m_materials.back().m_name = token;
            }
            /* Ambient */
            else if (token == "Ka")
            {
                lineBuffer >> x >> y >> z;
                m_materials.back().m_ambient[0] = x;
                m_materials.back().m_ambient[1] = y;
                m_materials.back().m_ambient[2] = z;
                m_materials.back().m_ambient[3] = 1;
            }
            /* Diffuse */
            else if (token == "Kd")
            {
                lineBuffer >> x >> y >> z;
                m_materials.back().m_diffuse[0] = x;
                m_materials.back().m_diffuse[1] = y;
                m_materials.back().m_diffuse[2] = z;
                m_materials.back().m_diffuse[3] = 1;
            }
            /* Specular */
            else if (token == "Ks")
            {
                lineBuffer >> x >> y >> z;
                m_materials.back().m_specular[0] = x;
                m_materials.back().m_specular[1] = y;
                m_materials.back().m_specular[2] = z;
                m_materials.back().m_specular[3] = 1;
            }
            /* Specular exponent */
            else if (token == "Ns")
            {
                lineBuffer >> x;
                m_materials.back().m_shine = x / 1000 * 128;
            }
        }
    }
}
