#include "stdafx.h"
#include "SWFDecoder.h"
#include "LFile.h"
#include "QtZlib/zlib.h"
#include "LByteStream.h"
#include "LzmaLib.h"

SWFDecoder::SWFDecoder(Mode mode)
	: m_mode(mode)
{
	test();
}


SWFDecoder::~SWFDecoder()
{
	for each (TagObject* pTag in m_tags)
		delete pTag;
}

bool SWFDecoder::test()
{
// 	return open(R"(D:\Game\ShimakazeGo\cache\kcs\resources\swf\ships\aejfywpsegbv.swf)");
//  	return open(R"(D:\Game\ShimakazeGo\cache\kcs\PortMain.swf)");

	return true;
}

bool SWFDecoder::open(const QString& fileName)
{
	LFile file(fileName);
	file.open(QFile::ReadOnly);
	if (file.size() < sizeof(SwfHeader))
		return false;

	file.readAs(m_header);

	SwfCompressedType type = (SwfCompressedType)m_header.magic[0];
	if (m_header.magic[1] != 'W' || m_header.magic[2] != 'S')
		return false;

	uLongf UncompressedSize = file.read<UI32>();
	QByteArray buf;
	switch (type)
	{
	case swfUncompressed:
		buf = file.readAll();
		break;
	case swfZLib:
	{
		buf = file.readAll();
		QByteArray dest(UncompressedSize, 0);
		int res = uncompress((uchar*)dest.data(), &UncompressedSize, (uchar*)buf.data(), buf.size());
		bool bSucc = (Z_OK == res);
		if (bSucc)
			buf = dest;
		else
			return false;
		break;
	}
	case swfLZMA:
	{
		size_t srcLen = file.read<UI32>();
		uchar prop[LZMA_PROPS_SIZE] = { 0 };
		file.readAs(prop);

		buf = file.readAll();
		size_t dstSize = UncompressedSize;
		QByteArray dest(UncompressedSize, 0);
		int res = LzmaUncompress((uchar*)dest.data(), &dstSize, (uchar*)buf.data(), &srcLen, prop, LZMA_PROPS_SIZE);
		bool bSucc = (SZ_OK == res);
		if (bSucc)
			buf = dest;
		else
			return false;
		break;
	}
	default:
		return false;
	}

	return decode(buf);
}

bool SWFDecoder::decode(QByteArray& buf)
{
	LByteStream stream(buf);
	m_frameSize.fromStream(stream);
	stream.readAs(m_frameinfo);
	while (!stream.eof())
		readTag(stream);

#ifdef _DEBUG
// 	for (auto iter = m_images.begin(); iter != m_images.end(); ++iter)
// 	{
// 		if (!iter->img.isNull())
// 			iter->img.save(QString(".\\test\\world\\%1.png").arg(iter.key()));
// 	}
// 	if (!m_jpgeTables.isNull())
// 		m_jpgeTables.save("test\\world\\JPEGTables.jpg");
#endif // _DEBUG
	return true;
}

void SWFDecoder::readTag(LByteStream& stream)
{
	SwfTagHeader header;
	header.fromStream(stream);
	if (!NeedPaser(header.TagType) ||
		IsFilterOut(header.TagType, *(UI16*)stream.current()))
	{
		stream.skip(header.length);
		return;
	}

	size_t endpos = stream.tell() + header.length;
	switch (header.TagType)
	{
	case tagDoAction:
	case tagDoInitAction:
	case tagDoABC:

	/************************
	*  Display List case tags   *
	************************/
	case tagShowFrame:
	case tagPlaceObject:
	case tagRemoveObject:
		createTag<TagUnknown>(stream, header);
		break;
	case tagPlaceObject2:
		createTag<TagPlaceObject2>(stream, header);
		break;
	case tagRemoveObject2:
	case tagPlaceObject3:
		createTag<TagUnknown>(stream, header);
		break;

	// Chapter 4: Control Tags
	case tagSetBackgroundColor:
		createTag<TagSetBackgroundColor>(stream, header);
		break;
	case tagFrameLabel:
	case tagProtect:
	case tagEnd:
	case tagExportAssets:
	case tagImportAssets:
	case tagEnableDebugger:
	case tagEnableDebugger2:
	case tagScriptLimits:
	case tagSetTabIndex:
		createTag<TagUnknown>(stream, header);
		break;
	case tagFileAttributes:
		createTag<TagFileAttributes>(stream, header);
		break;
	case tagImportAsset2:
	case tagSymbolClass:
		createTag<TagUnknown>(stream, header);
		break;
	case tagMetadata:
		createTag<TagMetadata>(stream, header);
		break;
	case tagDefineScalingGrid:
	case tagDefineSceneAndFrameLabelData:
		createTag<TagUnknown>(stream, header);
		break;

	/************************
	*     Shape case tags   *
	************************/
	case tagDefineShape:
	case tagDefineShape2:
	case tagDefineShape3:
	{
		TagDefineShape* pTag = createTag<TagDefineShape>(stream, header);
		auto& fills = pTag->fills().FillStyles;
		for (auto iter = fills.begin(); iter != fills.end(); ++iter)
		{
			if (iter->isBitmap() && !IsFilterOut(tagDefineBits, iter->bitmap.id) &&
				m_images.find(iter->bitmap.id) != m_images.end())
				m_images[iter->bitmap.id].rect = pTag->bounds().toRect();
		}
		break;
	}
	case tagDefineShape4:
	{
		TagDefineShape4* pTag = createTag<TagDefineShape4>(stream, header);
		auto& fills = pTag->fills().FillStyles;
		for (auto iter = fills.begin(); iter != fills.end(); ++iter)
		{
			if (iter->isBitmap() && !IsFilterOut(tagDefineBits, iter->bitmap.id) &&
				m_images.find(iter->bitmap.id) != m_images.end())
				m_images[iter->bitmap.id].rect = pTag->bounds().toRect();
		}
		break;
	}

	// Chapter 8: Bitmaps
	case tagDefineBits:
		createImageTag<TagDefineBits>(stream, header);
		break;
	case tagJPEGTables:
	{
		TagJPEGTables* pTag = createTag<TagJPEGTables>(stream, header);
		if (header.length)
			m_jpgeTables = pTag->image();
		break;
	}
	case tagDefineBitsJPEG2:
		createImageTag<TagDefineBitsJPEG2>(stream, header);
		break;
	case tagDefineBitsJPEG3:
		createImageTag<TagDefineBitsJPEG3>(stream, header);
		break;
	case tagDefineBitsLossless:
		createImageTag<TagDefineBitsLossless>(stream, header);
		break;
	case tagDefineBitsLossless2:
		createImageTag<TagDefineBitsLossless2>(stream, header);
		break;
	case tagDefineBitsJPEG4:
		createImageTag<TagDefineBitsJPEG4>(stream, header);
		break;

	/************************
	* Shape Morphing case tags  *
	************************/
	case tagDefineMorphShape:
	case tagDefineMorphShape2:

	/************************
	*       Font case tags  *
	************************/
	case tagDefineFont:
	case tagDefineText:
	case tagDefineFontInfo:
	case tagDefineText2:
	case tagDefineEditText:
	case tagDefineFont2:
	case tagDefineFontInfo2:
	case tagDefineFontAlignZones:
	case tagCSMTextSettings:
	case tagDefineFont3:
	case tagDefineFontName:

	/************************
	*      Sound case tags  *
	************************/
	case tagDefineSound:
	case tagStartSound:
	case tagSoundStreamHead:
	case tagSoundStreamBlock:
	case tagSoundStreamHead2:
	case tagStartSound2:

	/************************
	*      Button case tags *
	************************/
	case tagDefineButton:
	case tagDefineButtonSound:
	case tagDefineButtonCxform:
	case tagDefineButton2:

	/************************
	*     Sprite case tags  *
	************************/
	case tagDefineSprite:

	/************************
	*      Video case tags  *
	************************/
	case tagDefineVideoSteam:
	case tagVideoFrame:

	// Chapter 15: Metadata
	case tagEnableTelemetry:
	case tagDefineBinaryData:

	/************************
	*  Undocumented case tags   *
	************************/
	case tagSerialNumber:
	case tagMX4:
	case tagAmayetaEncrypt:
	case tagUNK:
	case tagSWFFile:
	default:
		createTag<TagUnknown>(stream, header);
		break;
	}
	assert(endpos == stream.tell());
}

bool SWFDecoder::NeedPaser(SwfTagType type)
{
	if (m_mode == modeFull)
		return true;

	switch (type)
	{
	case tagDefineShape:
	case tagDefineShape2:
	case tagDefineShape3:
	case tagDefineShape4:
		return (m_mode == modeShape);
	case tagDefineBits:
	case tagJPEGTables:
	case tagDefineBitsJPEG2:
	case tagDefineBitsJPEG3:
	case tagDefineBitsLossless:
	case tagDefineBitsLossless2:
	case tagDefineBitsJPEG4:
		return (m_mode == modeShape || m_mode == modeImage);
	default:
		return false;
	}
	return false;
}

bool SWFDecoder::IsFilterOut(SwfTagType type, UI16 imageID)
{
	if (m_imageFilter.empty())
		return false;

	switch (type)
	{
	case tagDefineBits:
	case tagDefineBitsJPEG2:
	case tagDefineBitsJPEG3:
	case tagDefineBitsLossless:
	case tagDefineBitsLossless2:
	case tagDefineBitsJPEG4:
		return (m_imageFilter.find(imageID) == m_imageFilter.end());
	default:
		return false;
	}

	return false;
}
