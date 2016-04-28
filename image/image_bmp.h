/*
 * Deimos tool library - Tobias Alexander Franke 2001
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#if !defined(DEIMOS_IMAGE_BMP__)
#define DEIMOS_IMAGE_BMP__

#include <string>
#include <cassert>
#include <iostream>

#include "image.h"

namespace deimos {
namespace image {

class ImageBmp : public Image
{
private:
	struct tBmpFileHeader
	{
		unsigned short bfType;
		unsigned int   bfSize;
		unsigned short bfReserved1;
		unsigned short bfReserved2;
		unsigned int   bfOffBits;
	};

	struct tBmpInfoHeader
	{
		unsigned int   biSize;
		int            biWidth;
		int            biHeight;
		unsigned short biPlanes;
		unsigned short biBitCount;
		unsigned int   biCompression;
		unsigned int   biSizeImage;
		int            biXPelsPerMeter;
		int            biYPelsPerMeter;
		unsigned int   biClrUsed;
		unsigned int   biClrImportant;
	};

protected:
	inline void do_load_24(std::ifstream& stream);
	inline void do_load_8 (std::ifstream& stream);

	inline void do_save_24(std::ofstream& stream) const;
	inline void do_save_8 (std::ofstream& stream) const;

	bool do_load(endian_ifstream& stream);
	bool do_save(endian_ofstream& stream) const;

public:
	ImageBmp();
	virtual ~ImageBmp();

};

} // namespace image
} // namespace deimos

#endif // DEIMOS_IMAGE_BMP__
