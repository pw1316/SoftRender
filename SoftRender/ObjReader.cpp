#include "ObjReader.hpp"

#include <fstream>
#include <iostream>

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

    // first pass
    // second pass

    file.close();
    return true;
}
