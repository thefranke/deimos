/*
 * Deimos tool library - Tobias Alexander Franke 2001
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#include "image.h"

namespace deimos {
namespace image {

Image::Image() :
	raw_data_(0), width_(0), height_(0)
{

}

Image::Image(const char* filename) :
	raw_data_(0), width_(0), height_(0)
{
	load(filename);
}

Image::Image(const Image& image) :
	raw_data_(0), width_(0), height_(0)
{
	this->operator=(image);
}

Image::~Image()
{
	delete [] raw_data_;
}

Image& Image::operator =(const Image& image)
{
	if(raw_data_)
		delete [] raw_data_;

	width_ = image.width_;
	height_ = image.height_;
	bytes_per_pixel_ = image.bytes_per_pixel_;

	const size_t data_size = width_ * height_ * bytes_per_pixel_;

	raw_data_ = new unsigned char[data_size];

	for (size_t i = 0; i < data_size; ++i)
		raw_data_[i] = image.raw_data_[i];

	return *this;
}

bool Image::load(const char* filename)
{
	endian_ifstream stream(filename, std::ios_base::binary);

	if (stream.fail())
	{
		std::cout << "Image: Could not open file (read) " << filename << std::endl;
		return false;
	}

	// Discard possible old image
	delete [] raw_data_;

#ifdef BOOST_BIG_ENDIAN
	stream.toggle_convert();
#endif

	bool ret = do_load(stream);

	stream.close();

	return ret;
}

bool Image::save(const char* filename) const
{
	endian_ofstream stream(filename, std::ios_base::binary);

	if (stream.fail())
	{
		std::cout << "Image: Could not open file (write) " << filename << std::endl;
		return false;
	}

#ifdef BOOST_BIG_ENDIAN
	stream.toggle_convert();
#endif

	bool ret = do_save(stream);

	stream.close();

	return ret;
}

void Image::get_color(unsigned int x, unsigned int y, unsigned char* p_color) const
{
	assert(p_color);

	for (unsigned int b = 0; b < bytes_per_pixel_; ++b)
		p_color[b] = raw_data_[(width_ * bytes_per_pixel_ * y) + (x * bytes_per_pixel_) + b];
}

void Image::set_color(unsigned int x, unsigned int y, const unsigned char* p_color)
{
	assert(p_color);

	for (unsigned int b = 0; b < bytes_per_pixel_; ++b)
		raw_data_[(width_ * bytes_per_pixel_ * y) + (x * bytes_per_pixel_) + b] = p_color[b];
}

} // namespace image
} // namespace deimos
