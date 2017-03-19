#pragma once
class LByteStream;

enum SwfCompressedType
{
	swfUncompressed	= 'F',
	swfZLib			= 'C',
	swfLZMA			= 'Z',
};

enum SwfTagType : ushort
{
	/************************
	*    DoAction Tags *
	************************/
	tagDoAction = 12,
	tagDoInitAction = 59,
	tagDoABC = 82,

	/************************
	*  Display List Tags   *
	************************/
	tagShowFrame = 1,
	tagPlaceObject = 4,
	tagRemoveObject = 5,
	tagPlaceObject2 = 26,
	tagRemoveObject2 = 28,
	tagPlaceObject3 = 70,

	// Chapter 4: Control Tags
	tagSetBackgroundColor = 9,
	tagFrameLabel = 43,
	tagProtect = 24,
	tagEnd = 0,
	tagExportAssets = 56,
	tagImportAssets = 57,
	tagEnableDebugger = 58,
	tagEnableDebugger2 = 64,
	tagScriptLimits = 65,
	tagSetTabIndex = 66,
	tagFileAttributes = 69,
	tagImportAsset2 = 71,
	tagSymbolClass = 76,
	tagMetadata = 77,
	tagDefineScalingGrid = 78,
	tagDefineSceneAndFrameLabelData = 86,

	/************************
	*     Shape Tags   *
	************************/
	tagDefineShape = 2,
	tagDefineShape2 = 22,
	tagDefineShape3 = 32,
	tagDefineShape4 = 83,

	/************************
	*     Bitmap Tags  *
	************************/
	tagDefineBits = 6,
	tagJPEGTables = 8,
	tagDefineBitsJPEG2 = 21,
	tagDefineBitsJPEG3 = 35,
	tagDefineBitsLossless = 20,
	tagDefineBitsLossless2 = 36,
	tagDefineBitsJPEG4 = 90,

	/************************
	* Shape Morphing Tags  *
	************************/
	tagDefineMorphShape = 46,
	tagDefineMorphShape2 = 84,

	/************************
	*       Font Tags  *
	************************/
	tagDefineFont = 10,
	tagDefineText = 11,
	tagDefineFontInfo = 13,
	tagDefineText2 = 33,
	tagDefineEditText = 37,
	tagDefineFont2 = 48,
	tagDefineFontInfo2 = 62,
	tagDefineFontAlignZones = 73,
	tagCSMTextSettings = 74,
	tagDefineFont3 = 75,
	tagDefineFontName = 88,

	/************************
	*      Sound Tags  *
	************************/
	tagDefineSound = 14,
	tagStartSound = 15,
	tagSoundStreamHead = 18,
	tagSoundStreamBlock = 19,
	tagSoundStreamHead2 = 45,
	tagStartSound2 = 89,

	/************************
	*      Button Tags *
	************************/
	tagDefineButton = 7,
	tagDefineButtonSound = 17,
	tagDefineButtonCxform = 23,
	tagDefineButton2 = 34,

	/************************
	*     Sprite Tags  *
	************************/
	tagDefineSprite = 39,

	/************************
	*      Video Tags  *
	************************/
	tagDefineVideoSteam = 60,
	tagVideoFrame = 61,

	// Chapter 15: Metadata
	tagEnableTelemetry = 93,
	tagDefineBinaryData = 87,

	/************************
	*  Undocumented Tags   *
	************************/
	tagSerialNumber = 41,
	tagMX4 = 63,
	tagAmayetaEncrypt = 253,
	tagUNK = 0x3FF,
	tagSWFFile = 0xFFD0,
};

enum SwfImgType
{
	imgUnknown,
	imgJPEG,
	imgPNG,
	imgGIF89a
};

SwfImgType checkImgType(const uchar* pData);

typedef unsigned char UI8;
typedef unsigned short UI16;
typedef unsigned int UI32;

template <typename T, UI8 bw = sizeof(T) * 8 / 2>
struct _FIXED_NUM
{
	T val;
};
typedef _FIXED_NUM<UI32> FixedNum;
typedef _FIXED_NUM<UI16> FixedNum8;

#pragma pack(push)
#pragma pack(1)
struct SwfHeader
{
	UI8 magic[3];
	UI8 version;
};
struct SwfFrameInfo
{
	FixedNum8 FrameRate;
	UI16 FrameCount;
};
struct SwfString
{
	char str[1];
};
struct SwfRawData
{
	char buf[1];
};
struct SwfRGB
{
	UI8 red;
	UI8 green;
	UI8 blue;
};
struct SwfARGB
{
	UI8 alpha;
	UI8 red;
	UI8 green;
	UI8 blue;
};
struct SwfRGBA
{
	UI8 red;
	UI8 green;
	UI8 blue;
	UI8 alpha;
};

struct SwfRect
{
	// 5bits -> 0x1F = 31bits
	int xmin;
	int xmax;
	int ymin;
	int ymax;
	
	void fromStream(LByteStream& stream);
};

struct SwfMATRIX
{
	bool HasScale;
	bool HasRotate;
	FixedNum ScaleX;
	FixedNum ScaleY;
	FixedNum RotateSkew0;
	FixedNum RotateSkew1;
	int TranslateX;
	int TranslateY;

	void fromStream(LByteStream& stream);
};

struct SwfCXFORM
{
	// 4bit -> 0xF = 15bits
	bool HasAddTerms;
	bool HasMultTerms;
	int16_t RedMultTerm;
	int16_t GreenMultTerm;
	int16_t BlueMultTerm;
	int16_t RedAddTerm;
	int16_t GreenAddTerm;
	int16_t BlueAddTerm;

	void fromStream(LByteStream& stream);
};

struct SwfTagHeaderBase
{
	UI16 TagLength : 6;
	SwfTagType TagType : 10;
};

struct SwfTagHeader : public SwfTagHeaderBase
{
	UI32 length;

	void fromStream(LByteStream& stream);
};
struct SwfTag : public SwfTagHeader
{
	byte tagData[1];
};

struct TagFileAttributes
{
	UI32 Reserved2		: 24;
	UI32 UseNetwork		: 1;
	UI32 Reserved1		: 2;
	UI32 ActionScript3	: 1;
	UI32 HasMetadata	: 1;
	UI32 UseGPU			: 1;
	UI32 UseDirectBlit	: 1;
	UI32 Reserved0		: 1;
};

struct EncondedU32
{
	UI32 value;

	void fromStream(LByteStream& stream);
};

struct TagDefineSceneAndFrameLabelData
{
	UI32 SceneCount;
};

struct TagDefineBits
{
	UI16 CharacterID;
	UI8 JPEGData[1];

	size_t JPEGDataSize(size_t tagSize) const { return tagSize - sizeof(CharacterID); }
	QImage image(size_t tagSize) const;
};

struct TagJPEGTables
{
	UI8 JPEGData[1];

	QImage image(size_t tagSize) const;
};

struct TagDefineBitsJPEG2
{
	UI16 CharacterID;
	UI8 ImageData[1];

	size_t ImageDataSize(size_t tagSize) const { return tagSize - sizeof(CharacterID); }
	QImage image(size_t tagSize) const;
	SwfImgType type() const { return checkImgType(ImageData); }
};

struct TagDefineBitsJPEG3
{
	UI16 CharacterID;
	UI32 AlphaDataOffset;
	UI8 ImageData[1];

	UI8* BitmapAlphaData() const { return (UI8*)ImageData + AlphaDataOffset; }
	size_t AlphaDataSize(size_t tagSize) const { return tagSize - AlphaDataOffset - sizeof(CharacterID) - sizeof(AlphaDataOffset); }
	QImage image(size_t tagSize) const;
	SwfImgType type() const { return checkImgType(ImageData); }
};

struct TagDefineBitsLossless
{
	UI16 CharacterID;
	UI8 BitmapFormat;
	UI16 BitmapWidth;
	UI16 BitmapHeight;
	union
	{
		struct COLORMAPDATA
		{
			UI8 BitmapColorTableSize;
			UI8 ColorMappedData[1];
		} mappedDat;
		struct BITMAPDATA
		{
			UI8 BitmapPixelData[1];
		} bitmapDat;
	};

	enum Format
	{
		ftColorMapped8 = 3,
		ftRGB15,
		ftRGB24
	};

	QImage image(size_t tagSize) const;
	size_t BitmapDataSize(size_t tagSize) const { return tagSize - sizeof(UI16) * 3 - sizeof(UI8); }
	size_t MappedDataSize(size_t tagSize) const { return BitmapDataSize(tagSize) - sizeof(UI8); }
};

struct TagDefineBitsLossless2 : public TagDefineBitsLossless
{
	enum Format
	{
		ftRGB32 = 5
	};
	QImage image(size_t tagSize) const;

	QImage imageMapped8(size_t tagSize) const;
	QImage imageRGB32(size_t tagSize) const;
};

struct TagDefineBitsJPEG4
{
	UI16 CharacterID;
	UI32 AlphaDataOffset;
	UI16 DeblockParam;
	UI8 ImageData[1];

	UI8* BitmapAlphaData() const { return (UI8*)ImageData + AlphaDataOffset; }
	size_t AlphaDataSize(size_t tagSize) const { return tagSize - AlphaDataOffset - sizeof(CharacterID) - sizeof(AlphaDataOffset); }
	QImage image(size_t tagSize) const;
};

struct TagDefineShapeBase
{
	UI16 ShapeId;
	SwfRect ShapeBounds;
};

struct TagDefineShape : public TagDefineShapeBase
{
	size_t datSize;
	UI8 Shapes[1];

	void fromStream(LByteStream& stream, size_t tagSize);
};

struct TagDefineShape4 : public TagDefineShapeBase
{
	SwfRect EdgeBounds;
	struct 
	{
		UI8 UsesScalingStrokes		: 1;
		UI8 UsesNonScalingStrokes	: 1;
		UI8 UsesFillWindingRule		: 1;
		UI8 Reserved				: 5;
	} bits;
	size_t datSize;
	UI8 Shapes[1];

	void fromStream(LByteStream& stream, size_t tagSize);
};

struct TagPlaceObject
{
	UI16 CharacterId;
	UI16 Depth;
	SwfMATRIX Matrix;
};

struct TagPlaceObject2Base
{
	struct
	{
		UI8 PlaceFlagMove : 1;
		UI8 PlaceFlagHasCharacter : 1;
		UI8 PlaceFlagHasMatrix : 1;
		UI8 PlaceFlagHasColorTransform : 1;
		UI8 PlaceFlagHasRatio : 1;
		UI8 PlaceFlagHasName : 1;
		UI8 PlaceFlagHasClipDepth : 1;
		UI8 PlaceFlagHasClipActions : 1;
	} bits;
	UI16 Depth;
};

struct TagPlaceObject2 : public TagPlaceObject2Base
{
	UI8 OptionData[1];
};

#pragma pack(pop)
