#ifndef CleanPolygonMaker_H_INCLUDED
#define CleanPolygonMaker_H_INCLUDED

#include <vector>

#include "data/CombinedPolygon.hpp"

using namespace std;
class CleanPolygonMaker{
public:
    static bool combine(CombinedPolygon* cp1, CombinedPolygon* cp2, Checker* checker);
    static int simplifyShareLine(CombinedPolygon* , CombinedPolygon*, Checker* checker);
    
    
private:
    static bool isNeighbor(CombinedPolygon* cp1, CombinedPolygon* cp2);
    static void findStartAndEnd(vector<Vertex*>& vi, vector<Vertex*>& vj, ll middle_i, ll middle_j, ll& start_i, ll& end_i, ll& start_j, ll& end_j);
    static bool findShareVertex(vector<Vertex*>& vi, vector<Vertex*>& vj, ll& middle_i, ll& middle_j);
    static vector<Vertex*> simplifySegment(vector<Vertex*>& origin, ll start, ll end, Checker* checker);
};

#endif // CleanPolygonMaker_H_INCLUDED
