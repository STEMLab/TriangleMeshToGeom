//
// Created by dongmin on 18. 1. 17.
//

#include "polyhedron.h"
#include "features/Wall/Surface.h"
#include "features/Wall/Triangle.h"
#include "features/Vertex.h"

#include <algorithm>

typedef CGAL::Polyhedron_3<Kernel>         Polyhedron;

typedef Polyhedron::Halfedge_handle    Halfedge_handle;
typedef Polyhedron::Facet_handle       Facet_handle;
typedef Polyhedron::Vertex_handle      Vertex_handle;
typedef Polyhedron::HalfedgeDS             HalfedgeDS;

typedef Polyhedron::Facet_iterator         Facet_iterator;
typedef Polyhedron::Vertex_iterator        Vertex_iterator;

typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;

namespace TM2IN {
    namespace unused {
// A modifier creating a triangle with the incremental builder.
        template<class HDS>
        class polyhedron_builder : public CGAL::Modifier_base<HDS> {
        public:
            vector<Vertex *> &coords;
            vector<Wall::Triangle *> &surfaces;

            polyhedron_builder(vector<Vertex *> &_vertices, vector<Wall::Triangle *> &_surfaces) : coords(_vertices),
                                                                                            surfaces(_surfaces) {}

            void operator()(HDS &hds) {
                typedef typename HDS::Vertex HDS_Vertex;
                typedef typename HDS_Vertex::Point Point;

                // create a cgal incremental builder
                CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
                B.begin_surface(coords.size(), surfaces.size());

                cout << coords.size() << " , " << surfaces.size() << endl;

                map<Vertex *, int> vertex_index;
                // add the polyhedron vertices
                for (int i = 0; i < (int) coords.size(); i++) {
                    B.add_vertex(Point(coords[i]->coords[0], coords[i]->coords[1], coords[i]->coords[2]));
                    Vertex * pVertex = coords[i];
                    vertex_index[pVertex] = i;
                }

                // add the polyhedron triangles
                for (int i = 0; i < (int) surfaces.size(); i++) {
                    B.begin_facet();
                    vector<Vertex*> vt_list = surfaces[i]->getVerticesList();
                    for (auto vt : vt_list) {
                        int index = vertex_index[vt];
                        B.add_vertex_to_facet(index);
                    }

                    B.end_facet();
                }

                // finish up the surface
                B.end_surface();
            }
        };

        vector<Vertex *> fillHole(vector<Vertex *> &vertices, vector<Wall::Triangle *> &triangles) {
            Polyhedron poly;
            polyhedron_builder<HalfedgeDS> polybuilder(vertices, triangles);
            poly.delegate(polybuilder);
            // Incrementally fill the holes
            unsigned int nb_holes = 0;
            BOOST_FOREACH(Halfedge_handle h, halfedges(poly)) {
                            if (h->is_border()) {
                                std::vector<Facet_handle> patch_facets;
                                std::vector<Vertex_handle> patch_vertices;
                                bool success = CGAL::cpp11::get<0>(
                                        CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole(
                                                poly,
                                                h,
                                                std::back_inserter(patch_facets),
                                                std::back_inserter(patch_vertices),
                                                CGAL::Polygon_mesh_processing::parameters::vertex_point_map(
                                                        get(CGAL::vertex_point, poly)).
                                                        geom_traits(Kernel())));
                                std::cout << " Number of facets in constructed patch: " << patch_facets.size()
                                          << std::endl;
                                std::cout << " Number of vertices in constructed patch: " << patch_vertices.size()
                                          << std::endl;
                                std::cout << " Fairing : " << (success ? "succeeded" : "failed") << std::endl;
                                ++nb_holes;
                            }
                        }
            std::cout << std::endl;
            std::cout << nb_holes << " holes have been filled" << std::endl;

            vector<Wall::Triangle *> new_triangles;
            vector<Vertex *> newVertices;
            for (Vertex_iterator i = poly.vertices_begin(); i != poly.vertices_end(); ++i) {
                Vertex * vt = new Vertex(i->point().x(), i->point().y(), i->point().z());
                newVertices.push_back(vt);
            }

            for (Facet_iterator i = poly.facets_begin(); i != poly.facets_end(); i++) {
                Halfedge_facet_circulator j = i->facet_begin();

                vector<Vertex *> oneSurfaceCoords;
                CGAL_assertion(CGAL::circulator_size(j) >= 3);
                do {
                    ull vtIndex = std::distance(poly.vertices_begin(), j->vertex());
                    oneSurfaceCoords.push_back(newVertices[vtIndex]);
                } while (++j != i->facet_begin());
                Wall::Triangle * triangle = new Wall::Triangle(oneSurfaceCoords);
                new_triangles.push_back(triangle);
            }

            triangles.clear();
            triangles = new_triangles;
            return newVertices;
        }
    }
}