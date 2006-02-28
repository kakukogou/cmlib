#ifndef GIL_EXCEPTION_H
#define GIL_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace gil {

    // this exception will be thrown when the image file is corrupt
    class InvalidFormat : std::runtime_error {
	public:
	    InvalidFormat(const std::string& msg)
		: std::runtime_error(msg)	
	    {
		// empty
	    }
    };

    // throw this if you meet an unexpected end of file
    class EndOfFile : std::runtime_error {
	public:
	    EndOfFile(const std::string& msg)
		: std::runtime_error(msg)
	    {
		// empty
	    }
    };

    // throw this if you meed an unknow IO error
    class FileError : std::runtime_error {
	public:
	    FileError(const std::string& msg)
		: std::runtime_error(msg)
	    {
		// empty
	    }
    };
    
} // namespace gil

#endif // GIL_EXCEPTION_H