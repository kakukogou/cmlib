#ifndef CONVOLUTION_H
#define CONVOLUTION_H

namespace gil {
	/*
	class Kernel {
		public:
			
		protected:
		private:
	}
	*/

	template<typename I, typename K>
	void convolute(I &dst, const I &src, const K kernel)
	{
	}

	template<typename I, typename K>
	I convolute(const I &src, const K kernel)
	{
		I dst(src.width(), src.height());
		convolute(dst, src, kernel);
		return dst;
	}
}

#endif
