#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "graph.h"

using std::cout; using std::endl;

// very raw parsing of graph file
bool parse_graph(std::ifstream &is,int &number, int &k,
        std::map<int, std::vector<int> > &edges, int &max_nodes) {

    max_nodes = -1;
    edges.clear();

    std::string line, tmp;
    char c;

    // parse "Graph [x]:"
    getline(is, line);
    std::istringstream iss(line);
    iss >> tmp >> number;
    if ( !iss || tmp != "Graph" ) return false;

    // parse K
    getline(is, line);
    std::istringstream iss2(line);
    iss2 >> c >> c  >> k;

    int len = is.tellg(), start;

    // there are a clique of the first 21 register
    for ( int i = 0; i < 22; i++)
        for ( int j = i+1; j < 22; j++)
            edges[i].push_back(j);

    // Parse edges
    while ( getline(is, line) ) {

        std::istringstream iss(line);
        iss >> start;

        if ( !iss ) {
            // if the are no node of the type x<->y rollback of one line
            is.seekg(len ,std::ios_base::beg);
            break;
        }

        max_nodes = std::max(start,max_nodes);

        iss >> tmp;
        if ( tmp != "-->") break; // found the x<->y

        while ( true ) {
            int node;
            iss >> node;
            if (!iss) break; // no more edges
            edges[start].push_back(node);
            max_nodes = std::max(node,max_nodes);
        }

        len = is.tellg(); 
    }

    return true;
}

int main( /*int argc, char* argv[] */ ) {

    std::ifstream is;
    is.open("../graphs/Appel_George/graphs.txt");

    int graph_number, k, total_nodes;
    std::map<int, std::vector<int> > edges;
    int total_graph = 0;
    int correcty_approx = 0;
    int bigger_graph = 0;

    int counter = 0;
    while ( parse_graph(is, graph_number, k, edges, total_nodes) && counter++ < 100000 ) {

        //if (graph_number != 311) continue;
        if ( total_nodes < 22 ) total_nodes = 21;

        Graph g(total_nodes + 1);

        for ( const auto &key_val : edges ) {
            for ( auto n : key_val.second ) {
                g.add_edge( key_val.first, n );
                g.add_edge( n, key_val.first );
            }
        }

        std::vector<Graph::color_t> colors(total_nodes+1);
        int chromatic_number_approx = 10000000;
        //g.get_approximate_clique();
        //int chromatic_number_approx = g.get_approximate_coloring( colors );
        //if ( ! g.check_coloration(colors) )
        //    cout << "invalid approx coloration for graph " << graph_number << endl;
        int chromatic_number_sequen = g.sequential_coloring(colors);
        if ( ! g.check_coloration(colors) )
            cout << "invalid sequantial coloration for graph " << graph_number << endl;
        //int chromatic_number_sequen = chromatic_number_approx;

        if ( chromatic_number_approx < chromatic_number_sequen )
            cout << "ERROR, approx better than sequential" << endl;

        //if ( chromatic_number_approx > chromatic_number_sequen )
//            cout << "BAD APPROX " << chromatic_number_approx << " vs " <<
//                chromatic_number_sequen << endl;
        if ( chromatic_number_approx == chromatic_number_sequen )
            ++correcty_approx;
            //cout << "PERFECT APPROX on graph " << graph_number << endl;

        total_graph++;
        bigger_graph = std::max(total_nodes, bigger_graph);
    }

    cout << "parsed " << total_graph << " graphs\nBigger graph " << bigger_graph << endl;
    cout << "correctly approximated " << correcty_approx << "/" << total_graph << endl;

    return 0;
}

