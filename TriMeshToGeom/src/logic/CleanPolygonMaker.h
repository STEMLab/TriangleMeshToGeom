#ifndef CleanPolygonMaker_H_INCLUDED
#define CleanPolygonMaker_H_INCLUDED

#include <vector>
#include <map>

#include "logic/CGALCalculation.h"
#include "data/Surface.hpp"
#include "logic/util.h"

using namespace std;
class CleanPolygonMaker{
private:
    static bool isNeighbor(Surface* cp1, Surface* cp2);
    static void findStartAndEnd(vector<Vertex*>& vi, vector<Vertex*>& vj, ll middle_i, ll middle_j, ll& start_i, ll& end_i, ll& start_j, ll& end_j);
    static vector<Vertex*> simplifySegment(vector<Vertex*>& origin, ll start, ll end, Checker* checker);

public:
    static bool combine(Surface* cp1, Surface* cp2, Checker* checker, double degree);
    static bool findShareVertex(vector<Vertex*>& vi, vector<Vertex*>& vj, ll& middle_i, ll& middle_j);
    static bool simplifyLineSegment(Surface* origin, Surface*);
};

#endif // CleanPolygonMaker_H_INCLUDED
