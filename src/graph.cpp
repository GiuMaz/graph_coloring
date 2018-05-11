#include "edge_heap.h"
#include "graph.h"
#include "shrinking_set.h"
#include <iostream>

using std::cout; using std::endl; using std::vector;

void
Graph::print() const {
    for ( node_t i = 0; i < nodes; ++i ) {
        cout << "[" << i << "]: ";
        for ( node_t j = 0; j < nodes; ++j )
            if ( adjacency_matrix.get(i,j) )
                cout << j << " ";
        cout << endl;
    }
}

void
Graph::get_sorted_node(vector<Graph::node_t> &sorted) {

    Graph::node_t max_degree = 0;
    for(Graph::node_t v = 0; v < nodes; v++)
        if ( degrees[v] > max_degree )
            max_degree = degrees[v];

    vector<Graph::node_t> tmp(max_degree+1,0);

    for ( Graph::node_t i = 0; i < nodes; i++ )
        tmp[degrees[i]] = tmp[degrees[i]] + 1;
    for ( Graph::node_t i = 1; i <= max_degree; i++ )
        tmp[i] += tmp[i-1];
    for ( int i = nodes-1; i >= 0; --i ) {
        sorted[tmp[degrees[i]]-1] = i;
        tmp[degrees[i]]-=1;
    }
}

Graph::color_t
Graph::sequential_coloring(vector<Graph::color_t> &colors) {

    vector<Graph::node_t> sorted(nodes,0);
    get_sorted_node(sorted);

    for ( Graph::node_t i = 0; i < nodes; i++ ) colors[i] = 0;
    auto clique = get_max_clique();
    Graph::color_t color = 0;
    for ( auto v : clique )
        colors[v] = ++color;

    return sequential_coloring_rec(colors,sorted,color,nodes+1,clique.size());
}

int
Graph::sequential_coloring_rec( vector<Graph::color_t> &colors,
        const vector<unsigned int> &sorted, int k,
        int best, int lower_bound ) {

    // check if everything is colored
    size_t uncolored = 0;
    bool all_colored = true;
    for ( int i = nodes-1; i >= 0; i-- ) {
    //for ( size_t i = 0; i < nodes; i++ ) {
        if ( colors[sorted[i]] == 0 ) {
            uncolored = sorted[i];
            all_colored = false;
            break;
        }
    }
    if ( all_colored ) return k;

    for ( int c = 1; c <= std::min(k+1, best-1); ++c ) {

        bool conflicting_col = false;
        for( size_t i = 0; i < nodes; i++ ) {
            if ( adjacency_matrix.get(uncolored, i) && colors[i] == c ) {
                conflicting_col = true;
                break;
            }
        }
        if ( conflicting_col ) continue;

        // it is possible to use color c to color the nodes
        colors[uncolored] = c;
        best = sequential_coloring_rec(colors,sorted, std::max(c,k), best,
                lower_bound );
        if ( lower_bound == best ) return best;
    }

    return best;
}

Graph::Clique
Graph::get_approximate_clique() {
    vector<Graph::node_t> sorted(nodes,0);
    get_sorted_node(sorted);
    return get_approximate_clique(sorted);
}

Graph::Clique
Graph::get_approximate_clique(const vector<Graph::node_t> &sorted) {
    vector<bool> active(nodes, true);
    Graph::Clique clique;
    for ( int i = nodes-1; i >= 0; i-- ) {
        if ( ! active[sorted[i]] ) continue;
        clique.push_back(sorted[i]);
        for ( size_t j = 0; j < nodes; j++ )
            active[j] = active[j] && adjacency_matrix.get(sorted[i],j);
    }
    return clique;
}

Graph::Clique
Graph::get_approximate_clique(SearchData &g,
        const vector<Graph::node_t> &sorted) {
    vector<bool> active(nodes, true);
    Graph::Clique clique;
    for ( int i = nodes-1; i >= 0; i-- ) {
        if ( ! active[sorted[i]] || !g.active[sorted[i]] ) continue;
        clique.push_back(sorted[i]);
        for ( size_t j = 0; j < nodes; j++ )
            active[j] = active[j] && adjacency_matrix.get(sorted[i],j);
    }
    return clique;
}

bool
Graph::check_coloration(vector<Graph::color_t> &colors) {
    for( size_t n = 0; n < nodes; n++ ) {
        if ( colors[n] <= 0 ) {
            cout << "uncolored nodes " << n << endl;
            return false;
        }
        for( size_t e = 0; e < nodes; e++ ) {
            if ( adjacency_matrix.get(n, e) && colors[n] == colors[e] ) {
                cout << "neighborn with identical color " <<
                    n << "," << e << endl;
                return false;
            }
        }
    }
    return true;
}

Graph::Clique
Graph::get_max_clique(){
    SearchData sd(nodes);
    Clique c, best;
    return maximum_clique_rec(sd, c, best, nodes+1);
}

Graph::Clique
Graph::maximum_clique_rec( SearchData &sd, Clique c, Clique best,
        size_t upper_bound ) {

    // = tmp assert =
    size_t check_counter = 0;
    for ( auto i : sd.active ) if (i) check_counter++;
    assert( check_counter == sd.active_counter );
    // ==============

    //std::cout << sd.active_counter << " " << c.size() << " " << 
    //    (c.empty()? -1 : c.back())
    //    << " " << best.size() << " " << upper_bound << std::endl;

    auto stack_base = sd.get_stack_mark();
    bool removed_some_nodes;
    do {
        removed_some_nodes = false;
        // if Empty exit
        if ( sd.active_counter == 0 ) {
            sd.undo_stack(stack_base);
            return c;
        }

        // calcolate approximate coloring
        auto chromatic_number_approx = get_approximate_coloring( sd );

        // get upper bound
        upper_bound = std::min(upper_bound, c.size() + chromatic_number_approx);

        // if upperbound is lower than best, finish
        if ( upper_bound <= best.size() ) {
            sd.undo_stack(stack_base);
            return best;
        }

        // Coudert heuristic
        for( size_t i = 0; i < nodes; i++ ) {
            if ( ! sd.active[i] ) continue;
            if ( sd.avail[i].size() >
                    (c.size() - best.size() + chromatic_number_approx) ) {
                sd.push_node(i);
                removed_some_nodes = true;
            }
        }

    } while ( removed_some_nodes );

    // get a maximum degrees vertex
    vector<Graph::node_t> sorted(nodes,0);
    get_sorted_node(sorted);
    size_t v = -1;
    for( int i = nodes-1; i >= 0; i-- ) {
        if ( sd.active[i] ) {
            v = i;
            break;
        }
    }

    // get the subgraph induced by the neighbourn of the vertex
    auto   neigbourn_base = sd.get_stack_mark();

    for( size_t i = 0; i < nodes; i++ )
        if ( !adjacency_matrix.get(v, i) )
            sd.push_node(i);

    c.push_back(v);

    // try to solve the problem with only the neibourn and the vertex in the
    // clique
    best = maximum_clique_rec(sd, c, best, upper_bound);

    // undo the change
    sd.undo_stack(neigbourn_base);

    c.pop_back();

    // if the best found is equal to the upper, return
    if ( upper_bound == best.size() ) {
        sd.undo_stack(stack_base);
        return best;
    }

    // remove the vertex from the graph
    auto single_base = sd.get_stack_mark();
    sd.push_node(v);

    // return a recursive call without the vertex
    best = maximum_clique_rec(sd,c, best, upper_bound);

    // undo change
    sd.undo_stack(single_base);

    sd.undo_stack(stack_base);
    return best;
}

Graph::color_t
Graph::get_approximate_coloring(Graph::SearchData &g) {
    vector<Graph::node_t> sorted(nodes,0);
    get_sorted_node(sorted);
    for ( auto & i : g.avail )
        i = ShrinkingSet(1,g.active_counter);

    Graph::color_t k = 0;
    for ( int uncolored = nodes-1; uncolored >= 0; uncolored-- ) {
        Graph::node_t &v = sorted[uncolored];
        if ( ! g.active[v] ) continue;

        auto color = g.avail[v].select_min();
        if ( color > k ) k = color;

        for( size_t i = 0; i < nodes; i++ )
            if ( g.active[i] && adjacency_matrix.get(v, i) )
                g.avail[i].erase(color);
    }

    // keep only meaningfull colors
    for ( auto & i : g.avail ) i.reduce_to(k);
    return k;
}

Graph::color_t
Graph::get_approximate_coloring(vector<Graph::color_t> &colors) {

    for(size_t v = 0; v < nodes; v++) colors[v] = 0;

    vector<ShrinkingSet> avail(nodes,ShrinkingSet(1,1));

    vector<unsigned int> degrees(nodes,0);
    for(size_t v = 0; v < nodes; v++)
        for(size_t e = 0; e < nodes; e++)
            if (adjacency_matrix.get(v,e))
                degrees[v]++;

    EdgeHeap h(degrees,avail);
    for(size_t v = 0; v < nodes; v++) h.insert(v);

    // si può migliorare con clique approx
    size_t total_color = get_approximate_clique().size();
    while ( ! h.empty() ) {

        size_t v = h.pop_max();

        if ( avail[v].size() == 0 ) {
            ++total_color;
            for(size_t v = 0; v < nodes; v++)
                avail[v].add_value(total_color);
        }

        colors[v] = avail[v].select_min();

        for(size_t e = 0; e < nodes; e++) {
            if ( colors[e] == 0 ) {
                avail[e].erase(colors[v]);
                degrees[e]--;
                h.update_decreased(e);
            }
        }
    }
    return total_color;
}
