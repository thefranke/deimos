/*
 * Deimos tool library - Tobias Alexander Franke 2001
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#if !defined(DEIMOS_IMAGE__)
#define DEIMOS_IMAGE__

#include <iostream>
#include <string>
#include <cassert>

#include "../stream/endian_stream.h"

namespace deimos {
namespace image {

class Image
{
protected:
	unsigned char* raw_data_;
	unsigned int width_, height_, bytes_per_pixel_;

	virtual bool do_load(endian_ifstream& stream) = 0;
	virtual bool do_save(endian_ofstream& stream) const { return false; };

public:
	Image();
	Image(const Image& image);
	Image(const char* filename);
	Image& operator=(const Image& image);

	inline unsigned int get_width() const { return width_; };
	inline unsigned int get_height() const { return height_; };
	inline unsigned int get_bytes_per_pixel() const { return bytes_per_pixel_; };

	inline const unsigned char* const get_data() const { return raw_data_; };

	void get_color(unsigned int x, unsigned int y, unsigned char* p_color) const;
	void set_color(unsigned int x, unsigned int y, const unsigned char* p_color);

	bool load(const char* filename);
	bool save(const char* filename) const;

	virtual ~Image();

};

} // namespace image
} // namespace deimos

#endif // DEIMOS_IMAGE__
