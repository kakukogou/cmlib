#ifndef GIL_TIFF_H
#define GIL_TIFF_H

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>

#include "../Exception.h"
#include "../Color.h"

namespace gil {

    class DLLAPI TiffReader {
	public:
	    TiffReader() : my_tiff(NULL)
	    {
		// empty
	    }

	    template <typename I>
	    bool operator ()(I& image, const std::string& name);

	private:
	    // use type T as scanline buffer
	    template <typename T, typename I>
	    void read_pixels(I& image);
	    
	    void init(const std::string& name, size_t& w, size_t& h, size_t& c);
	    void finish();
	    void read_scanline(std::vector<Byte1>& buf, unsigned int y);
	    void read_scanline(std::vector<Byte3>& buf, unsigned int y);
	    void read_scanline(std::vector<Byte4>& buf, unsigned int y);
	    
	    void* my_tiff;
    };

    template <typename T, typename I>
    void TiffReader::read_pixels(I& image)
    {
	typedef typename I::Converter Conv;
	size_t w = image.width(), h = image.height();
	std::vector<T> buffer(w);
	for(size_t y = 0; y < h; y++){
	    read_scanline(buffer, y);
	    for(size_t x = 0; x < w; x++)
		Conv::ext2int( image(x,y), buffer[x] );
	}
    }

    template <typename I>
    bool TiffReader::operator ()(I& image, const std::string& name)
    {
	size_t width, height, channel;
	init(name, width, height, channel);
	if(my_tiff == NULL)
	    return false; // unable to read file

	image.allocate(width, height);
	
	// template banzai!
	if(channel == 1) {
	    read_pixels<Byte1>(image);
	} else if(channel == 3) {
	    read_pixels<Byte3>(image);
	} else if(channel == 4){
	    read_pixels<Byte4>(image);
	} else {
	    finish();
	    throw InvalidFormat("unsupported tiff channel number");
	}
	finish();
	return true;
    }

    class DLLAPI TiffWriter {
	public:
	    TiffWriter() : my_tiff(NULL)
	    {
		// empty
	    }
	    
	    template <typename I>
	    bool operator ()(const I& image, const std::string& name);

	private:
	    // use type T as scanline buffer
	    template <typename T, typename I>
	    void write_pixels(I& image);
	    
	    void init(const std::string& name, size_t w, size_t h, size_t c);
	    void write_scanline(std::vector<Byte1>& buf, unsigned int y);
	    void write_scanline(std::vector<Byte3>& buf, unsigned int y);
	    void write_scanline(std::vector<Byte4>& buf, unsigned int y);
	    void finish();
	    
	    void* my_tiff;
    };

    template <typename T, typename I>
    void TiffWriter::write_pixels(I& image)
    {
	typedef typename I::Converter Conv;
	size_t w = image.width(), h = image.height();
	std::vector<T> buffer(w);
	for(size_t y = 0; y < h; y++){
	    for(size_t x = 0; x < w; x++)
		Conv::int2ext( buffer[x], image(x,y) );
	    
	    write_scanline(buffer, y);
	}
    }

    template <typename I>
    bool TiffWriter::operator ()(const I& image, const std::string& name)
    {
	size_t c = image.channels();
	
	if(c >= 4)
	    c = 4;
	else if(c >= 3)
	    c = 3;
	else 
	    c = 1;
	
	init( name, image.width(), image.height(), c);
	if(my_tiff == NULL)
	    return false;
	if(c == 1)
	    write_pixels<Byte1>(image);
	else if(c == 3)
	    write_pixels<Byte3>(image);
	else
	    write_pixels<Byte4>(image);

	finish();
	return true;
    }

} // namespace gil

#endif // GIL_TIFF_H
