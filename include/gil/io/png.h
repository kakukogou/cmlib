#ifndef GIL_PNG_H
#define GIL_PNG_H

#include <cstdio>
#include <stdexcept>
#include <vector>

#include "../Exception.h"
#include "../Color.h"

namespace gil {
    class DLLAPI PngReader {
	public:
	    template <typename I>
	    void operator ()(I& image, FILE* f)
	    {
		check(f);
		init(f);
		if (my_channels == 1)
		    read<I, Color<Byte1, 1>::ColorType>(image);
		else if (my_channels == 3)
		    read<I, Color<Byte1, 3>::ColorType>(image);
		else if (my_channels == 4)
		    read<I, Color<Byte1, 4>::ColorType>(image);
		else
		    throw InvalidFormat("channels");
		finish();
	    }
	protected:
	    void check(FILE* f);
	    void init(FILE* f);
	    void finish();
	    void read(unsigned char** row_pointers);
	    template<typename I, typename ColorType>
	    void read(I& image)
	    {
		std::vector<ColorType*> row_pointers(my_height);
		std::vector<ColorType> row_data(my_height*my_width);
		row_pointers[0] = &row_data[0];
		for (size_t i = 1; i < my_height; ++i)
		    row_pointers[i] = row_pointers[i-1] + my_width;

		read((unsigned char**)&row_pointers[0]);
		image.allocate(my_width, my_height);
		for (size_t h = 0; h < my_height; ++h)
		    for (size_t w = 0; w < my_width; ++w)
			I::Converter::ext2int(image(w, h), row_pointers[h][w]);
	    }
	private:
	    const static size_t MAGIC_NUMBER = 8;
	    void *my_png_ptr;
	    void *my_info_ptr;
	    size_t my_width;
	    size_t my_height;
	    size_t my_channels;
    };

    class DLLAPI PngWriter {
	public:
	    template <typename I>
	    void operator ()(const I& image, FILE* f)
	    {
		init(f);
		if (image.channels() >= 4)
		    write<I, Color<Byte1, 4>::ColorType>(image);
		else if (image.channels() == 3)
		    write<I, Color<Byte1, 3>::ColorType>(image);
		else
		    write<I, Color<Byte1, 1>::ColorType>(image);
		finish();
	    }
	protected:
	    void init(FILE* f);
	    void write(unsigned char** row_pointers);
	    void finish();
	    template<typename I, typename ColorType>
	    void write(const I& image)
	    {
		my_width = image.width();
		my_height = image.height();
		my_channels = ColorTrait<ColorType>::channels();

		std::vector<ColorType*> row_pointers(my_height);
		std::vector<ColorType> row_data(my_height*my_width);
		row_pointers[0] = &row_data[0];
		for (size_t i = 1; i < my_height; ++i)
		    row_pointers[i] = row_pointers[i-1] + my_width;

		for (size_t h = 0; h < my_height; ++h)
		    for (size_t w = 0; w < my_width; ++w)
			I::Converter::int2ext(row_pointers[h][w], image(w, h));

		write((unsigned char**)&row_pointers[0]);
	    }
	private:
	    const static size_t BIT_DEPTH = 8;
	    void *my_png_ptr;
	    void *my_info_ptr;
	    size_t my_width;
	    size_t my_height;
	    size_t my_channels;
	    size_t my_bit_depth;
    };
} // namespace gil

#endif // GIL_PNG_H
