#ifndef SHRINKING_SET_HH
#define SHRINKING_SET_HH

#include <set>
#include <iostream>

class ShrinkingSet {
public:
    using size_type = unsigned int;
    ShrinkingSet(size_type l, size_type h): data{{l,h}}, counter(h-l+1) {}

    int select_min() const { return data.rbegin()->low; }
    size_type size() const { return counter; }

    void erase(size_type x);
    void print_status(const std::string &extra_message) const;
    void print_status() const { print_status(""); }
    void add_value(size_type x);
    void clear() { data.clear(); counter = 0; }
    void reduce_to(size_type x) {
        auto last_interval = *data.begin();

        if ( last_interval.high <= x )
            return;

        counter-= (last_interval.high-last_interval.low+1);
        data.erase(data.begin());

        if ( last_interval.low <= x ) {
            last_interval.high = x;
            data.insert(last_interval);
            counter+= (last_interval.high-last_interval.low+1);
        }
    }

private:
    struct Interval { // DISJOINT interval [low-high]
        size_type low;
        size_type high;
        // NB:  descending order, for compatibility with c++ lower_bound
        // NB2: it assume that all the intervall are disjoined, so only the
        //      lower bound must be tested
        bool operator< (const Interval &rhs) const { return low >  rhs.low; }
    };

    std::set<Interval> data;
    size_type counter;
};

inline void ShrinkingSet::erase(size_type x) { 
    auto it = data.lower_bound( {x,0} );
    if (x > it->high) return;
    auto val = *it;
    --counter;
    data.erase(it);
    if ( val.low  < x ) data.insert({val.low, x-1 });
    if ( val.high > x ) data.insert({x+1, val.high});
}

inline void ShrinkingSet::print_status(const std::string &extra_message) const {
    if ( extra_message != "" )
        std::cout << extra_message << std::endl;

    std::cout << "\tsize: " << counter << std::endl << "\tdata: ";
    for ( auto it = data.rbegin(); it != data.rend(); ++it) {
        std::cout << it->low << "-" << it->high << ", ";
    }
    std::cout << std::endl << std::endl;
}

inline void ShrinkingSet::add_value(size_type x) {
    size_type begin = x, end = x;

    auto it = data.lower_bound({x,0});

    if ( it != data.end() ) {

        if ( x <= it->high ) return; // already inserted

        if ( it->high == (x-1)) {
            begin = it->low;
            data.erase(it);
        }

        if ( (--it)->low == (x+1)) {
            end = it->high;
            data.erase(it);
        }
    }

    ++counter;
    data.insert({begin,end});
}

#endif
