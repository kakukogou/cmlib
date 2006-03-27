#ifndef GIL_IMAGE_H
#define GIL_IMAGE_H

#include <algorithm>
#include <string>
#include <cstdio>

#include "Color.h"
#include "Converter.h"

namespace gil {

    template<typename, template<typename,typename> class Conv=DefaultConverter> 
    class Image;
    
    template<typename Type, template<typename,typename> class Conv>
    void swap(Image<Type,Conv>& a, Image<Type,Conv>& b);

    template<typename Type, template<typename,typename> class Conv>
    class Image {
	public:
	    typedef Type ColorType;
	    typedef Type* PtrType;
	    typedef const Type* ConstPtrType;
	    typedef Type& RefType;
	    typedef const Type& ConstRefType;

	    typedef PtrType iterator;
	    typedef ConstPtrType const_iterator;
	    
	    Image()
		: my_width(0), my_height(0), my_data(NULL), my_row(NULL)
	    {
		// empty
	    }

	    Image(size_t w, size_t h)
		: my_width(0), my_height(0), my_data(NULL), my_row(NULL)
	    {
		allocate(w, h);
	    }

	    Image(const Image& img)
		: my_width(0), my_height(0), my_data(NULL), my_row(NULL)
	    {
		*this = img;
	    }

	    ~Image() { 
		delete [] my_data; 
		delete [] my_row; 
	    }

	    size_t width() const { return my_width; }
	    size_t height() const { return my_height; }
	    size_t size() const { return my_width*my_height; }
	    size_t channels() const { return ColorTrait<Type>::channels(); }

	    void fill(ConstRefType pixel){
		std::fill(begin(), end(), pixel);
	    }

	    void allocate(size_t w, size_t h){
		if(w != my_width || h != my_height){
		    // the delete operator will automatically check for NULL
		    delete [] my_data;
		    delete [] my_row;
		    
		    if(w != 0 && h != 0){
			my_data = new Type[w*h];
			my_row = new Type*[h];
			my_width = w;
			my_height = h;
			init_row();
		    }else{
			my_width = my_height = 0;
			my_data = NULL;
			my_row = NULL;
		    }
		}
	    }

	    RefType operator ()(int x, int y) { return my_row[y][x]; }
	    ConstRefType operator ()(int x, int y) const { return my_row[y][x]; }

	    // If you turn on /Wp64, VC warns when size_t is passed, so I added this
	    RefType operator ()(size_t x, size_t y) { return my_row[y][x]; }
	    ConstRefType operator ()(size_t x, size_t y) const { return my_row[y][x]; }

	    // the non-const one is essensial or we'll get ambiguous call. 
	    Type operator ()(double x, double y) { return lerp(x,y); }
	    Type operator ()(double x, double y) const { return lerp(x,y); }
	    Type operator ()(float x, float y) { return lerp(x,y); }
	    Type operator ()(float x, float y) const { return lerp(x,y); }
	    // bilinear interpolation, quite useful
	    Type lerp(double x, double y) const;

	    Image& operator =(const Image& img){
		if(this != &img){
		    allocate(img.width(), img.height());
		    std::copy(img.begin(), img.end(), begin());
		}
		return *this;
	    }

	    iterator begin() { return my_data; }
	    const_iterator begin() const { return my_data; }
	    iterator end() { return my_data + my_width*my_height; }
	    const_iterator end() const { return my_data + my_width*my_height; }

	    
	    friend void swap<>(Image<Type>& a, Image<Type>& b);

	    // converter
	    struct Converter {
		typedef Type Internal;
		
		template <typename External>
		static void ext2int(Internal& to, const External& from){
		    Conv<Internal,External>::convert(to, from);
		}
		
		template <typename External>
		static void int2ext(External& to, const Internal& from){
		    Conv<External,Internal>::convert(to, from);
		}
	    };

	protected:
	    void init_row(){
		my_row[0] = my_data;
		for (size_t i = 1; i < my_height; ++i)
		    my_row[i] = my_row[i-1] + my_width;
	    }

	private:
	    size_t my_width;
	    size_t my_height;
	    Type *my_data;
	    Type **my_row;
    };

    template<typename Type, template<typename,typename> class Conv>
    Type Image<Type,Conv>::lerp(double x, double y) const
    {
	// we assert that  0 <= x <= width-1 and 0 <= y <= height-1
	int x0 = static_cast<int>(x);
	int y0 = static_cast<int>(y);
	double xf = x - x0;
	double yf = y - y0;

	if(xf == 0 && yf == 0)
	    return my_row[y0][x0];
	
	if(xf == 0){
	    return mix( my_row[y0][x0], my_row[y0+1][x0], yf );
	}else if(yf == 0){
	    return mix( my_row[y0][x0], my_row[y0][x0+1], xf );
	}else{
	    return mix(
		mix( my_row[y0][x0], my_row[y0][x0+1], xf ),
		mix( my_row[y0+1][x0], my_row[y0+1][x0+1], xf ),
		yf );
	}
    }

    template<typename Type, template<typename,typename> class Conv>
    void swap(Image<Type,Conv>& a, Image<Type,Conv>& b)
    {
	using std::swap;
	swap(a.my_width, b.my_width);
	swap(a.my_height, b.my_height);
	swap(a.my_data, b.my_data);
	swap(a.my_row, b.my_row);
    }


    typedef Image<Byte1> ByteImage1;
    typedef Image<Byte3> ByteImage3;
    typedef Image<Byte4> ByteImage4;
    typedef Image<Short1> ShortImage1;
    typedef Image<Short3> ShortImage3;
    typedef Image<Short4> ShortImage4;
    typedef Image<Float1> FloatImage1;
    typedef Image<Float3> FloatImage3;
    typedef Image<Float4> FloatImage4;

} // namespace gil

#endif // GIL_IMAGE_H
