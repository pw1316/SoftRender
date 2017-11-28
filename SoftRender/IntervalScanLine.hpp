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

inline PWbool edgeLessComparator(const EdgeNode &lhs, const EdgeNode &rhs)
{
    if (lhs.m_y < rhs.m_y)
        return true;
    if (lhs.m_y > rhs.m_y)
        return false;

    if (lhs.m_x < rhs.m_x)
        return true;
    if (lhs.m_x > rhs.m_x)
        return false;

    if (lhs.m_dx < rhs.m_dx)
        return true;
    return false;
}

inline PWint getNearestPoly(const PolygonTable &pt, const InPolygonTable &ipl, const ActiveEdgeTable &aet, const ActiveEdgeTable::iterator &edge1, const ActiveEdgeTable::iterator &edge2)
{
    if (edge2 == aet.end())
    {
        return -1;
    }
    auto x = (edge1->m_x + edge2->m_x) / 2;
    auto y = edge1->m_y;
    PWdouble max = -1;
    PWint idx = -1;
    for (auto poly : ipl)
    {
        PWdouble depth = -(pt[poly].m_a * x + pt[poly].m_b * y + pt[poly].m_d) / pt[poly].m_c;
        if (depth > max)
        {
            max = depth;
            idx = poly;
        }
    }
    return idx;
}
