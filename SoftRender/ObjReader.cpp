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
        std::cerr << "ObjModel::readObj() failed: Can't open file \"" << path << "\".\n";
        return false;
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
        lineBuffer.clear();
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
    // first pass
    // second pass

    file.close();
    return true;
}
