///////////////////////////////////////////////////////////////////////////////
//
// THIS SOFTWARE IS PROVIDED "AS-IS". THERE IS NO WARRANTY OF ANY KIND.
// NEITHER THE AUTHORS NOR THE OHIO STATE UNIVERSITY WILL BE LIABLE
// FOR ANY DAMAGES OF ANY KIND, EVEN IF ADVISED OF SUCH POSSIBILITY.
//
// Copyright (c) 2010 Jyamiti Research Group.
// CS&E Department of the Ohio State University, Columbus, OH.
// All rights reserved.
//
// Author: Sayan Mandal
//
///////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <list>
#include <vector>
#include <cmath>
#include <sstream>
#include <unordered_set>
#include <string>
#include <sys/stat.h>

#ifndef INFINITY
#define INFINITY std::numeric_limits< double >::infinity()
#endif // INFINITY

#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/progress.hpp>
#include <boost/geometry.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>

#include <gudhi/graph_simplicial_complex.h>
#include <gudhi/Simplex_tree.h>


// #include <CGAL/Cartesian.h>

#include<ParseCommand.h>
#include <Legal.h>
#include "SimplicialComplex.h"
#include "Graph.h"

using namespace boost;
using namespace std;
namespace bg = boost::geometry;

extern vector<int> complexSizes;
extern vector<int> accumulativeSizes;
extern int accumulativeSize;
extern float fThreshold;
extern vector<float> vecFiltrationScale;
extern SimplicialTree<bool> domain_complex;


//extern Simplex_tree;// = Gudhi::Simplex_tree<>;
// extern Vertex_handle;// = Simplex_tree::Vertex_handle;
// extern Filtration_value;// = Simplex_tree::Filtration_value;
// extern typeVectorVertex;// = std::vector<Vertex_handle>;
// extern typePairSimplexBool;// = std::pair<Simplex_tree::Simplex_handle, bool>;

// bool isEqual(const std::pair<int, int>& element)
// {
//     return element.first ==  indf;
// }


void simpersPart(vector<pair<int,int>>  &intervals, string simpers_file){

  ifstream ff(simpers_file.c_str());
    // cout<<"simpers part";
    if( ff.good()==false)
    {
        cout<<"simpers file "<<simpers_file<<" does not exist.";
        exit(0);
    }

    int dim, fborn;
    std::string fdead;
    int count=0;
    while(!ff.eof()){
    count ++;
      char sLine[256]="";
      ff.getline(sLine, 256);
      if(strcmp(sLine,"")==0||strlen(sLine)==0)
        return;
      
      stringstream ss;
      ss.str(sLine);
      
      ss >> dim;  ss>>fborn; ss>> fdead;
      
      if(dim==0 || dim==1)
        continue;
      if(fdead == "inf")
        intervals.push_back(make_pair(fborn,-1));
      else
        intervals.push_back(make_pair(fborn,stoi(fdead)));

    }

    ff.close();
    return;
}

int main(int argc, char *argv[] )
{
  int dimensions, noPoints, currentEdgeSize = 0, barcode_count = 0, scalecount = 0, filtration = 0, totinsCount = 0;
  std::vector<pair<int,int>>  intervals;
  string filtration_file, simpers_file;
  std::vector<bg::model::point<float, 3, bg::cs::cartesian>> vPoint;
  Simplex_tree simplexTree;
  
  
  ParseCommand(argc, argv,  simpers_file, filtration_file);
  simpersPart(intervals,simpers_file);
  

  if(openfilt.good()==false)
    {
        cout<<"Point+ Filtration file "<<filtration_file<<" does not exist.";
        exit(0);
    }

  buildFullSimpCom(filtration_file);
  ifstream openfilt(simpers_file.c_str());


 while (!openfilt.eof()) 
    {
    	string sLine;
    	openfilt >> sLine
    	while(sLine[0]!='2')
    		continue;
		int dim, birth, death;
		stringstream ss(sLine);
    	ss >> dim;
    	ss >> birth;
    	if (s1.find("inf") != std::string::npos) {
    		death = -1;
		}
		else{
			ss >> death;
			upDateMatrix( simplexTree, birth, death );
		}

    }//end of reading file




  return EXIT_SUCCESS;
}//end of main
 