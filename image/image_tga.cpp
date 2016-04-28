/*
 * Deimos tool library - Tobias Alexander Franke 2003
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#include "image_tga.h"

//#define DEBUG__

namespace deimos {
namespace image {

ImageTga::ImageTga()
{

}

ImageTga::~ImageTga()
{

}

bool ImageTga::do_load(endian_ifstream& stream)
{
	tTgaFileHeader tgaFileHeader;

	stream.read((char*)&tgaFileHeader.cCharacteristic, 1);
	stream.read((char*)&tgaFileHeader.cColorMapType, 1);
	stream.read((char*)&tgaFileHeader.cImageTypeCode, 1);
	stream.read((char*)&tgaFileHeader.usColorMapOrigin, 2);
	stream.read((char*)&tgaFileHeader.usColorMapLength, 2);
	stream.read((char*)&tgaFileHeader.cColorMapEntrySize, 1);
	stream.read((char*)&tgaFileHeader.usXOrigin, 2);
	stream.read((char*)&tgaFileHeader.usYOrigin, 2);
	stream.read((char*)&tgaFileHeader.usWidth, 2);
	stream.read((char*)&tgaFileHeader.usHeight, 2);
	stream.read((char*)&tgaFileHeader.cBitsPerPixel, 1);
	stream.read((char*)&tgaFileHeader.cImageDescriptor, 1);

#ifdef DEBUG__

	cout << "cCharacteristic = " << static_cast<int>(tgaFileHeader.cCharacteristic) << endl;
	cout << "cColorMapType = " << static_cast<int>(tgaFileHeader.cColorMapType) << endl;
	cout << "cImageTypeCode = " << static_cast<int>(tgaFileHeader.cImageTypeCode) << endl;
	cout << "usColorMapOrigin = " << tgaFileHeader.usColorMapOrigin << endl;
	cout << "usColorMapLength = " << tgaFileHeader.usColorMapLength << endl;
	cout << "cColorMapEntrySize = " << static_cast<int>(tgaFileHeader.cColorMapEntrySize) << endl;
	cout << "usXOrigin = " << tgaFileHeader.usXOrigin << endl;
	cout << "usYOrigin = " << tgaFileHeader.usYOrigin << endl;
	cout << "usWidth = " << tgaFileHeader.usWidth << endl;
	cout << "usHeight = " << tgaFileHeader.usHeight << endl;
	cout << "cBitsPerPixel = " << static_cast<int>(tgaFileHeader.cBitsPerPixel) << endl;
	cout << "cImageDescriptor = " << static_cast<int>(tgaFileHeader.cImageDescriptor) << endl;

#endif // DEBUG__

	// No colormap support
	if(tgaFileHeader.cColorMapType)
	{
		std::cout << "ImageTga: error (no colormap support)" << std::endl;
		return false;
	}

	// Format not supported
	if(tgaFileHeader.cImageTypeCode != 2)
	{
		std::cout << "ImageTga: error (wrong color format)" << std::endl;
		return false;
	}

	width_ = tgaFileHeader.usWidth;
	height_ = tgaFileHeader.usHeight;
	bytes_per_pixel_ = tgaFileHeader.cBitsPerPixel / 8;

	// Leave possible image description alone
	stream.seekg(tgaFileHeader.cCharacteristic, std::ios_base::cur);

	if (!(raw_data_ = new unsigned char[width_ * height_ * bytes_per_pixel_]))
	{
		std::cout << "ImageTga: error (couldn\'t allocate memory)" << std::endl;
		return false;
	}

	switch(bytes_per_pixel_)
	{
		case 3:
			do_load_24(stream);
			break;
		case 4:
			do_load_32(stream);
			break;
		default:
			std::cout << "ImageTga: Format not supported (" << bytes_per_pixel_ << "bit)" << std::endl;
			assert(0);
			break;
	}

	return true;
}

bool ImageTga::do_save(endian_ofstream& stream) const
{
	tTgaFileHeader tgaFileHeader;

	const std::string comment = "visit http://www.cyberhead.de/";

	tgaFileHeader.cCharacteristic		= static_cast<char>(comment.length());
	tgaFileHeader.cColorMapType			= 0;
	tgaFileHeader.cImageTypeCode		= 2;
	tgaFileHeader.usColorMapOrigin		= 0;
	tgaFileHeader.usColorMapLength		= 0;
	tgaFileHeader.cColorMapEntrySize	= 0;
	tgaFileHeader.usXOrigin				= 0;
	tgaFileHeader.usYOrigin				= 0;
	tgaFileHeader.usWidth				= width_;
	tgaFileHeader.usHeight				= height_;
	tgaFileHeader.cBitsPerPixel			= bytes_per_pixel_ * 8;
	tgaFileHeader.cImageDescriptor		= 0;

	stream.write((char*)&tgaFileHeader.cCharacteristic, 1);
	stream.write((char*)&tgaFileHeader.cColorMapType, 1);
	stream.write((char*)&tgaFileHeader.cImageTypeCode, 1);
	stream.write((char*)&tgaFileHeader.usColorMapOrigin, 2);
	stream.write((char*)&tgaFileHeader.usColorMapLength, 2);
	stream.write((char*)&tgaFileHeader.cColorMapEntrySize, 1);
	stream.write((char*)&tgaFileHeader.usXOrigin, 2);
	stream.write((char*)&tgaFileHeader.usYOrigin, 2);
	stream.write((char*)&tgaFileHeader.usWidth, 2);
	stream.write((char*)&tgaFileHeader.usHeight, 2);
	stream.write((char*)&tgaFileHeader.cBitsPerPixel, 1);
	stream.write((char*)&tgaFileHeader.cImageDescriptor, 1);
	stream.write(comment.c_str(), static_cast<char>(comment.length()));

	switch(bytes_per_pixel_)
	{
		case 3:
			do_save_24(stream);
			break;
		case 4:
			do_save_32(stream);
			break;
		default:
			assert(0);
			break;
	}

	return true;
}

void ImageTga::do_load_24(std::ifstream& stream)
{
	stream.read((char*)raw_data_, width_ * height_ * bytes_per_pixel_);

	for (unsigned int i=0; i<width_ * height_ * bytes_per_pixel_; i+=3)
	{
		// Swap BGR to RGB
		std::swap(raw_data_[i + 0], raw_data_[i + 2]);
	}
}

void ImageTga::do_save_24(std::ofstream& stream) const
{
	for (unsigned int i=0; i<width_ * height_ * bytes_per_pixel_; i+=3)
	{
		// Swap RGB to BGR
		std::swap(raw_data_[i + 0], raw_data_[i + 2]);
	}

	stream.write((char*)raw_data_, width_ * height_ * bytes_per_pixel_);
}

void ImageTga::do_load_32(std::ifstream& stream)
{
	stream.read((char*)raw_data_, width_ * height_ * bytes_per_pixel_);

	for (unsigned int i=0; i<width_ * height_ * bytes_per_pixel_; i+=4)
	{
		// Swap BGRA to RGBA
		std::swap(raw_data_[i + 0], raw_data_[i + 2]);
	}
}

void ImageTga::do_save_32(std::ofstream& stream) const
{
	for (unsigned int i=0; i<width_ * height_ * bytes_per_pixel_; i+=4)
	{
		// Swap RGBA to BGRA
		std::swap(raw_data_[i + 0], raw_data_[i + 2]);
	}

	stream.write((char*)raw_data_, width_ * height_ * bytes_per_pixel_);
}

} // namespace image
} // namespace deimos
