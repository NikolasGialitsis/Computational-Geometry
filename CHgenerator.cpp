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


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> 			 Polyhedron;
typedef CGAL::Convex_hull_d_traits_3<Kernel> Hull_traits_3;
typedef CGAL::Convex_hull_d<Hull_traits_3>   Convex_hull_3;

typedef Kernel::Point_3 Point_3;

typedef Kernel::Vector_3  Vector;
typedef Kernel::Plane_3 Plane_3;

typedef std::vector<Point_3> pvector3;
typedef std::vector<Point_3>::iterator viterator;

typedef Polyhedron::Vertex  Vertex;
typedef Polyhedron::Vertex_iterator  Vertex_iterator;
typedef Polyhedron::Point_iterator Point_iterator;
typedef Polyhedron::Halfedge_around_facet_circulator 	HF;
typedef Polyhedron::Edge_iterator Edge_iterator;
typedef Polyhedron::Facet_iterator Facet_iterator;


int main(int argc,char* argv[]){

	Convex_hull_3 CH(3);

	int N = (argc>2) && (strcmp(argv[1],"-generate")==0) ? atoi(argv[2]) : 50;
	int radius = 2 *N;

	std::cout<<"Sphere_3 : radius = " << radius << std::endl;
	std::cout << "Get " << N << " random points"<<std::endl;

	CGAL::Random_points_in_sphere_3<Point_3> gen (radius);
	for (int i = 0; i < N; i++) {		
		CH.insert (*gen++);
	} 
	assert(CH.is_valid());

	Polyhedron poly;
	
	CGAL::convex_hull_d_to_polyhedron_3 (CH,poly);
	assert(poly.is_valid());


	
	std::cout << poly.size_of_vertices() << " points on the convex hull written into file output.txt" << std::endl;

	std::ofstream myfile;
	myfile.open ("output.txt");


	CGAL::Geomview_stream gv(CGAL::Bbox_3(-2*radius, -2*radius, -2*radius,2*radius,2*radius,2*radius));

	gv.set_line_width(4);
	gv.set_bg_color(CGAL::Color(0, 200, 200));
	gv << CGAL::BLUE;
	std::cout << "Drawing Polyhedron.\n";

	gv << poly;
	sleep(10);
	gv.clear();
	gv << CGAL::WHITE;
	std::cout << "Drawing Convex Vertices.\n";
  	for (Vertex_iterator iter=poly.vertices_begin(); iter!=poly.vertices_end(); ++iter){
		myfile <<  iter->point() << std::endl;
		gv << iter->point();
  	}
	

  	for (Facet_iterator iter=poly.facets_begin(); iter!=poly.facets_end(); ++iter){
		HF hf = iter->facet_begin();
		do{
			myfile << hf->vertex()->point()<< " , " << std::endl;		
 		}while((++hf) != iter->facet_begin());
  	}
  

	myfile.close();

	std::cout << "Enter a key to finish" << std::endl;
	char ch;
	std::cin >> ch;





  	return 0;
}