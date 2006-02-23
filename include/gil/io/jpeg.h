#ifndef GIL_JPEG_H
#define GIL_JPEG_H

#include <stdexcept>
#include <cstdio>

#include "../Color.h"

namespace gil {

    class JpegReader {
	public:
	    JpegReader() : my_cinfo(NULL), my_jerr(NULL)
	    {
		// empty
	    }

	    ~JpegReader() throw ()
	    {
		cleanup();
	    }
	    
	    template <typename I>
	    void operator ()(I& image, FILE* f);

	private:
	    // use type T as scanline buffer
	    template <typename T, typename I>
	    void read_pixels(I& image);
	    
	    void init(FILE* f, size_t& w, size_t& h, size_t& c);
	    void finish();
	    void read_scanline(std::vector<Byte1>& buf);
	    void read_scanline(std::vector<Byte3>& buf);
	    void cleanup() throw();
	    
	    void* my_cinfo;
	    void* my_jerr;
    };

    template <typename T, typename I>
    void JpegReader::read_pixels(I& image)
    {
	typedef typename I::Converter Conv;
	size_t w = image.width(), h = image.height();
	std::vector<T> buffer(w);
	for(size_t y = 0; y < h; y++){
	    read_scanline(buffer);
	    for(size_t x = 0; x < w; x++)
		Conv::ext2int( image(x,y), buffer[x] );
	}
    }

    template <typename I>
    void JpegReader::operator ()(I& image, FILE* f)
    {
	size_t width, height, channel;
	init(f, width, height, channel);
	image.allocate(width, height);
	
	// template banzai!
	if(channel == 1) {
	    read_pixels<Byte1>(image);
	} else if(channel == 3) {
	    read_pixels<Byte3>(image);
	} else {
	    finish();
	    throw std::runtime_error("unsupported jpeg channel number");
	}
	finish();
    }

    class JpegWriter {
	public:
	    JpegWriter() : my_cinfo(NULL), my_jerr(NULL)
	    {
		// empty
	    }

	    ~JpegWriter() throw ()
	    {
		cleanup();
	    }
	    
	    template <typename I>
	    void operator ()(const I& image, FILE* f);

	private:
	    // use type T as scanline buffer
	    template <typename T, typename I>
	    void write_pixels(I& image);
	    
	    void init(FILE* f, size_t w, size_t h, size_t c);
	    void write_scanline(std::vector<Byte1>& buf);
	    void write_scanline(std::vector<Byte3>& buf);
	    void finish();
	    void cleanup() throw ();
	    
	    void* my_cinfo;
	    void* my_jerr;
    };

    template <typename T, typename I>
    void JpegWriter::write_pixels(I& image)
    {
	typedef typename I::Converter Conv;
	size_t w = image.width(), h = image.height();
	std::vector<T> buffer(w);
	for(size_t y = 0; y < h; y++){
	    for(size_t x = 0; x < w; x++)
		Conv::int2ext( buffer[x], image(x,y) );
	    
	    write_scanline(buffer);
	}
    }

    template <typename I>
    void JpegWriter::operator ()(const I& image, FILE* f)
    {
	size_t c = (image.channels()==1) ? 1 : 3;
	init( f, image.width(), image.height(), c);
	if(c == 1)
	    write_pixels<Byte1>(image);
	else
	    write_pixels<Byte3>(image);
	finish();
    }

} // namespace gil

#endif // GIL_JPEG_H