#ifndef TRIANGLESPACE_H
#define TRIANGLESPACE_H

#include <string>
#include <vector>

#include "Trinagle.h"
#include "CombinedPolygon.hpp"

using namespace std;

class TriangleSpace
{
public:
    string name;
    TriangleSpace();
    virtual ~TriangleSpace();
    std::vector<Triangle> triangles;
    std::vector<CombinedPolygon*> polygon_list;

    TriangleSpace(string pname) {name = pname;}
    int makePolygons(Checker* ch);
    CombinedPolygon* makeOneBigPolygon(vector<Triangle*> tri_list, CombinedPolygon* cp, bool* checked, int& id, Checker* check);
    void cleaning(Checker* check);

protected:
    void printProcess(unsigned long, unsigned long);
    std::vector<std::vector<Triangle*>> separateByNormal_6();
    vector<vector<Triangle*>> separateByNormal_6(vector<Triangle>& triangles);
private:

};

#endif // TRIANGLESPACE_H