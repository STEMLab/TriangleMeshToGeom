//
// Created by dongmin on 18. 7. 19.
//

#include <detail/feature/plane.h>
#include <compute/VertexComputation.h>
#include "merge_surfaces.h"

#include "detail/algorithm/surface_neighbor.h"
#include "detail/feature/polygon.h"

namespace TM2IN {
    namespace detail {
        namespace algorithm {
            bool merging_invalid_test(vector<HalfEdge *> new_edges, Vector_3 newNormal){

                // TODO : HalfEdgeString
                vector<Vertex*> newVertexList;
                for (int i = 0 ; i < new_edges.size() ; i++){
                    newVertexList.push_back(new_edges[i]->vertices[0]);
                }
                // = HalfEdgeComputation::getFirstVertexList(new_edges);

                // TODO : HalfEdgeString.is_simple()
                vector<Vertex*> sorted_v_list(newVertexList);
                sort(sorted_v_list.begin(), sorted_v_list.end(), VertexComputation::greater);
                for (ull i = 0 ; i < sorted_v_list.size() - 1; i++){
                    if (sorted_v_list[i] == sorted_v_list[i+1]){
                        return 1;
                    }
                }

                // check polygon after merging
                Surface* pSurface = new Surface();
                pSurface->setBoundaryEdgesList(new_edges);
                Plane_3 planeRef = TM2IN::detail::feature::make_simple_plane(newNormal);
                vector<Point_2> point2dList = TM2IN::detail::feature::project_to_plane(pSurface->getVerticesList(), planeRef);
                Polygon_2 polygon = TM2IN::detail::feature::make_CGAL_polygon(point2dList);
                if (!polygon.is_simple() || polygon.orientation() == -1){
                    return 1;
                }
                else{
                    delete pSurface;
                }

                return 0;
            }

            SurfaceMerger::SurfaceMerger(double t1, double t2) : thres1(t1), thres2(t2){
            }

            bool SurfaceMerger::mergeSurfaces(vector<Surface *> surfaceList, vector<Surface *> &result) {
                result.clear();
                //deep copy
                for (ull i = 0 ; i < surfaceList.size() ; i++){
                    result.push_back(new Surface(surfaceList[i]));
                }

                bool hasMerged = false;
                bool isMerged = true;
                ull combined_count = 0;
                while (isMerged){
                    sort(result.begin(), result.end(), Surface::compareLength);
                    if (result.size() == 1) break;
                    for (ull i = 0 ; i < result.size() - 1 ; i++){
                        ull j = i + 1;
                        isMerged = false;
                        while (j < result.size()){
                            if (merge(result[i], result[j]) == 0)
                            {
                                cout << ".";
                                combined_count++;
                                isMerged = true;
                                hasMerged = true;
                                result.erase(result.begin() + j);
                            } else
                                j++;
                        }
                        if (isMerged) i -= 1;
                        printProcess(combined_count, surfaceList.size(), "mergeSurface");
                    }
                }

                return hasMerged;
            }


            int SurfaceMerger::merge(Surface *origin, Surface *piece) {
                // check Polygon is in near polygon or not
                if (!CGALCalculation::isIntersect_BBOX(origin, piece)) return 1;

                // check They are neighbor
                if (!isNeighbor(origin, piece)) return 1;
                if (CGALCalculation::getAngle(origin->normal, piece->normal) > 179.999999){
                    return 1;
                }

                ll origin_size = origin->getVerticesSize();
                ll piece_size = piece->getVerticesSize();

                neighbor_info ni;
                if (constructNeighborInfo(piece, origin, ni)){
                    cerr << "\n" << origin->asJsonText() << endl;
                    cerr << "\n" << piece->asJsonText() <<endl;
                    cerr << CGALCalculation::getAngle(origin->normal, piece->normal)  << endl;
                    return 1;
                }

                /*
                ll piece_middle = -1, origin_middle = -1;
                if (!findShareVertex(piece_vertex_list, origin_vertex_list, piece_middle, origin_middle)) return 1;
                ll lastVertex_piece = -1, lastVertex_origin = -1;
                ll firstVertex_piece = -1, firstVertex_origin = -1;
                if (findStartAndEnd(piece_vertex_list, origin_vertex_list, piece_middle, origin_middle, firstVertex_piece, lastVertex_piece, firstVertex_origin, lastVertex_origin)){
                    cerr << "\n" << origin->asJsonText() << endl;
                    cerr << "\n" << piece->asJsonText() <<endl;
                    cerr << CGALCalculation::getAngle(origin->normal, piece->normal)  << endl;
                    return 1;
                }
                */

                assert (piece->vertex(ni.firstVertex_piece) == origin->vertex(ni.lastVertex_origin));
                assert (piece->vertex(ni.lastVertex_piece) == origin->vertex(ni.firstVertex_origin));

                int seg_num = piece->getSegmentsNumber(ni.lastVertex_piece, ni.firstVertex_piece);

                if (seg_num == -1)
                {
                    cerr << "segment Number is -1" << endl;
                    exit(-1);
                }
                else if (seg_num == 0){
                    return 1;
                }
                else if (seg_num == 1){

                }
                else{
                    if (!check_merge_condition(origin->normal, piece->normal)) {
                        return 1;
                    }
                }

                vector<HalfEdge*> new_edges;

                for (ll j = ni.lastVertex_origin; ; ){
                    new_edges.push_back(origin->boundary_edges(j));
                    j++;
                    if (j == origin_size) j = 0;
                    if (j == ni.firstVertex_origin) break;
                }

                for (ll i = ni.lastVertex_piece; ;){
                    new_edges.push_back(piece->boundary_edges(i));
                    i++;
                    if (i == piece_size) i = 0;
                    if (i == ni.firstVertex_piece) break;
                }

                if (merging_invalid_test(new_edges, origin->normal + piece->normal)) return 1;

                origin->setBoundaryEdgesList(new_edges);
                origin->normal = origin->normal + piece->normal;
                origin->area += piece->area;
                origin->setMBB(piece);
                origin->triangles.insert(origin->triangles.end(), piece->triangles.begin(), piece->triangles.end());

                // assert (!origin->checkDuplicate());

                //TODO : delete old edges

                // delete piece;

                return 0;
            }

            bool SurfaceMerger::check_merge_condition(Vector_3 &big, Vector_3 &small) {
                Vector_3 added = big + small;
                if (is_coplanar(big, small)){
                    double addedAngle = CGALCalculation::getAngle(added, big);
                    return addedAngle <= thres2;
                }
                return false;
            }

            /**
             * check two normal vector are co-planar by using threshold 1
             *
             * @return is it co-planar
             */
            bool SurfaceMerger::is_coplanar(Vector_3 &big, Vector_3 &small) {
                double angle = CGALCalculation::getAngle(big, small);
                return angle <= thres1;
            }

        } // algorithm
    } // detail
}