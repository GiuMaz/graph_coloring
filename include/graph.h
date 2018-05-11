#ifndef GRAPH_DEF_HH
#define GRAPH_DEF_HH

#include <vector>
#include "bitmatrix.h"
#include "shrinking_set.h"
    
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

    /**
     * insert the node with the highest degrees, and so on for it neighbourn
     */
    Clique get_approximate_clique();

    Clique get_max_clique();

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

    struct SearchData {

        SearchData( size_t n ):
            active(n,true), active_counter(n),
            nodes_stack(), avail(n, ShrinkingSet(1,n)),
            degrees(n,0) {
                nodes_stack.reserve(n);
            }

        std::vector<bool> active;
        unsigned int active_counter;
        std::vector<int> nodes_stack;
        std::vector<ShrinkingSet> avail;
        std::vector<node_t> degrees;

        size_t get_stack_mark() const { return nodes_stack.size(); }

        void undo_stack(size_t mark) { 
            while ( nodes_stack.size() > mark ) {
                active[nodes_stack.back()] = true;
                nodes_stack.pop_back();
                ++active_counter;
            }
        }

        void push_node(Graph::node_t n) {
            if ( active[n] ) {
                nodes_stack.push_back(n);
                active[n] = false;
                --active_counter;
            }
        }
    };

    color_t get_approximate_coloring(SearchData &g);
    Clique get_approximate_clique(const std::vector<node_t> &sorted);
    Clique get_approximate_clique(SearchData &g,
            const std::vector<node_t> &sorted);
    Clique maximum_clique_rec( SearchData &sd, Clique c,
            Clique best, size_t upper_bound );
    
    void get_sorted_node(std::vector<node_t> &sorted);

    BitMatrix adjacency_matrix;
    node_t nodes;
    std::vector<node_t> degrees;

    color_t sequential_coloring_rec( std::vector<color_t> &colors,
            const std::vector<node_t> &sorted, color_t k,
            color_t best, color_t lower_bound );
};

#endif
