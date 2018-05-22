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
#include <CGAL/enum.h>

#include <CGAL/IO/Color.h>



typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> 			 Polyhedron;
typedef CGAL::Convex_hull_d_traits_3<Kernel> Hull_traits_3;
typedef CGAL::Convex_hull_d<Hull_traits_3>   Convex_hull_3;

typedef Kernel::Point_3 Point_3;


typedef Kernel::Vector_3  Vector;
typedef Kernel::Plane_3 Plane_3;
typedef std::vector<Plane_3> Plane_Vector;
typedef std::vector<const Plane_3> Plane_Vector2;

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

/*
template <class Refs>
struct My_face : public CGAL::HalfedgeDS_face_base<Refs> {
    CGAL::Color color;
};



struct My_items : public CGAL::Polyhedron_items_3 {
    template <class Refs, class Traits>
    struct Face_wrapper {
        typedef My_face<Refs> Face;
    };
};
*/

// A modifier creating a triangle with the incremental builder.
template <class HDS>
class Build_pyramid : public CGAL::Modifier_base<HDS> {
public:

	Point_3 p;
	std::vector<Point_3> v;
    Build_pyramid(std::vector<Point_3> purple_vertices ,Point_3& new_point) {
    	v.reserve(purple_vertices.size());
    	for(int  i = 0 ; i < purple_vertices.size() ; i++){
    		v.push_back(purple_vertices.at(i));
    	}
    	p = new_point;
    	
    }
    void operator()( HDS& hds) {
        // Postcondition: hds is a valid polyhedral surface.
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        B.begin_surface( 3, 1, 6);
        typedef typename HDS::Vertex   Vertex;
        typedef typename Vertex::Point Point;

        B.add_vertex(p);
        for(int  i = 0 ; i < v.size() ; i++){
    		B.add_vertex(v.at(i));
    	}
        B.begin_facet();


        for(int  i = 0 ; i < v.size()+1 ; i++){
    		B.add_vertex_to_facet(i);
    	}

        B.end_facet();
        B.end_surface();
    }
};



typedef Polyhedron::HalfedgeDS HalfedgeDS;


int main(int argc,char* argv[]){


	pvector3 Points;

	int N = (argc>2) && (strcmp(argv[1],"-generate")==0) ? atoi(argv[2]) : 50;
	int radius = N;

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


	std::vector<HF> Facet_V;
	std::cout << "Facets " << std::endl;
	int fn = 0;
  	for (Facet_iterator iter=P.facets_begin(); iter!=P.facets_end(); ++iter){
  		Facet_V.push_back(iter->facet_begin());
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



	CGAL::Geomview_stream gv(CGAL::Bbox_3(-2*radius, -2*radius, -2*radius,2*radius,2*radius,2*radius));
	for( ; vi != Points.end() ; vi++){

		Point_3 new_point = *vi;

		std::vector<int> BLUE,RED;

		fn = 0;
		for(Plane_iterator I = V.begin(); I != V.end() ; I++){
			std::cout<<". "<<std::endl;
			CGAL::Oriented_side orientation = I->oriented_side(new_point)  ;
			if( orientation == CGAL::ON_POSITIVE_SIDE ){
				std::cout<<"\tPositive"<<std::endl;
				RED.push_back(fn);

			}

			else if (orientation == CGAL::ON_NEGATIVE_SIDE ){
				std::cout<<"\tNegative"<<std::endl;	
				BLUE.push_back(fn);

			}
			else {
				std::cout << "\tCollinear"<<std::endl;
				return -1;
			}

			fn++;

		}
		std::cout<<"\nBLUE"<<std::endl;
		std::copy( BLUE.begin(), BLUE.end(), std::ostream_iterator<int>( std::cout, "\n"));

		std::cout<<"\nRED"<<std::endl;
		std::copy( RED.begin(), RED.end(), std::ostream_iterator<int>( std::cout, "\n"));




		std::vector<Point_3> purple_vertices;
		purple_vertices.reserve(3);
		std::vector<Point_3> red_vertices;
		std::vector<Point_3> blue_vertices;
		for(int i = 0 ;i < RED.size() ; i++){


			HF red_facet = Facet_V.at(RED.at(i));
			do{
				red_vertices.push_back(red_facet->vertex()->point()) ;		
			
	 		}while((++red_facet) != Facet_V.at(RED.at(i)));
	 	}

		for(int j = 0 ; j < BLUE.size() ; j++){
			HF blue_facet = Facet_V.at(BLUE.at(j));
			do{
				blue_vertices.push_back(blue_facet->vertex()->point()) ;		

	 		}while((++blue_facet) != Facet_V.at(BLUE.at(j)));
		}
		

		std::vector<Point_3>::iterator result ;
		
		std::sort(red_vertices.begin(), red_vertices.end());
		std::vector<Point_3>::iterator it = std::unique(red_vertices.begin(), red_vertices.end());
		red_vertices.erase(it,red_vertices.end());

		std::sort(blue_vertices.begin(), blue_vertices.end());
		it = std::unique(blue_vertices.begin(), blue_vertices.end());
		blue_vertices.erase(it,blue_vertices.end());


		std::set_intersection(red_vertices.begin(),red_vertices.end() ,blue_vertices.begin(), blue_vertices.end(), std::back_inserter(purple_vertices));
		
		std::cout<< "\nRed :"<<std::endl;

		std::copy( red_vertices.begin(), red_vertices.end(), std::ostream_iterator<Point_3>( std::cout, "\n"));
		std::cout<< "\nBlue :"<<std::endl;


		std::copy( blue_vertices.begin(), blue_vertices.end(), std::ostream_iterator<Point_3>( std::cout, "\n"));

		std::cout<< "\nPurple :"<<std::endl;


		std::copy( purple_vertices.begin(), purple_vertices.end(), std::ostream_iterator<Point_3>( std::cout, "\n"));





		

		
		gv.clear();
		gv.set_line_width(12);
		gv.set_bg_color(CGAL::WHITE);
		
		std::cout << "Drawing Polyhedron.\n";
		
		gv << CGAL::BLUE;
		gv << P;
		gv << CGAL::RED;



		gv << new_point;

		gv << CGAL::PURPLE;

		 
	/*	for(std::vector<Point_3>::iterator iter = purple_vertices.begin();iter != purple_vertices.end();iter++){

			Point_3 p = *iter;
			gv << p;
		}
	*/

	    Build_pyramid<HalfedgeDS> pyramid(purple_vertices,new_point);
	    P.delegate(pyramid);

	    sleep(5);
		gv.clear();
		gv << CGAL::GREEN;
		gv << P;
		

	}
	std::cout << "Enter a key to finish" << std::endl;
	char ch;
	std::cin >> ch;

    
  	return 0;
}