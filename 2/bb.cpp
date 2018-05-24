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
#include <CGAL/HalfedgeDS_items_2.h>
#include <CGAL/HalfedgeDS_default.h>


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

typedef Polyhedron::HalfedgeDS HalfedgeDS;


typedef Polyhedron::Facet Facet;

typedef struct FV{//Facet Vertices
	Point_3 p;
	Facet f;
}FV;



bool Sort(Point_3& a,Point_3& b){
  return a.hx() > b.hx();
}



bool PointSort(FV& a,FV& b){
  return a.p < b.p;
}


bool AreSame(FV& a,FV& b){
  return (a.p ==  b.p);
}


struct Plane_equation {
	template <class Facet>
	typename Facet::Plane_3 operator()( Facet& f) {
		typename Facet::Halfedge_handle h = f.halfedge();
		typedef typename Facet::Plane_3 Plane;
		return Plane(h->vertex()->point() , h->next()->vertex()->point(),h->next()->next()->vertex()->point());
	}
};



// A modifier creating a pyramid with the incremental builder.
template <class HDS>
class Build_pyramid : public CGAL::Modifier_base<HDS> {
public:

	Point_3 p;
	std::vector<Point_3> v;
    Build_pyramid(std::vector<FV> purple_vertices ,Point_3& new_point) {
    	v.reserve(purple_vertices.size());
    	for(int  i = 0 ; i < purple_vertices.size() ; i++){
    		v.push_back(purple_vertices.at(i).p);
    		std::cout<<"Purple vertices: "<<purple_vertices.at(i).p<<std::endl;
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

                        

template <class Refs>
struct My_face : public CGAL::HalfedgeDS_face_base<Refs> {
    CGAL::Color color;
    My_face() {}
    My_face( CGAL::Color c) : color(c) {}
};
// An items type using my face.
struct My_items : public CGAL::HalfedgeDS_items_2 {
    template <class Refs, class Traits>
    struct Face_wrapper {
        typedef My_face<Refs> Face;
    };
};
struct My_traits { // arbitrary point type, not used here.
    typedef int  Point_2;
};


typedef CGAL::HalfedgeDS_default<My_traits, My_items> HDS;
typedef HDS::Face                                     Face;
typedef HDS::Face_handle Face_handle;

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


	std::vector<Facet> Facet_V;
	std::cout << "Facets " << std::endl;
	int fn = 0;
  	for (Facet_iterator iter=P.facets_begin(); iter!=P.facets_end(); ++iter){
  		Facet_V.push_back(*iter);
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




		std::vector<FV> purple_vertices;
		purple_vertices.reserve(3);
		std::vector<FV> red_vertices;
		std::vector<FV> blue_vertices;
		for(int i = 0 ;i < RED.size() ; i++){


			Facet red_facet = Facet_V.at(RED.at(i));
			HF circ = red_facet.facet_begin();
			do{

				FV new_struct;
				new_struct.p = circ->vertex()->point();
				new_struct.f = red_facet;
				red_vertices.push_back(new_struct) ;		
			
	 		}while((++circ) !=red_facet.facet_begin());
	 	}



		for(int i = 0 ;i < BLUE.size() ; i++){


			Facet blue_facet = Facet_V.at(BLUE.at(i));
			HF circ = blue_facet.facet_begin();
			do{

				FV new_struct;
				new_struct.p = circ->vertex()->point();
				new_struct.f = blue_facet;
				blue_vertices.push_back(new_struct) ;		
			
	 		}while((++circ) !=blue_facet.facet_begin());
	 	}

		std::vector<FV>::iterator result ;
				
		std::sort(red_vertices.begin(), red_vertices.end(),PointSort);
		std::sort(blue_vertices.begin(), blue_vertices.end(),PointSort);

		std::cout<<"Red Vertices: "<<std::endl;
		for(std::vector<FV>::iterator iter = red_vertices.begin();iter != red_vertices.end();iter++){
			std::cout << "\t" << iter->p << std::endl;
		}
		std::vector<FV>::iterator it = std::unique(red_vertices.begin(), red_vertices.end(),AreSame);
		red_vertices.erase(it,red_vertices.end());
		
		std::cout<<"Blue Vertices: "<<std::endl;
		for(std::vector<FV>::iterator iter = blue_vertices.begin();iter != blue_vertices.end();iter++){
			std::cout << "\t" << iter->p << std::endl;
		}

		
		it = std::unique(blue_vertices.begin(), blue_vertices.end(),AreSame);
		blue_vertices.erase(it,blue_vertices.end());
		



		
		std::cout<<"After:Red Vertices: "<<std::endl;
		for(std::vector<FV>::iterator iter = red_vertices.begin();iter != red_vertices.end();iter++){
			std::cout << "\t" << iter->p << std::endl;
		}
		std::cout<<"After:Blue Vertices: "<<std::endl;
		for(std::vector<FV>::iterator iter = blue_vertices.begin();iter != blue_vertices.end();iter++){
			std::cout << "\t" << iter->p << std::endl;
		}


		std::set_intersection(red_vertices.begin(),red_vertices.end() ,blue_vertices.begin(), blue_vertices.end(), std::back_inserter(purple_vertices),PointSort);
		


		gv.clear();
		gv.set_line_width(12);
		gv.set_bg_color(CGAL::WHITE);		
		std::cout << "Drawing Polyhedron.\n";		
		gv << CGAL::BLUE;
		gv << P;
		gv << CGAL::RED;
		gv << new_point;


		gv << CGAL::PURPLE;
		std::cout<<"Purple Vertices: "<<std::endl;
		for(std::vector<FV>::iterator iter = purple_vertices.begin();iter != purple_vertices.end();iter++){
			std::cout << "\t" << iter->p << std::endl;
			Point_3 p = iter->p;
			gv << p;	
			
		}

		std::cout<<std::endl<<std::endl<<"Remove purple from red"<<std::endl;
		for(std::vector<FV>::iterator iter = purple_vertices.begin();iter != purple_vertices.end();iter++){

			std::cout << "Outer loop" << std::endl;
			Point_3 p = iter->p;
			gv << p;

			
 			for(std::vector<FV>::iterator red_iter = red_vertices.begin();red_iter != red_vertices.end();red_iter++){
 				std::cout<<"\tInner loop"<<std::endl;
 				if(!red_vertices.empty()){
					if((red_iter->p == p)){
						red_vertices.erase(red_iter);
						std::cout<<"\t\terased purple"<<std::endl;
						break;
					}
				}
				else break;
			}
		}
		
		
		

	
	






   	  	Build_pyramid<HalfedgeDS> pyramid(purple_vertices,new_point);
	 	P.delegate(pyramid);



	 	std::cout<<"Erasing red facets "<<std::endl;
		for(std::vector<FV>::iterator iter = red_vertices.begin();iter != red_vertices.end();iter++){

			Facet::Halfedge_handle h = iter->f.halfedge();
			P.erase_facet(iter->f.halfedge());
			std::cout<<"Erased"<<std::endl;
			sleep(10);
		}
		


		


	    //sleep(1);
		//gv.clear();
		gv << CGAL::GREEN;
		gv << P;
		

	}
	std::cout << "Enter a key to finish" << std::endl;
	char ch;
	std::cin >> ch;

    
  	return 0;
}