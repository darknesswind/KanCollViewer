#include "stdafx.h"
#include "SWFDecoder.h"
#include "LFile.h"
#include "QtZlib/zlib.h"
#include "LByteStream.h"

SWFDecoder::SWFDecoder()
{
}


SWFDecoder::~SWFDecoder()
{
	for each (SwfTagHeader* pTag in m_tags)
		free(pTag);
}

bool SWFDecoder::test()
{
	// return open(R"(D:\Game\ShimakazeGo\cache\kcs\resources\swf\ships\aejfywpsegbv.swf)");
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
	QByteArray buf = file.readAll();
	switch (type)
	{
	case swfUncompressed:
		break;
	case swfZLib:
	{
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
		return false;
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
// 		if (!iter->isNull())
// 			iter->save(QString(".\\test\\%1.png").arg(iter.key()));
// 	}
// 	if (!m_jpgeTables.isNull())
// 		m_jpgeTables.save("test\\JPEGTables.jpg");
#endif // _DEBUG
	return true;
}

#define DefaultTagProc()\
{\
	SwfRawData* pTag = createTag<SwfRawData>(header, header.length);\
	stream.readAs(pTag->buf, header.length);\
	break;\
}
#define CreateImageTag(Type)\
{\
	Type* pTag = createTag<Type>(stream, header, header.length);\
	if (header.length)\
	{\
		m_images[pTag->CharacterID] = pTag->image(header.length);\
	}\
}

void SWFDecoder::readTag(LByteStream& stream)
{
	SwfTagHeader header;
	header.fromStream(stream);
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
		DefaultTagProc();
		break;
	case tagPlaceObject2:
	{
		TagPlaceObject2* pTag = createTag<TagPlaceObject2>(header, header.length);
		stream.readAs((char*)pTag, header.length);
		break;
	}
	case tagRemoveObject2:
	case tagPlaceObject3:
		DefaultTagProc();
		break;

	// Chapter 4: Control Tags
	case tagSetBackgroundColor:
	{
		SwfRGB* pTag = createTag<SwfRGB>(header);
		stream.readAs(*pTag);
		break;
	}
	case tagFrameLabel:
	case tagProtect:
	case tagEnd:
	case tagExportAssets:
	case tagImportAssets:
	case tagEnableDebugger:
	case tagEnableDebugger2:
	case tagScriptLimits:
	case tagSetTabIndex:
		DefaultTagProc();
		break;
	case tagFileAttributes:
	{
		TagFileAttributes* pTag = createTag<TagFileAttributes>(header);
		stream.readAs(*pTag);
		break;
	}
	case tagImportAsset2:
	case tagSymbolClass:
		DefaultTagProc();
		break;
	case tagMetadata:
	{
		SwfString* pTag = createTag<SwfString>(header, header.length);
		stream.readAs(pTag->str, header.length);
		break;
	}
	case tagDefineScalingGrid:
	case tagDefineSceneAndFrameLabelData:
		DefaultTagProc();
		break;

	/************************
	*     Shape case tags   *
	************************/
	case tagDefineShape:
	case tagDefineShape2:
	case tagDefineShape3:
	{
		TagDefineShape* pTag = createTag<TagDefineShape>(header, header.length + sizeof(TagDefineShape));
		pTag->fromStream(stream, header.length);
		break;
	}
	case tagDefineShape4:
	{
		TagDefineShape4* pTag = createTag<TagDefineShape4>(header, header.length + sizeof(TagDefineShape4));
		pTag->fromStream(stream, header.length);
		break;
	}

	// Chapter 8: Bitmaps
	case tagDefineBits:
		CreateImageTag(TagDefineBits);
		break;
	case tagJPEGTables:
	{
		TagJPEGTables* pTag = createTag<TagJPEGTables>(stream, header, header.length);
		if (header.length)
			m_jpgeTables = pTag->image(header.length);
		break;
	}
	case tagDefineBitsJPEG2:
		CreateImageTag(TagDefineBitsJPEG2);
		break;
	case tagDefineBitsJPEG3:
		CreateImageTag(TagDefineBitsJPEG3);
		break;
	case tagDefineBitsLossless:
		CreateImageTag(TagDefineBitsLossless);
		break;
	case tagDefineBitsLossless2:
		CreateImageTag(TagDefineBitsLossless2);
		break;
	case tagDefineBitsJPEG4:
		CreateImageTag(TagDefineBitsJPEG4);
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
		DefaultTagProc();
		break;
	}
	assert(endpos == stream.tell());
}
