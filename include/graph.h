#ifndef GRAPH_DEF_HH
#define GRAPH_DEF_HH

#include <vector>
#include "bitmatrix.h"
    
class Graph {
public:
    using color_t = int;
    using node_t  = unsigned int;
    using Clique  = std::vector<node_t>;

    explicit Graph(node_t n): adjacency_matrix(n,n), nodes(n), degrees(n,0) {}
    ~Graph() {}

    void add_edge( node_t f, node_t t) {
        if (adjacency_matrix.get(f,t)) return;
        ++degrees[f];
        adjacency_matrix.set(f,t,true);
    }

    Clique get_approximate_clique();

    void print() const;
    bool check_coloration(std::vector<color_t> &colors );

    /**
     * Brelaz approximate coloring as seen in 
     * Jonathan S.Turner,"Almost All k-Colorable Graphs Are Easy to Color"
     * It's use a DSATUR heuristic for the node ordering
     */
    color_t get_approximate_coloring(std::vector<color_t> &colors);

    /**
     * Exact coloration of the graph.
     * It use a clique (not necessarily optimal) as a starting point
     */
    color_t sequential_coloring(std::vector<color_t> &colors);

private:

    Clique get_approximate_clique(const std::vector<node_t> &sorted);
    void get_sorted_node(std::vector<node_t> &sorted);

    BitMatrix adjacency_matrix;
    node_t nodes;
    std::vector<node_t> degrees;

    color_t sequential_coloring_rec( std::vector<color_t> &colors,
            const std::vector<node_t> &sorted, color_t k,
            color_t best, color_t lower_bound );
};

    /*
    struct SearchData {

        SearchData( size_t n ):
            active(n,true), active_counter(n), nodes_stack() {
                nodes_stack.reserve(n);
            }

        std::vector<bool> active;
        unsigned int active_counter;
        std::vector<int> nodes_stack;
    };

    using Clique = std::vector<int>;

    //pseudocode maximum-clique
    Clique maximum_clique() {
        SearchData sd(nodes);

        Clique c, best;
        return maximum_clique_rec(sd, c, best, nodes+1);
    }

    Clique maximum_clique_rec( SearchData &sd, Clique &c, Clique best, size_t upper_bound ) {
        // if Empty exit
        if ( sd.active_counter == 0 ) return c;

        int k = 0;
        // calcolate approximate coloring
        // TODO

        // get upper bound
        upper_bound = std::min( upper_bound, c.size() + k );

        // if upperbound is lower than best, finish
        if ( upper_bound <= best.size() ) return best;

        // get a maximum degrees vertex
        // TODO improve performance (maybe keep and ordere priority queue?)
        size_t v = 0, max_degree = 0;
        for( size_t i = 0; i < nodes; i++ ) {
            size_t degree_count = 0;
            if ( ! sd.active[i] ) continue;
            for( size_t e = 0; e < nodes; e++ )
                if ( sd.active[e] && adjacency_matrix.get(v, i) )
                    ++degree_count;
            if ( degree_count > max_degree ) {
                v = i;
                max_degree = degree_count;
            }
        }

        // get the subgraph induced by the neighbourn of the vertex
        size_t stack_base = sd.nodes_stack.size();

        for( size_t i = 0; i < nodes; i++ ) {
            if ( sd.active[i] && !adjacency_matrix.get(v, i) ) {
                sd.nodes_stack.push_back(i);
                sd.active[i] = false;
                --sd.active_counter;
            }
        }
        c.push_back(v);

        // try to solve the problem with only the neibourn and the vertex in the
        // clique
        best = maximum_clique_rec(sd, c, best, upper_bound);

        // undo the change
        for ( size_t i = sd.nodes_stack.size(); i > stack_base; --i ) {
            sd.active[sd.nodes_stack.back()] = true;
            ++sd.active_counter;
            sd.nodes_stack.pop_back();
        }
        c.pop_back();

        // if the best found is equal to the upper, return
        if ( upper_bound == best.size() ) return best;

        // remove the vertex from the graph
        sd.nodes_stack.push_back(v);
        sd.active[v] = false;
        --sd.active_counter;

        // return a recursive call without the vertex
        return maximum_clique_rec(sd,c, best, upper_bound);
    }
    */
#endif
