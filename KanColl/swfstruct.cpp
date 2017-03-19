#include "stdafx.h"
#include "swfstruct.h"
#include "LByteStream.h"
#include "LBitsField.h"
#include "QtZlib/zlib.h"

void SwfRect::fromStream(LByteStream& stream)
{
	int maxbyte = sizeof(SwfRect) + 1;
	std::vector<uchar> raw(maxbyte);
	stream.getAs(raw.data(), raw.size());
	
	const size_t sizeWidth = 5;
	LBitsField fields(raw.data(), raw.data() + raw.size());
	size_t offset = 0;
	int nbits = fields.getUbyte(offset, sizeWidth);
	xmin = fields.getSint(offset += sizeWidth, nbits);
	xmax = fields.getSint(offset += nbits, nbits);
	ymin = fields.getSint(offset += nbits, nbits);
	ymax = fields.getSint(offset += nbits, nbits);
	offset += nbits;

	bool bAdd = (0 != (offset & 0x7));
	int nSkip = (offset >> 3);
	if (bAdd)
		++nSkip;
	stream.skip(nSkip);
}

void SwfMATRIX::fromStream(LByteStream& stream)
{
	int maxbyte = sizeof(SwfMATRIX) + 3;
	std::vector<uchar> raw(maxbyte);
	stream.getAs(raw.data(), raw.size());

	const size_t sizeWidth = 5;
	LBitsField fields(raw.data(), raw.data() + raw.size());
	size_t offset = 0;
	HasScale = fields.getUbyte(offset, 1);
	if (HasScale)
	{
		int nbits = fields.getUbyte(offset, sizeWidth);
		ScaleX.val = fields.getUint(offset += sizeWidth, nbits);
		ScaleY.val = fields.getUint(offset += nbits, nbits);
		offset += nbits;
	}
	HasRotate = fields.getUbyte(offset, 1);
	if (HasRotate)
	{
		int nbits = fields.getUbyte(offset, sizeWidth);
		RotateSkew0.val = fields.getUint(offset += sizeWidth, nbits);
		RotateSkew1.val = fields.getUint(offset += nbits, nbits);
		offset += nbits;
	}
	{
		int nbits = fields.getUbyte(offset, sizeWidth);
		TranslateX = fields.getSint(offset += sizeWidth, nbits);
		TranslateY = fields.getSint(offset += nbits, nbits);
		offset += nbits;
	}

	bool bAdd = (0 != (offset & 0x7));
	int nSkip = (offset >> 3);
	if (bAdd)
		++nSkip;
	stream.skip(nSkip);
}

void SwfTagHeader::fromStream(LByteStream& stream)
{
	stream.readAs<SwfTagHeaderBase>(*this);
	if (TagLength == 0x3F)
		length = stream.read<int>();
	else
		length = TagLength;
}

void EncondedU32::fromStream(LByteStream& stream)
{
	value = stream.read<uchar>();
	if (!(value & 0x00000080))
		return;

	value = (value & 0x0000007f) | stream.read<uchar>() << 7;
	if (!(value & 0x00004000))
		return;

	value = (value & 0x00003fff) | stream.read<uchar>() << 14;
	if (!(value & 0x00200000))
		return;

	value = (value & 0x001fffff) | stream.read<uchar>() << 21;
	if (!(value & 0x10000000))
		return;

	value = (value & 0x0fffffff) | stream.read<uchar>() << 28;
}

QImage TagDefineBits::image(size_t tagSize) const
{
	return QImage::fromData(JPEGData, JPEGDataSize(tagSize));
}

QImage TagJPEGTables::image(size_t tagSize) const
{
	return QImage::fromData(JPEGData, tagSize);
}

QImage TagDefineBitsJPEG2::image(size_t tagSize) const
{
	UI8* pData = (UI8*)ImageData;
	size_t szImg = ImageDataSize(tagSize);
	if (szImg > 4 && *(UI32*)pData == 0xD9FFD8FF)
	{
		pData += 4;
		szImg -= 4;
	}
	return QImage::fromData(pData, szImg);
}

SwfImgType checkImgType(const uchar* pData)
{
	const ushort PNG_JPEG = 0xD8FF;
	const byte SIG_PNG[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	const byte SIG_GIF89a[] = { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 };

	if (*(ushort*)pData == PNG_JPEG)
		return imgJPEG;
	else if (0 == memcmp(SIG_PNG, pData, sizeof(SIG_PNG)))
		return imgPNG;
	else if (0 == memcmp(SIG_GIF89a, pData, sizeof(SIG_GIF89a)))
		return imgGIF89a;
	else
		return imgUnknown;
}

QImage TagDefineBitsJPEG3::image(size_t tagSize) const
{
	QImage img = QImage::fromData(ImageData, AlphaDataOffset);
	if (imgJPEG == checkImgType(ImageData))
	{
		size_t alphaSize = AlphaDataSize(tagSize);
		if (!alphaSize)
			return img;

		uLongf UncompressedSize = img.width() * img.height();
		QByteArray dest(UncompressedSize, 0);
		int res = uncompress((uchar*)dest.data(), &UncompressedSize, BitmapAlphaData(), alphaSize);
		if (Z_OK != res)
			return img;

		uchar* pAlpha = (uchar*)dest.data();
		QImage alphaCh(img.width(), img.height(), QImage::Format_Alpha8);
		for (int y = 0; y < img.height(); ++y)
		{
			for (int x = 0; x < img.width(); ++x)
			{
				alphaCh.setPixelColor(x, y, QColor(0, 0, 0, *pAlpha));
				++pAlpha;
			}
		}
		img.setAlphaChannel(alphaCh);
	}
	return img;
}

QImage TagDefineBitsLossless::image(size_t tagSize) const
{
	return QImage();
}

QImage TagDefineBitsLossless2::image(size_t tagSize) const
{
	switch (BitmapFormat)
	{
 	case ftColorMapped8:
		return imageMapped8(tagSize);
		break;
	case ftRGB32:
		return imageRGB32(tagSize);
	default:
		DebugBreak();
		break;
	}
	return QImage();
}

QImage TagDefineBitsLossless2::imageMapped8(size_t tagSize) const
{
	QImage img(BitmapWidth, BitmapHeight, QImage::Format_ARGB32_Premultiplied);

	uLongf UncompressedSize = img.width() * img.height() * sizeof(UI32);
	QByteArray table(UncompressedSize, 0);
	int res = uncompress((uchar*)table.data(), &UncompressedSize, mappedDat.ColorMappedData, MappedDataSize(tagSize));
	if (Z_OK != res)
		return img;

	SwfRGBA* pTable = (SwfRGBA*)table.data();
	UI8* pIndex = (UI8*)(pTable + mappedDat.BitmapColorTableSize + 1);
	for (int y = 0; y < img.height(); ++y)
	{
		for (int x = 0; x < img.width(); ++x)
		{
			SwfRGBA& clr = pTable[*pIndex];
			img.setPixelColor(x, y, QColor(clr.red, clr.green, clr.blue, clr.alpha));
			++pIndex;
		}
	}
	return img;
}

QImage TagDefineBitsLossless2::imageRGB32(size_t tagSize) const
{
	QImage img(BitmapWidth, BitmapHeight, QImage::Format_ARGB32_Premultiplied);

	uLongf UncompressedSize = img.width() * img.height() * sizeof(UI32);
	QByteArray dest(UncompressedSize, 0);
	int res = uncompress((uchar*)dest.data(), &UncompressedSize, bitmapDat.BitmapPixelData, BitmapDataSize(tagSize));
	if (Z_OK != res)
		return img;

	SwfARGB* p = (SwfARGB*)dest.data();
	for (int y = 0; y < img.height(); ++y)
	{
		for (int x = 0; x < img.width(); ++x)
		{
			img.setPixelColor(x, y, QColor(p->red, p->green, p->blue, p->alpha));
			++p;
		}
	}
	return img;
}

QImage TagDefineBitsJPEG4::image(size_t tagSize) const
{
	QImage img = QImage::fromData(ImageData, AlphaDataOffset);
	if (imgJPEG == checkImgType(ImageData))
	{
		size_t alphaSize = AlphaDataSize(tagSize);
		if (!alphaSize)
			return img;

		uLongf UncompressedSize = img.width() * img.height();
		QByteArray dest(UncompressedSize, 0);
		int res = uncompress((uchar*)dest.data(), &UncompressedSize, BitmapAlphaData(), alphaSize);
		if (Z_OK != res)
			return img;

		uchar* pAlpha = (uchar*)dest.data();
		QImage alphaCh(img.width(), img.height(), QImage::Format_Alpha8);
		for (int y = 0; y < img.height(); ++y)
		{
			for (int x = 0; x < img.width(); ++x)
			{
				alphaCh.setPixelColor(x, y, QColor(0, 0, 0, *pAlpha));
				++pAlpha;
			}
		}
		img.setAlphaChannel(alphaCh);
	}
	return img;
}

void TagDefineShape::fromStream(LByteStream& stream, size_t tagSize)
{
	size_t begin = stream.tell();
	stream.readAs(ShapeId);
	ShapeBounds.fromStream(stream);

	datSize = tagSize - (stream.tell() - begin);
	stream.readAs((uchar*)Shapes, datSize);
}

void TagDefineShape4::fromStream(LByteStream& stream, size_t tagSize)
{
	size_t begin = stream.tell();
	stream.readAs(ShapeId);
	ShapeBounds.fromStream(stream);
	EdgeBounds.fromStream(stream);
	stream.readAs(bits);

	datSize = tagSize - (stream.tell() - begin);
	stream.readAs((uchar*)Shapes, datSize);
}
