//File Streams
#include <iostream>
#include <fstream>
#include <ostream>

//Basic Libraries
#include <vector>
#include <string.h>
#include <iterator>
#include <cassert>

//CGAL Initialization
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Point_3.h>
#include <CGAL/Plane_3.h>
#include <CGAL/Sphere_3.h>
#include <CGAL/Polyhedron_3.h>

//Random Point Set Creation
#include <CGAL/point_generators_3.h>
#include <CGAL/Random.h>

//Convex Hull
#include <CGAL/Convex_hull_d.h>
#include <CGAL/Convex_hull_d_traits_3.h>
#include <CGAL/Convex_hull_d_to_polyhedron_3.h>

//Geomview 
#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Polyhedron_geomview_ostream.h>


#include <CGAL/Polyhedron_incremental_builder_3.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> 			 Polyhedron;
typedef CGAL::Convex_hull_d_traits_3<Kernel> Hull_traits_3;
typedef CGAL::Convex_hull_d<Hull_traits_3>   Convex_hull_3;

typedef Kernel::Point_3 Point_3;


typedef Kernel::Vector_3  Vector;
typedef Kernel::Plane_3 Plane_3;
typedef std::vector<Plane_3> Plane_Vector;
typedef std::vector<Plane_3>::const_iterator Plane_iterator;

typedef std::vector<Point_3> pvector3;
typedef std::vector<Point_3>::iterator viterator;

typedef Polyhedron::Vertex  Vertex;
typedef Polyhedron::Vertex_iterator  Vertex_iterator;
typedef Polyhedron::Point_iterator Point_iterator;
typedef Polyhedron::Halfedge_around_facet_circulator 	HF;
typedef Polyhedron::Edge_iterator Edge_iterator;
typedef Polyhedron::Facet_iterator Facet_iterator;
typedef Polyhedron::Halfedge_handle Halfedge_handle;


bool Sort(Point_3& a,Point_3& b){
  return a.hx() > b.hx();
}

struct Plane_equation {
	template <class Facet>
	typename Facet::Plane_3 operator()( Facet& f) {
		typename Facet::Halfedge_handle h = f.halfedge();
		typedef typename Facet::Plane_3 Plane;
		return Plane(h->vertex()->point() , h->next()->vertex()->point(),h->next()->next()->vertex()->point());
	}
};


int main(int argc,char* argv[]){


	pvector3 Points;

	int N = (argc>2) && (strcmp(argv[1],"-generate")==0) ? atoi(argv[2]) : 50;
	int radius = 2 *N;

	std::cout<<"Sphere_3 : radius = " << radius << std::endl;
	std::cout << "Get " << N << " random points"<<std::endl;

	CGAL::Random_points_in_sphere_3<Point_3> gen (radius);
	for (int i = 0; i < N; i++) {
		Point_3 p = *gen++;		
		Points.push_back(p);
		std::cout <<  p <<std::endl;
	} 

	std::cout<< "\nSorting\n" << std::endl;
	std::sort(Points.begin() , Points.end() , Sort);
	for(viterator vi = Points.begin(); vi != Points.end() ; vi++ ){
		Point_3 p = *vi;
		std::cout << p << std::endl;
	}

	viterator vi = Points.begin();
    Point_3 p = *vi++;

    Point_3 q = *vi++;
    Point_3 r = *vi++;
    Point_3 s = *vi++;
    std::cout<<"Initiate Polyhedron : ";
	std::cout << p.hx() << "  , "<< q.hx() << "  , "<< r.hx() << "  , "<< s.hx() << std::endl;

	Polyhedron P;
	Halfedge_handle h = P.make_tetrahedron(p,q,r,s);
	assert(P.is_valid());

	/*CGAL::Geomview_stream gv(CGAL::Bbox_3(-2*radius, -2*radius, -2*radius,2*radius,2*radius,2*radius));

	gv.set_line_width(4);
	gv.set_bg_color(CGAL::Color(0, 200, 200));
	gv << CGAL::BLUE;
	std::cout << "Drawing Polyhedron.\n";

	gv << P;
	std::cout << "Enter a key to finish" << std::endl;
	char ch;
	std::cin >> ch;
*/


	std::cout << "Facets " << std::endl;
	int fn = 1;
  	for (Facet_iterator iter=P.facets_begin(); iter!=P.facets_end(); ++iter){
		HF hf = iter->facet_begin();
		std::cout << "\nFacet "<< fn++ << std::endl;
		do{
			std::cout << hf->vertex()->point()<< " , " << std::endl;		
 		}while((++hf) != iter->facet_begin());
  	}



	std::transform( P.facets_begin(), P.facets_end(), P.planes_begin(),	Plane_equation());

	Plane_Vector V;
	Plane_iterator I;
	std::copy( P.planes_begin(), P.planes_end(),std::back_inserter(V));



	Point_3 new_point = *vi;

	
	for(Plane_iterator I = V.begin(); I != V.end(); I++){
		std::cout<<". "<<std::endl;
		I->oriented_side(new_point); 
	}

	





  	return 0;
}