/*
 * Deimos tool library - Tobias Alexander Franke 2003
 * For copyright and license see LICENSE
 * http://www.tobias-franke.eu
 */

#if !defined(DEIMOS_IMAGE_TGA__)
#define DEIMOS_IMAGE_TGA__

#include <string>
#include <iostream>
#include <algorithm>

#include "image.h"

namespace deimos {
namespace image {

class ImageTga : public Image
{
private:
	struct tTgaFileHeader
	{
		char			cCharacteristic;
		char			cColorMapType;
		char			cImageTypeCode;
		unsigned short	usColorMapOrigin;
		unsigned short	usColorMapLength;
		char			cColorMapEntrySize;
		unsigned short	usXOrigin;
		unsigned short	usYOrigin;
		unsigned short	usWidth;
		unsigned short	usHeight;
		char			cBitsPerPixel;
		char			cImageDescriptor;
	};

protected:
	inline void do_load_24(std::ifstream& stream);
	inline void do_load_32(std::ifstream& stream);

	inline void do_save_24(std::ofstream& stream) const;
	inline void do_save_32(std::ofstream& stream) const;

	bool do_load(endian_ifstream& stream);
	bool do_save(endian_ofstream& stream) const;

public:
	ImageTga();
	virtual ~ImageTga();

};

} // namespace image
} // namespace deimos

#endif // DEIMOS_IMAGE_TGA__
