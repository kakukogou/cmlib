#ifndef GIL_HDR_H
#define GIL_HDR_H

#include <stdexcept>
#include <cstdio>
#include <vector>

#include "../Color.h"

namespace gil {

    class HdrReader {
	public:
	    HdrReader() : my_file(NULL)
	    {
		// empty
	    }

	    template <typename I>
	    void operator ()(I& image, FILE* f);

	private:
	    void init(FILE* f, size_t& w, size_t& h);
	    void finish();
	    void read_scanline(std::vector<Float3>& buf);

	    void read_encoded();
	    void read_encoded_old(size_t pos = 0);
	    
	    std::vector<Byte4> my_enc_buffer;
	    FILE* my_file;
    };

    template <typename I>
    void HdrReader::operator ()(I& image, FILE* f)
    {
	size_t width, height;
	init(f, width, height);
	image.allocate(width, height);
	
	typedef typename I::Converter Conv;
	std::vector<Float3> buffer(width);
	for(size_t y = 0; y < height; y++){
	    read_scanline(buffer);
	    for(size_t x = 0; x < width; x++)
		Conv::ext2int( image(x,y), buffer[x] );
	}
	
	finish();
    }
    
    class HdrWriter {
	public:
	    HdrWriter() : my_file(NULL)
	    {
		// empty
	    }

	    template <typename I>
	    void operator ()(const I& image, FILE* f);

	private:
	    void init(FILE* f, size_t w, size_t h);
	    void finish();
	    void write_scanline(const std::vector<Float3>& buf);

	    void write_encoded();
	    
	    std::vector<Byte4> my_enc_buffer;
	    FILE* my_file;
    };

    template <typename I>
    void HdrWriter::operator ()(const I& image, FILE* f)
    {
	size_t width = image.width(), height = image.height();
	init(f, width, height);
	
	typedef typename I::Converter Conv;
	std::vector<Float3> buffer(width);
	for(size_t y = 0; y < height; y++){
	    for(size_t x = 0; x < width; x++)
		Conv::int2ext( buffer[x], image(x,y) );
	    write_scanline(buffer);
	}
	
	finish();
    }

} // namespace gil

#endif // GIL_HDR_H
