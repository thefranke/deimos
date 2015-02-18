/* 
 * endian_stream.h by Tobias Alexander Franke (tob@cyberhead.de) 2004 
 * For copyright and license see LICENSE
 */

#if !defined(DEIMOS_ENDIAN_STREAM__)
#define DEIMOS_ENDIAN_STREAM__

#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>

#include <boost/detail/endian.hpp>

namespace deimos {

template<typename type, typename interf>
class endian_stream : public type
{
protected:
	bool need_convert_;
	endian_stream(){};
	
	void convert_data(char* buffer, unsigned int byte_count)
	{
		if(byte_count == 2 || byte_count == 4)
			for (unsigned int i=0; (i*2) < byte_count; ++i)
				std::swap(buffer[i], buffer[byte_count-1-i]);
	}

public:
	endian_stream(const char* filename, std::ios_base::openmode mode = std::ios_base::in) :
	  type(filename, mode), need_convert_(false)
	{
	}

	void toggle_convert()
	{
		need_convert_ = !need_convert_; 
	}

	interf& read(char* buffer, const std::streamsize size)
	{
		type::read(buffer, size);
		
		if(need_convert_)
			convert_data(buffer, size);

		return *this;
	}
	
	interf& write(const char* buffer, const std::streamsize size)
	{
		char* buf_copy = new char[size];
		std::copy(buffer, buffer + size, buf_copy);
		
		if(need_convert_)
			convert_data(buf_copy, size);
			
		type::write(buf_copy, size);
		
		delete [] buf_copy;

		return *this;
	}
};

typedef endian_stream<std::ifstream, std::istream> endian_ifstream;
typedef endian_stream<std::ofstream, std::ostream> endian_ofstream;

} // namespace deimos

#endif // DEIMOS_ENDIAN_STREAM__