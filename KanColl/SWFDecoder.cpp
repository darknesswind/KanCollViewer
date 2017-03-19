#include "stdafx.h"
#include "SWFDecoder.h"
#include "LFile.h"
#include "QtZlib/zlib.h"
#include "LByteStream.h"

SWFDecoder::SWFDecoder()
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
	return open(R"(D:\Game\ShimakazeGo\cache\kcs\resources\swf\ships\aejfywpsegbv.swf)");
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

#define CreateImageTag(Type)\
{\
	Type* pTag = createTagObj<Type>(stream, header);\
	if (header.length)\
	{\
		m_images[pTag->character()] = pTag->image();\
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
		createTagObj<TagUnknown>(stream, header);
		break;
	case tagPlaceObject2:
		createTagObj<TagPlaceObject2>(stream, header);
		break;
	case tagRemoveObject2:
	case tagPlaceObject3:
		createTagObj<TagUnknown>(stream, header);
		break;

	// Chapter 4: Control Tags
	case tagSetBackgroundColor:
		createTagObj<TagSetBackgroundColor>(stream, header);
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
		createTagObj<TagUnknown>(stream, header);
		break;
	case tagFileAttributes:
		createTagObj<TagFileAttributes>(stream, header);
		break;
	case tagImportAsset2:
	case tagSymbolClass:
		createTagObj<TagUnknown>(stream, header);
		break;
	case tagMetadata:
		createTagObj<TagMetadata>(stream, header);
		break;
	case tagDefineScalingGrid:
	case tagDefineSceneAndFrameLabelData:
		createTagObj<TagUnknown>(stream, header);
		break;

	/************************
	*     Shape case tags   *
	************************/
	case tagDefineShape:
	case tagDefineShape2:
	case tagDefineShape3:
		createTagObj<TagDefineShape>(stream, header);
		break;
	case tagDefineShape4:
		createTagObj<TagDefineShape4>(stream, header);
		break;

	// Chapter 8: Bitmaps
	case tagDefineBits:
		CreateImageTag(TagDefineBits);
		break;
	case tagJPEGTables:
	{
		TagJPEGTables* pTag = createTagObj<TagJPEGTables>(stream, header);
		if (header.length)
			m_jpgeTables = pTag->image();
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
		createTagObj<TagUnknown>(stream, header);
		break;
	}
	assert(endpos == stream.tell());
}
