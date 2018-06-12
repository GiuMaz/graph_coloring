#ifndef EDGE_HEAP_HH
#define EDGE_HEAP_HH

#include <assert.h>
#include <vector>
#include "shrinking_set.h"

// binary Heap
class EdgeHeap {
    using size_type = size_t;
public:
    explicit EdgeHeap(const std::vector<unsigned int> &d, const std::vector<ShrinkingSet> &a):
        degrees(d), avail(a), value(), map_position(d.size(),-1)  {}

    void insert(size_t val) {
        if ( map_position[val] != -1 ) return; // already inserted

        value.push_back(val);
        map_position[val] = value.size()-1;
        increase_key(value.size()-1);
    }

    size_t pop_max() {
        assert(!value.empty());

        auto max = value.front();
        value.front() = value.back();
        map_position[max] = -1;
        map_position[value.front()] = 0;
        value.pop_back();
        heapify(0);
        return max;
    }

    void initialize() {
        value.clear();
        value.reserve(map_position.size());

        for ( size_t i = 0; i < map_position.size(); ++i ) {
            value.push_back(i);
            map_position[i] = value.size()-1;
        }

        for ( int i = value.size()/2; i >= 0; --i )
            heapify(i);
    }

    bool empty() {
        return value.empty();
    }

    void update_increased(size_t val) {
        if ( map_position[val] == -1 ) return;
        increase_key(map_position[val]);
    }

    void update_decreased(size_t val) {
        if ( map_position[val] == -1 ) return;
        heapify(map_position[val]);
    }

private:
    const std::vector<unsigned int> &degrees;
    const std::vector<ShrinkingSet> &avail;

    std::vector<size_t> value;
    std::vector<int> map_position;

    // tree navigation (0 based, it's different from CLR's 1 based)
    size_type father(size_type i) { return (i-1) >> 1; }
    size_type left(size_type i)   { return (i<<1) + 1; }
    size_type right(size_type i)  { return (i+1) << 1; }

    void heapify(size_type i) {
        //std::cout <<"heapify " << i << " " << left(i) << " " << right(i) << std::endl;
        auto largest = i;
        if (left(i)  < value.size() && is_greater(left(i),largest))
            largest = left(i);
        if (right(i) < value.size() && is_greater(right(i),largest)) 
            largest = right(i);

        if ( largest != i ) {
            std::swap(value[i],value[largest]);
            std::swap(map_position[value[i]], map_position[value[largest]]);
            heapify(largest);
        }
    }

    bool is_greater(size_t lpos, size_t rpos) {
        return avail[value[lpos]].size() < avail[value[rpos]].size() ||
            (avail[value[lpos]].size() == avail[value[rpos]].size() &&
             degrees[value[lpos]] > degrees[value[rpos]]);
    }

    void increase_key( size_type pos ) {
        while ( pos > 0 && is_greater(pos,father(pos)) ) {
            std::swap(value[pos],value[father(pos)]);
            std::swap(map_position[value[pos]],
                    map_position[value[father(pos)]]);
            pos = father(pos);
        }
    }

};

#endif
