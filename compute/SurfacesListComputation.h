#ifndef SURFACESLISTCALCULATION_H_INCLUDED
#define SURFACESLISTCALCULATION_H_INCLUDED

#include "predefine.h"
#include "features/Surface.hpp"
#include "SurfacesListComputation.h"

using namespace std;

class SurfacesListComputation{
public:
    static int findFirstSurfaceIndexSimilarWithAxis(vector<Surface*>& surfacesList, int axis);
    static void tagID(vector<Surface*>& surfacesList);
    static vector<vector<double>> getMBB(vector<Surface*>& surfacesList);

    static int flattenSurfaces(vector<Surface *> &vector);
};

#endif // SURFACESLISTCALCULATION_H_INCLUDED