#pragma once
#include "stdafx.h"

#include <list>
#include <vector>

#include "Math.hpp"

struct EdgeNode
{
    PWdouble m_x;
    PWdouble m_y;
    PWdouble m_z;
    PWdouble m_ymax;
    PWdouble m_dx;
    PWdouble m_dy;
    PWdouble m_dz;
    PWint m_polygonId;
};

struct PolygonNode
{
    PWint m_id;
    PWdouble m_a, m_b, m_c, m_d;
    Math::Vector3d m_color;
    PWbool m_isIn;
};

using EdgeTable = std::list<EdgeNode>;
using ActiveEdgeTable = std::list<EdgeNode>;
using PolygonTable = std::vector<PolygonNode>;
using InPolygonTable = std::list<PWint>;
