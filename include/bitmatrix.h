#ifndef BITMATRIX_HH
#define BITMATRIX_HH

#include <cstring>

class BitMatrix {
public:
    BitMatrix(size_t h, size_t w):
        height(h), width(w), matrix(new uint8_t[(((h*w)/8)+((h*w)%8==0?0:1))]()) {
        }

    BitMatrix( const BitMatrix &bm ) :
        height(bm.height), width(bm.width), matrix() {
            size_t size = ((height*width)/8)+((height*width)%8==0?0:1);
            matrix = new uint8_t[size];
            memcpy( matrix, bm.matrix, size);
        }

    ~BitMatrix() { delete[] matrix; }

    void set( size_t y, size_t x, bool on ) {
        size_t pos = y*width+x;
        uint8_t mask = 1<<(7-(pos%8)) ;
        if ( on ) matrix[pos/8] |= mask ;
        else      matrix[pos/8] &= (0xFF^mask);
    }

    bool get( size_t y, size_t x ) const {
        size_t pos = y*width+x;
        return (matrix[pos/8]&(1<<(7-pos%8))) > 0;
    }

    void print() const {
        for( size_t y = 0; y < height; ++y ) {
            for( size_t x = 0; x < width; ++x )
                std::cout << get(y,x) << " ";
            std::cout << std::endl;
        }
    }

    BitMatrix & operator=(const BitMatrix &m) {
        if ( &m == this ) return *this;

        height = m.height;
        width  = m.width;

        size_t size = ((height*width)/8)+((height*width)%8==0?0:1);
        delete[] matrix;
        matrix = new uint8_t[size];
        memcpy( matrix, m.matrix, size);

        return *this;
    }

private:
    size_t height, width;
    uint8_t *matrix;
};

#endif
