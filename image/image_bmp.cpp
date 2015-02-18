/* 
 * image_bmp.cpp by Tobias Alexander Franke (tob@cyberhead.de) 2001 
 * For copyright and license see LICENSE
 */

#include "image_bmp.h"

namespace deimos {
namespace image {

ImageBmp::ImageBmp()
{

}

ImageBmp::~ImageBmp()
{

}

bool ImageBmp::do_load(endian_ifstream& stream)
{
	tBmpFileHeader bmp_file_header;
	tBmpInfoHeader bmp_info_header;

	stream.read((char*)&bmp_file_header.bfType, 2);
	stream.read((char*)&bmp_file_header.bfSize, 4);
	stream.read((char*)&bmp_file_header.bfReserved1, 2);
	stream.read((char*)&bmp_file_header.bfReserved2, 2);
	stream.read((char*)&bmp_file_header.bfOffBits, 4);
	
	stream.read((char*)&bmp_info_header.biSize, 4);
	stream.read((char*)&bmp_info_header.biWidth, 4);
	stream.read((char*)&bmp_info_header.biHeight, 4);
	stream.read((char*)&bmp_info_header.biPlanes, 2);
	stream.read((char*)&bmp_info_header.biBitCount, 2);
	stream.read((char*)&bmp_info_header.biCompression, 4);
	stream.read((char*)&bmp_info_header.biSizeImage, 4);
	stream.read((char*)&bmp_info_header.biXPelsPerMeter, 4);
	stream.read((char*)&bmp_info_header.biYPelsPerMeter, 4);
	stream.read((char*)&bmp_info_header.biClrUsed, 4);
	stream.read((char*)&bmp_info_header.biClrImportant, 4);

#ifdef DEBUG__
	
	cout << "READ" << endl;
	cout << "bmp_file_header" << endl;
	cout << "type = " << bmp_file_header.bfType << endl;
	cout << "size = " << bmp_file_header.bfSize << endl;
	cout << "offbits = " << bmp_file_header.bfOffBits << endl;
	cout << "bmp_info_header" << endl;
	cout << "bitcount = " << bmp_info_header.biBitCount << endl;
	cout << "clrimportant = " << bmp_info_header.biClrImportant << endl;
	cout << "clrused = " << bmp_info_header.biClrUsed << endl;
	cout << "compression = " << bmp_info_header.biCompression << endl;
	cout << "height = " << bmp_info_header.biHeight << endl;
	cout << "planes = " << bmp_info_header.biPlanes << endl;
	cout << "size = " << bmp_info_header.biSize << endl;
	cout << "sizeimage = " << bmp_info_header.biSizeImage << endl;
	cout << "width = " << bmp_info_header.biWidth << endl;
	cout << "xpels = " << bmp_info_header.biXPelsPerMeter << endl;
	cout << "ypels = " << bmp_info_header.biYPelsPerMeter << endl;
	cout << endl;

#endif // DEBUG__

	// Jump to actual data (this is needed because of bugs in packages like Photoshop 5.0 
	// where the header was aligned to 4 bytes and is thus 2 bytes too long).
	stream.seekg(bmp_file_header.bfOffBits, std::ios_base::beg);

	// Format not supported (wrong color format)
	if(!(bmp_info_header.biClrUsed == 0 || bmp_info_header.biClrUsed == 24))
	{
		std::cout << "ImageBmp: error (wrong color format)" << std::endl;
		return false;
	}

	// Format not supported (wrong compression mode)
	if(bmp_info_header.biCompression != 0)
	{
		std::cout << "ImageBmp: error (no compression support)" << std::endl;
		return false;
	}

	bytes_per_pixel_ = static_cast<int>(bmp_info_header.biBitCount/8);
	width_ = bmp_info_header.biWidth;
	height_ = bmp_info_header.biHeight;

	if (!(raw_data_ = new unsigned char[width_ * height_ * bytes_per_pixel_]))
	{
		std::cout << "ImageBmp: error (couldn\'t allocate memory)" << std::endl;
		return false;
	}

	switch(bytes_per_pixel_)
	{
		case 3:
			do_load_24(stream);
			break;
		case 1:
			do_load_8(stream);
			break;
		default:
			// Format not supported
			std::cout << "ImageBmp: Format not supported (" << bytes_per_pixel_ << "bit)" << std::endl;
			assert(0);
			break;
	}

	return true;
}

bool ImageBmp::do_save(endian_ofstream& stream) const
{
	tBmpFileHeader bmp_file_header;
	tBmpInfoHeader bmp_info_header;
	
	const int size_file_header = 14; // sizeof(tBmpFileHeader)
	const int size_info_header = 40; // sizeof(tBmpInfoHeader)
	const int header_size = size_file_header + size_info_header;
	const int color_table_size = (bytes_per_pixel_ < 3) ?  (1 << (bytes_per_pixel_ * 8)) * 4 : 0;
	
	long data_size = width_ * height_ * bytes_per_pixel_;
	data_size += data_size%4;

	bmp_file_header.bfType		= ('M' << 8) + 'B';
	bmp_file_header.bfSize		= header_size + data_size + color_table_size;
	bmp_file_header.bfReserved1	= 0;
	bmp_file_header.bfReserved2	= 0;
	bmp_file_header.bfOffBits		= header_size + color_table_size;

	stream.write((char*)&bmp_file_header.bfType, 2);
	stream.write((char*)&bmp_file_header.bfSize, 4);
	stream.write((char*)&bmp_file_header.bfReserved1, 2);
	stream.write((char*)&bmp_file_header.bfReserved2, 2);
	stream.write((char*)&bmp_file_header.bfOffBits, 4);
	
	bmp_info_header.biSize			= size_info_header;
	bmp_info_header.biWidth			= width_;
	bmp_info_header.biHeight		= height_;
	bmp_info_header.biPlanes		= 1;
	bmp_info_header.biBitCount		= bytes_per_pixel_ * 8;
	bmp_info_header.biCompression	= 0;
	bmp_info_header.biSizeImage		= data_size;
	bmp_info_header.biXPelsPerMeter	= 2800;
	bmp_info_header.biYPelsPerMeter	= 2800;
	bmp_info_header.biClrUsed		= 0;
	bmp_info_header.biClrImportant	= 0;

	stream.write((char*)&bmp_info_header.biSize, 4);
	stream.write((char*)&bmp_info_header.biWidth, 4);
	stream.write((char*)&bmp_info_header.biHeight, 4);
	stream.write((char*)&bmp_info_header.biPlanes, 2);
	stream.write((char*)&bmp_info_header.biBitCount, 2);
	stream.write((char*)&bmp_info_header.biCompression, 4);
	stream.write((char*)&bmp_info_header.biSizeImage, 4);
	stream.write((char*)&bmp_info_header.biXPelsPerMeter, 4);
	stream.write((char*)&bmp_info_header.biYPelsPerMeter, 4);
	stream.write((char*)&bmp_info_header.biClrUsed, 4);
	stream.write((char*)&bmp_info_header.biClrImportant, 4);

#ifdef DEBUG__
	
	cout << "WRITE" << endl;
	cout << "bmp_file_header" << endl;
	cout << "type = " << bmp_file_header.bfType << endl;
	cout << "size = " << bmp_file_header.bfSize << endl;
	cout << "offbits = " << bmp_file_header.bfOffBits << endl;
	cout << "bmp_info_header" << endl;
	cout << "bitcount = " << bmp_info_header.biBitCount << endl;
	cout << "clrimportant = " << bmp_info_header.biClrImportant << endl;
	cout << "clrused = " << bmp_info_header.biClrUsed << endl;
	cout << "compression = " << bmp_info_header.biCompression << endl;
	cout << "height = " << bmp_info_header.biHeight << endl;
	cout << "planes = " << bmp_info_header.biPlanes << endl;
	cout << "size = " << bmp_info_header.biSize << endl;
	cout << "sizeimage = " << bmp_info_header.biSizeImage << endl;
	cout << "width = " << bmp_info_header.biWidth << endl;
	cout << "xpels = " << bmp_info_header.biXPelsPerMeter << endl;
	cout << "ypels = " << bmp_info_header.biYPelsPerMeter << endl;
	cout << endl;

#endif // DEBUG__

	switch(bytes_per_pixel_)
	{
		case 3:
			do_save_24(stream);
			break;
		case 1:
			do_save_8(stream);
			break;
		default:
			// Format not supported
			assert(0);
			break;
	}

	return true;
}

void ImageBmp::do_load_24(std::ifstream& stream)
{
	const size_t row_size = width_ * bytes_per_pixel_;

	for(unsigned int y = 0; y < height_; ++y)
	{
		stream.read((char*)raw_data_ + (y * row_size), std::streamsize(row_size));

		// Swap BGR to RGB
		for(unsigned int x = 0; x < row_size; x+=3)
			std::swap(raw_data_[(y * row_size) + x + 0], raw_data_[(y * row_size) + x + 2]);

		// Align to 4 padding bytes
		stream.seekg(width_%4, std::ios_base::cur);
	}
}

void ImageBmp::do_save_24(std::ofstream& stream) const
{
	const size_t row_size = width_ * bytes_per_pixel_;

	for(unsigned int y = 0; y < height_; ++y)
	{
		for(unsigned int x = 0; x < row_size; x+=3)
			std::swap(raw_data_[(y * row_size) + x + 0], raw_data_[(y * row_size) + x + 2]);
		
		stream.write((char*)raw_data_ + (y * row_size), std::streamsize(row_size));

		// Align to 4 bytes
		stream.write("\0", width_%4);
	}
}

void ImageBmp::do_load_8(std::ifstream& stream)
{
	// Skip color index table
	stream.seekg(256*4, std::ios_base::cur);

	for(unsigned int y = 0; y < height_; ++y)
	{
		stream.read((char*)(raw_data_ + (y * width_)), width_);
		stream.seekg(width_%4, std::ios_base::cur);
	}
}

void ImageBmp::do_save_8(std::ofstream& stream) const
{
	char rgbt[4];
	rgbt[3] = 0;

	// Write color table
	for(unsigned int i = 0; i < 256; ++i)
	{
		rgbt[0] = static_cast<char>(i);
		rgbt[1] = static_cast<char>(i);
		rgbt[2] = static_cast<char>(i);
		stream.write((char*)rgbt, 4);
	}

	// Write data
	for(unsigned int y = 0; y < height_; ++y)
	{
		stream.write((char*)(raw_data_ + (y * width_)), width_);
		stream.write("\0", width_%4);
	}
}

} // namespace image
} // namespace deimos
