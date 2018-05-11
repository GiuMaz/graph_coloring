#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>

#include "graph.h"

using std::cout; using std::endl;

// required for parse of compressed graph
uint8_t masks[ 8 ] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

bool parse_compressed_dimacs(std::ifstream &is,
        std::map<int, std::vector<int> > &edges, int &total_nodes) {

    // read the preamble, no fixed limit
    int preamble_lenght;
    is >> preamble_lenght;
    if ( !is ) {
        cout << "FAILED TO READ PREAMBLE" << endl;
        return false;
    }

    total_nodes = -1;
    edges.clear();
    std::string line;
    int total_edges;

    while ( std::getline( is, line ) ) {
        if (line.size() == 0 || line[0] == 'c') continue; // skip comment

        std::string tmp1, tmp2;
        std::istringstream iss(line);

        iss >> tmp1 >> tmp2 >> total_nodes >> total_edges; 

        if ( tmp1 != "p" || tmp2 != "edge" ) {
            cout << "invalid header " << tmp1 << " " << tmp2 << endl;
            return false;
        }

        cout << "nodes: " << total_nodes << " edges: " << total_edges << endl;
        break;
    }

    uint8_t byte;
    std::vector<std::vector<uint8_t> > input_bytes(total_nodes,
            std::vector<uint8_t>(total_nodes,0));

    for ( int i = 0; i < total_nodes; ++i ) {
        for ( int j = 0 ; j < ((i/8)+1); ++j ) {
            is >> std::noskipws >> byte;
            if ( ! is ) {
                goto end;
            }
            input_bytes[i][j] =  byte;
        }
    }
end:

    int edge_counter = 0;
    int non_edge_counter = 0;
    for (int i = 0; i < total_nodes; i++) {
        for (int j = 0; j < total_nodes; j++) {
            int from = std::max(j,i);
            int to   = std::min(j,i);
            uint8_t mask = masks[7-(to & 0x07)];
            if ( (input_bytes[from][to/8] & mask) == mask ) {
                ++edge_counter;
                edges[j].push_back(i);
            }
            else
                non_edge_counter++;
        }
    }
    return true;
}

bool parse_graph_dimacs(std::ifstream &is,
        std::map<int, std::vector<int> > &edges, int &total_nodes) {

    total_nodes = -1;
    edges.clear();
    std::string line;
    int total_edges;

    while ( std::getline( is, line ) ) {
        if (line[0] == 'c') continue; // skip comment

        std::string tmp1, tmp2;
        std::istringstream iss(line);

        iss >> tmp1 >> tmp2 >> total_nodes >> total_edges; 

        if ( tmp1 != "p" || tmp2 != "edge" )
            return false;

        cout << "nodes: " << total_nodes << " edges: " << total_edges << endl;
        break;
    }

    int edges_counter = 0;
    while ( std::getline( is, line ) && edges_counter++ < total_edges ) {
        if (line[0] == 'c') continue; // skip comment
        std::string tmp1;
        int from, to;

        std::istringstream iss(line);
        iss >> tmp1 >> from >> to; 

        if ( tmp1 != "e" )
            return false;

        edges[from-1].push_back(to-1); // normalize to 0
    }

    return true;
}

int main(int argc, char* argv[]) {

    if ( argc < 2 ) {
        cout << "file name required" << endl;
        return 1;
    }

    std::ifstream is;
    is.open(argv[1]);

    if ( ! is ) {
        cout << "impossible to open file " << argv[1] << endl;
        return 1;
    }

    int total_nodes;
    std::map<int, std::vector<int> > edges;

    if ( ! parse_compressed_dimacs(is, edges, total_nodes) ) {
        cout << "failed parsing of " << argv[1] << endl;
        return 1;
    }

    Graph g(total_nodes);

    for ( const auto &key_val : edges ) {
        for ( auto n : key_val.second ) {
            g.add_edge( key_val.first, n );
            g.add_edge( n, key_val.first );
        }
    }

    std::vector<Graph::color_t> colors(total_nodes);

    //int chromatic_number_approx = g.get_approximate_coloring( colors );

    //if ( ! g.check_coloration(colors) )
    //    cout << "invalid approx coloration for graph " << argv[1] << endl;

    int chromatic_number_sequen = g.sequential_coloring(colors);
    if ( ! g.check_coloration(colors) )
        cout << "invalid sequantial coloration for graph " << argv[1] << endl;

//    cout << "approx clique " << g.get_approximate_clique().size() << " max clique " <<
//        g.get_max_clique().size() << endl;

    cout << "sequential " << chromatic_number_sequen;
    //cout << " approx " << chromatic_number_approx;
    //cout << " approx clique " << g.get_approximate_clique().size();
    cout << endl;

    return 0;
}

