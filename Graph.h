#include <iostream>
#include <vector>
#include <queue>

using namespace std;
using Simplex_tree = Gudhi::Simplex_tree<>;
using Vertex_handle = Simplex_tree::Vertex_handle;
using Filtration_value = Simplex_tree::Filtration_value;
using typeVectorVertex = std::vector<Vertex_handle>;
using typePairSimplexBool = std::pair<Simplex_tree::Simplex_handle, bool>;

map <int,int> threesimplex_to_node; // maps index of tetrahedrons from simplex->first to dual-graph-nodes->second
map <int,int> twosimplex_to_tri; // maps index of traingles from simplex->first to dual-graph-edges->second
vector<int> graphNode;//negative weight: node is red
vector<vector<int>> adjMatrix;  // negative weight: not present, inf= turned red
//////////////////////////////////////////////////////////////////////////////////////////////
// See the picture here: https://www.srcmake.com/uploads/5/3/9/0/5390645/adjmatrix_1_orig.jpg
void FormAdjMatrix(Simplex_tree simplexTree, vector<int> simpcount)
{
//    for (auto f_simplex : simplexTree.filtration_simplex_range()) {
//        std::cout << "   "
//        << "[" << simplexTree.filtration(f_simplex) << "] ";
//
//        for (auto vertex : simplexTree.simplex_vertex_range(f_simplex)) {
//            std::cout << static_cast<int>(vertex) << " ";
//        }
//        for(auto cobo : simplexTree.cofaces_simplex_range(f_simplex,1)){
//            std::cout << "cobo: "
//            << "[" << simplexTree.filtration(cobo) << "] ";
//            for (auto vertex : simplexTree.simplex_vertex_range(cobo)) {
//                std::cout << static_cast<int>(vertex) << " ";
//            }
//        }
//        cout<<endl;
//    }
//
    const int n = simpcount[3]+2; // No of tetrahedrons, last two: one for infinite, one for final sink
    // Our adjacency list.
    vector<vector<int>> adjMatrixbuff(n, vector<int> (n, -1));  // negative weight: not present, inf= turned red
    vector<int> buffGnode(n,INT_MAX);
    graphNode = buffGnode;
    
    //SKIP non tetrahedrons, iterate through all simplex
    for (auto f_simplex : simplexTree.filtration_simplex_range()) {
        
        std::cout << "   "
        << "[" << simplexTree.filtration(f_simplex) << "] ";
        if(simplexTree.dimension(f_simplex)==3){
            
            //Demo Print vertices
        for (auto vertex : simplexTree.simplex_vertex_range(f_simplex)) {
             std::cout << static_cast<int>(vertex) << " ";
        }
            //Since all are red, connect to final sink
            adjMatrix[threesimplex_to_node[simplexTree.filtration(f_simplex)]][n-1] = INT_MAX;
        // Each boundary simplex of the tetrahedron
        for (auto face : simplexTree.boundary_simplex_range(f_simplex)) {
            cout<<"bound: ";
            
            cout<<"nocf: "<<(simplexTree.cofaces_simplex_range(face,1)).size()<<" ";
            vector<int> threesimp;
            if((simplexTree.cofaces_simplex_range(face,1)).size()<2){//connects to inf
                auto tetra =simplexTree.cofaces_simplex_range(face, 1)[0];
                adjMatrix[threesimplex_to_node[simplexTree.filtration(tetra)]][n-2] = INT_MAX;//second last is infinite vertex
            }
            else{//both side tetra
                
                for(auto tetra : simplexTree.cofaces_simplex_range(face, 1)){
                    cout<<"Bounding them: "<<simplexTree.filtration(tetra);
                    threesimp.push_back(simplexTree.filtration(tetra));
                    cout<<"map: "<<threesimplex_to_node[threesimp[0]]<<" "<<threesimplex_to_node[threesimp[1]];
                }
                getchar();
                //set edge to infinite
                adjMatrix[threesimplex_to_node[threesimp[0]]][threesimplex_to_node[threesimp[1]]] = INT_MAX;
                cout<<"after";
                getchar();
                
            }
            
        }
//        for(auto cobo : simplexTree.cofaces_simplex_range(f_simplex,1)){
//            std::cout << "cobo: "
//            << "[" << simplexTree.filtration(cobo) << "] ";
//            for (auto vertex : simplexTree.simplex_vertex_range(cobo)) {
//                std::cout << static_cast<int>(vertex) << " ";
//            }
//        }
        cout<<endl;
        }
    }
    
    // COMMENT THIS PART LATER
    for(int i=0;i<n;i++){
        cout<<graphNode[i]<<": ";
        for(int j=0;j<n;j++){
            cout<<adjMatrix[i][j]<<" ";
        }
        cout<<endl;
    }
    
    // Our graph is now represented as an adjacency list.
    adjMatrix = adjMatrixbuff;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void upDateMatrix(Simplex_tree simplexTree, int birth, int death ) //birth is when traingle came, death when tetra
{
    //Change red nodes to green nodes upto death: which are tetrahedrons
    //death from threesimplex_to_node
    fill(begin(graphNode), begin(graphNode) + threesimplex_to_node[death], 1); // upto t is green
    // For each triangle, edge in dual graph is green if: birth(triangle) <= birth
    // For each triangle, edge in dual graph is purple if: birth< birth(triangle) < death
    // For each tetrahedron, uptill death, no connection with final sink node n-1
    for (auto f_simplex : simplexTree.filtration_simplex_range()){
        if(simplexTree.filtration(f_simplex)>death)
            break;
        if(dimension(f_simplex)==2){//Triangle:Push purple or green edges
            auto cof = simplexTree.cofaces_simplex_range(face, 1));// Tetra corresponding to triangle
            if(cof.size()==2){//triangle bounded on both sides by tetra
                if(simplexTree.filtration(f_simplex)>birth){
                    int graphInd1 = threesimplex_to_node[simplexTree.filtration(cof[0])];
                    int graphInd2 = threesimplex_to_node[simplexTree.filtration(cof[1])];
                    adjMatrix[graphInd1][graphInd2]= INT_MAX;//purple
                }
                else{
                    int graphInd1 = threesimplex_to_node[simplexTree.filtration(cof[0])];
                    int graphInd2 = threesimplex_to_node[simplexTree.filtration(cof[1])];
                    adjMatrix[graphInd1][graphInd2]= 1; //green
                }
            }
            else if(cof.size()==1){//turn edge to inf green
                int graphInd1 = threesimplex_to_node[simplexTree.filtration(cof[0])];
                adjMatrix[graphInd1][n-2]= 1; //green
            }
                
        }
        else if(dimension(f_simplex)==3){//delete edge from tetra to final sink
            int graphInd1 = threesimplex_to_node[simplexTree.filtration(cof[0])];
            adjMatrix[graphInd1][n-1]= -1;
        }
    }
        
}

//////////////////////////////////////////////////////////////////////////////////////////////
// A special BFS version that returns true if there's a path from source to sink.
bool BFS(vector< vector<int> > &resAdjMatrix, int &source, int &sink, vector<int> &parent)
{
    // Create an array for all nodes we visited. Initialized to false.
    int n = resAdjMatrix.size();
    bool *visited = new bool[n];
    fill(visited, visited + n, false);
//    bool visited[n] = { false };
    
    // Create a queue to check each node.
    queue<int> q;
    
    // Push our source into the queue and mark it as visited. It has no parent.
    q.push(source);
    visited[source] = true;
    parent[source] = -1;
    
    // Keep visiting vertices.
    while(q.empty() == false)
    {
        int u = q.front();
        q.pop();
        
        // Check all of u's friends.
        for(int i = 0; i < n; i++)
        {
            int v = i;
            int capacity = resAdjMatrix[u][v];
            
            // We find a neighbor that hasn't been visited, and the capacity is bigger than 0.
            if(visited[v] == false && capacity > 0)
            {
                // Push the neighbor onto the queue, mark it's parent, and mark it as visited.
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    
    // If the sink got visited, then we found a path to it.
    if(visited[sink] == true)
    { return true; }
    
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
// Use the Ford Fulkerson algorithm. Return the max flow.
int FordFulkerson(vector< vector<int> > &adjMatrix, int &source, int &sink)
{
    int maxflow = 0;
    
    // 1. Create the residual graph. (Same as the original graph.)
    vector< vector<int> > resAdjMatrix;
    int n = adjMatrix.size();
    for(int i = 0; i < n; i++)
    {
        vector<int> row;
        resAdjMatrix.push_back(row);
        for(int j = 0; j < adjMatrix[i].size(); j++)
        {
            resAdjMatrix[i].push_back(adjMatrix[i][j]);
        }
    }
    
    // 2. Create an empty parent array for BFS to store the augmenting path.
    vector<int> parent;
    for(int i = 0; i < n; i++)
    {
        parent.push_back(-1);
    }
    
    // 3. Keep calling BFS to check for an augmenting path (from the source to the sink...
    while(BFS(resAdjMatrix, source, sink, parent) == true)
    {
        // 4. Find the max flow through the path we just found.
        int pathflow = INT_MAX;
        
        // Go through the path we just found. Iterate through the path.
        int v = sink;
        while(v != source)
        {
            int u = parent[v]; // The parent.
            
            // Update the pathflow to this capacity if it's smaller
            int capacity = resAdjMatrix[u][v];
            pathflow = min(pathflow, capacity);
            
            // Setup for the next edge in the path.
            v = u;
        }
        
        // 5. Update the residual capacities of the edges and reverse edges.
        v = sink;
        while(v != source)
        {
            int u = parent[v]; // The parent.
            
            // Update the capacities.
            
            resAdjMatrix[u][v] -= pathflow;
            resAdjMatrix[v][u] += pathflow;
            
            // Setup for the next edge in the path.
            v = u;
        }
        
        // 6. Add this path's flow to our total max flow so far.
        maxflow += pathflow;
    }
    
    return maxflow;
    }


vector<vector<int>> buildFullSimpCom(string filtration_file){

    int dimensions, noPoints;
    vector<vector<int>> G;
    Simplex_tree simplexTree;
    
    ifstream openfilt(filtration_file.c_str());
    if(!openfilt.good())
    {
        cout<<"Point+ Filtration file "<<filtration_file<<" does not exist.";
        exit(0);
    }
    
    openfilt >> dimensions ;      openfilt >> noPoints;
    cout<<"Dimming: "<<dimensions<<" #Pt: "<<noPoints<<" \n";
    
    if(dimensions!=3){
        cout<<"Dimension should be 3";
        exit(0);
    }
    int f_step = 0, threesimplex = 0, twosimplex = 0;
    vector<int> simpcount = {noPoints,0,0,0};
    
    for ( int itp=0; itp < noPoints+1; itp++ )
    {
        char sLine[256]="";
        openfilt.getline( sLine,256);
        typeVectorVertex vertex = {itp};
        const Filtration_value FILTRATION_VALUE = f_step++;
        simplexTree.insert_simplex(vertex, Filtration_value(FILTRATION_VALUE));
//        cout<<sLine<<"itp:"<<itp<<endl;
        
    }
    
    
    while (!openfilt.eof())
    {
        int indf;
        string infgetter;
        char sLine[256]="";
        
        
        
        openfilt.getline(sLine, 256);
//         cout<<sLine<<" sLine"<<endl;// getchar();
        if(strlen(sLine)==0||sLine[0]=='c')
        {//cout<<"continuing";
            continue;
        }
        
        
        
        if(sLine[0]=='#'){
          
            stringstream ss;
            ss.str(sLine);
            ss >> infgetter; ss >> indf;    //gets the # first
            
            if(infgetter!="inf")
                indf = atoi(infgetter.c_str());
            else
            {std::cout<<"Time should not be inf"; exit(0);}
            // cout<<sLine<<" indf: "<<indf<<" ";
            // getchar();
        }// end of #
        
            openfilt.getline(sLine, 256);
            if(openfilt.eof())
                break;
            
            stringstream ss;
            ss.str(sLine);
            char ch;
            int index;
            vector<int> simplex;
            ss >> ch;
            if(ch!='i'){
                cout<<"Not an insertion: "<<sLine<<" It is:"<<ch<<endl;
                exit(0);
            }
            
//            totinsCount++;
            while (ss >> index)
                simplex.push_back(index);
            
            sort(simplex.begin(),simplex.end());
            const Filtration_value FILTRATION_VALUE = f_step;
            typeVectorVertex vertex;// = {itp};
            if(simplex.size()==2){
                vertex = {simplex[0],simplex[1]};
                simpcount[1]++;
            }
        
            
            else if(simplex.size()==3){
                simpcount[2]++;
                vertex = {simplex[0],simplex[1],simplex[2]};
                twosimplex_to_tri[f_step] = twosimplex;
                twosimplex++;
            }
            
            else if(simplex.size()==4){
                vertex = {simplex[0],simplex[1],simplex[2],simplex[3]};
                simpcount[3]++;
                threesimplex_to_node[f_step] = threesimplex;
                threesimplex++;
            }
        
            else{
                cout<<"Unknown Simplex: "<<sLine;
                exit(0);
            }
            
            typePairSimplexBool returnValue =
            simplexTree.insert_simplex(vertex, Filtration_value(FILTRATION_VALUE));
        
            f_step += 1;
        
    }//end of reading file
    G = FormAdjMatrix(simplexTree,simpcount);
    return G;

}
