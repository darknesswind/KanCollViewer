#include "stdafx.h"
#include "swfstruct.h"
#include "LByteStream.h"
#include "LBitsField.h"
#include "QtZlib/zlib.h"
#include <cassert>

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

QRectF SwfRect::toRect() const
{
	const qreal twipPerPx = 20.0;
	QRectF rect;
	rect.setLeft(xmin / twipPerPx);
	rect.setRight(xmax / twipPerPx);
	rect.setTop(ymin / twipPerPx);
	rect.setBottom(ymax / twipPerPx);
	return rect;
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
		int nbits = fields.getUbyte(offset += 1, sizeWidth);
		ScaleX.val = fields.getUint(offset += sizeWidth, nbits);
		ScaleY.val = fields.getUint(offset += nbits, nbits);
		offset += nbits;
	}
	HasRotate = fields.getUbyte(offset, 1);
	if (HasRotate)
	{
		int nbits = fields.getUbyte(offset += 1, sizeWidth);
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

void TagUnknown::fromStream(LByteStream& stream)
{
	stream.readTo(m_raw, m_header.length);
	LByteStream rawStream(m_raw.data(), m_raw.size());
	parse(rawStream);
}

void TagPlaceObject2::fromStream(LByteStream& stream)
{
	TagUnknown::fromStream(stream);
	m_prop = (TagPlaceObject2Base*)m_raw.data();
	size_t optSize = m_header.length - sizeof(TagPlaceObject2Base);
	if (optSize > 0)
		m_option = { (byte*)(m_prop + 1), optSize };
}

void TagSetBackgroundColor::fromStream(LByteStream& stream)
{
	stream.readAs(m_rgb);
}

void TagFileAttributes::fromStream(LByteStream& stream)
{
	stream.readAs(m_prop);
}

void TagMetadata::fromStream(LByteStream& stream)
{
	stream.readTo(m_str, m_header.length);
}

void TagDefineShape::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	m_bounds.fromStream(stream);
	m_shapes = { stream.current(), stream.remindSize() };

	m_fillStyle.fromStream(stream, m_header.TagType);
}

void TagDefineShape4::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	m_bounds.fromStream(stream);
	m_edgeBounds.fromStream(stream);
	stream.readAs(m_flags);
	m_shapes = { stream.current(), stream.remindSize() };

	m_fillStyle.fromStream(stream, m_header.TagType);
}

void TagDefineBits::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	m_image = QImage::fromData(stream.current(), stream.remindSize());
}

void TagJPEGTables::parse(LByteStream& stream)
{
	m_character = 0;
	m_image = QImage::fromData(m_raw.data(), m_raw.size());
}

void TagDefineBitsJPEG2::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	if (stream.remindSize() > 4 && *(UI32*)stream.current() == 0xD9FFD8FF)
	{
		stream.skip(sizeof(UI32));
	}
	m_image = QImage::fromData(stream.current(), stream.remindSize());
}

void TagDefineBitsJPEG3::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	UI32 AlphaDataOffset = stream.read<UI32>();
	Segment segImageData = { stream.current(), AlphaDataOffset };
	stream.skip(segImageData.size);
	Segment segAlphaData = { stream.current(), stream.remindSize() };

	m_image = QImage::fromData(segImageData.ptr, segImageData.size);
	if (imgJPEG == checkImgType(segImageData.ptr))
	{
		if (!segAlphaData.size)
			return;

		uLongf UncompressedSize = m_image.width() * m_image.height();
		QByteArray dest(UncompressedSize, 0);
		int res = uncompress((uchar*)dest.data(), &UncompressedSize, segAlphaData.ptr, segAlphaData.size);
		if (Z_OK != res)
			return;

		uchar* pAlpha = (uchar*)dest.data();
		QImage alphaCh(m_image.width(), m_image.height(), QImage::Format_Alpha8);
		for (int y = 0; y < m_image.height(); ++y)
		{
			for (int x = 0; x < m_image.width(); ++x)
			{
				alphaCh.setPixelColor(x, y, QColor(0, 0, 0, *pAlpha));
				++pAlpha;
			}
		}
		m_image.setAlphaChannel(alphaCh);
	}
	else
	{
		assert(false);
	}
}

void TagDefineBitsLossless::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	UI8 BitmapFormat = stream.read<UI8>();
	UI16 BitmapWidth = stream.read<UI16>();
	UI16 BitmapHeight = stream.read<UI16>();;

	switch (BitmapFormat)
	{
	case ftColorMapped8:
		m_image = fromColorMappedRGB(stream, BitmapWidth, BitmapHeight);
		break;
	case ftRGB15:
		m_image = fromRGB<SwfPIX15>(stream, BitmapWidth, BitmapHeight);
		break;
	case ftRGB24:
		m_image = fromRGB<SwfPIX24>(stream, BitmapWidth, BitmapHeight);
		break;
	default:
		assert(false);
		break;
	}
}

QImage TagDefineBitsLossless::fromColorMappedRGB(LByteStream& stream, UI16 width, UI16 height)
{
	UI8 BitmapColorTableSize = stream.read<UI8>();
	Segment segColorMappedData = { stream.current(), stream.remindSize() };

	if (width % 4 != 0)
		width = (width / 4 + 1) * 4;

	QImage img(width, height, QImage::Format_ARGB32_Premultiplied);
	uLongf UncompressedSize = img.width() * img.height() + (BitmapColorTableSize + 1) * sizeof(SwfRGB);
	QByteArray table(UncompressedSize, 0);
	int res = uncompress((uchar*)table.data(), &UncompressedSize, segColorMappedData.ptr, segColorMappedData.size);
	if (Z_OK != res)
		return img;

	assert(table.size() == UncompressedSize);
	SwfRGB* pTable = (SwfRGB*)table.data();
	UI8* pIndex = (UI8*)(pTable + BitmapColorTableSize + 1);
	for (int y = 0; y < img.height(); ++y)
	{
		for (int x = 0; x < img.width(); ++x)
		{
			SwfRGB& clr = pTable[*pIndex];
			img.setPixelColor(x, y, QColor(clr.red, clr.green, clr.blue));
			++pIndex;
		}
	}
	return img;
}

template <typename RGBType>
QImage TagDefineBitsLossless::fromRGB(LByteStream& stream, UI16 width, UI16 height)
{
	Segment segPixelData = { stream.current(), stream.remindSize() };

	QImage img(width, height, QImage::Format_ARGB32_Premultiplied);

	uLongf UncompressedSize = img.width() * img.height() * sizeof(UI32);
	QByteArray dest(UncompressedSize, 0);
	int res = uncompress((uchar*)dest.data(), &UncompressedSize, segPixelData.ptr, segPixelData.size);
	if (Z_OK != res)
		return img;

	assert(dest.size() == UncompressedSize);
	RGBType* p = (RGBType*)dest.data();
	for (int y = 0; y < img.height(); ++y)
	{
		for (int x = 0; x < img.width(); ++x)
		{
			img.setPixelColor(x, y, QColor(p->red, p->green, p->blue));
			++p;
		}
	}
	return img;
}

void TagDefineBitsLossless2::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	UI8 BitmapFormat = stream.read<UI8>();
	UI16 BitmapWidth = stream.read<UI16>();
	UI16 BitmapHeight = stream.read<UI16>();;

	switch (BitmapFormat)
	{
	case ftColorMapped8:
		m_image = fromColorMappedRGBA(stream, BitmapWidth, BitmapHeight);
		break;
	case ftRGB32:
		m_image = fromRGB32(stream, BitmapWidth, BitmapHeight);
		break;
	default:
		assert(false);
		break;
	}
}
	
QImage TagDefineBitsLossless2::fromColorMappedRGBA(LByteStream& stream, UI16 width, UI16 height)
{
	UI8 BitmapColorTableSize = stream.read<UI8>();
	Segment segColorMappedData = { stream.current(), stream.remindSize() };

	if (width % 4 != 0)
		width = (width / 4 + 1) * 4;

	QImage img(width, height, QImage::Format_ARGB32_Premultiplied);
	uLongf UncompressedSize = img.width() * img.height() + (BitmapColorTableSize + 1) * sizeof(SwfRGBA);
	QByteArray table(UncompressedSize, 0);
	int res = uncompress((uchar*)table.data(), &UncompressedSize, segColorMappedData.ptr, segColorMappedData.size);
	if (Z_OK != res)
		return img;

	assert(table.size() == UncompressedSize);
	SwfRGBA* pTable = (SwfRGBA*)table.data();
	UI8* pIndex = (UI8*)(pTable + BitmapColorTableSize + 1);
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

QImage TagDefineBitsLossless2::fromRGB32(LByteStream& stream, UI16 width, UI16 height)
{
	Segment segPixelData = { stream.current(), stream.remindSize() };

	QImage img(width, height, QImage::Format_ARGB32_Premultiplied);

	uLongf UncompressedSize = img.width() * img.height() * sizeof(UI32);
	QByteArray dest(UncompressedSize, 0);
	int res = uncompress((uchar*)dest.data(), &UncompressedSize, segPixelData.ptr, segPixelData.size);
	if (Z_OK != res)
		return img;

	assert(dest.size() == UncompressedSize);
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

void TagDefineBitsJPEG4::parse(LByteStream& stream)
{
	stream.readAs(m_character);
	UI32 AlphaDataOffset = stream.read<UI32>();
	UI16 DeblockParam = stream.read<UI16>();
	Segment segImageData = { stream.current(), AlphaDataOffset };
	stream.skip(AlphaDataOffset);
	Segment segAlphaData = { stream.current(), stream.remindSize() };

	m_image = QImage::fromData(segImageData.ptr, segImageData.size);
	if (imgJPEG == checkImgType(segImageData.ptr))
	{
		if (!segAlphaData.size)
			return;

		uLongf UncompressedSize = m_image.width() * m_image.height();
		QByteArray dest(UncompressedSize, 0);
		int res = uncompress((uchar*)dest.data(), &UncompressedSize, segAlphaData.ptr, segAlphaData.size);
		if (Z_OK != res)
			return;

		uchar* pAlpha = (uchar*)dest.data();
		QImage alphaCh(m_image.width(), m_image.height(), QImage::Format_Alpha8);
		for (int y = 0; y < m_image.height(); ++y)
		{
			for (int x = 0; x < m_image.width(); ++x)
			{
				alphaCh.setPixelColor(x, y, QColor(0, 0, 0, *pAlpha));
				++pAlpha;
			}
		}
		m_image.setAlphaChannel(alphaCh);
	}
}

void SwfFillStyle::fromStream(LByteStream& stream, SwfTagType tag)
{
	stream.readAs(FillStyleType);
	if (0 == FillStyleType)
	{
		if (tag == tagDefineShape3)
		{
			stream.readAs(Color);
		}
		else
		{
			SwfRGB rgb = stream.read<SwfRGB>();
			Color.red = rgb.red;
			Color.green = rgb.green;
			Color.blue = rgb.blue;
			Color.alpha = 0xFF;
		}
	}
	else if (isBitmap(FillStyleType))
	{
		stream.readAs(bitmap.id);
		bitmap.matrix.fromStream(stream);
	}
	else if (isGradient(FillStyleType))
	{
		gradient.GradientMatrix.fromStream(stream);
		assert(false);
	}
	else
	{
		assert(false);
	}
}

void SwfFillStyleArray::fromStream(LByteStream& stream, SwfTagType tag)
{
	UI16 FillStyleCount = stream.read<UI8>();
	if (tag != tagDefineShape && FillStyleCount == 0xFF)
		FillStyleCount = stream.read<UI16>();

	FillStyles.resize(FillStyleCount);
	for (auto iter = FillStyles.begin(); iter != FillStyles.end(); ++iter)
	{
		iter->fromStream(stream, tag);
	}
}
