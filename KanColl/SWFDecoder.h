#pragma once
#include "swfstruct.h"
class LByteStream;

class SWFDecoder
{
public:
	enum Mode
	{
		modeFull,
		modeShape,
		modeImage,
	};
	SWFDecoder(Mode mode);
	~SWFDecoder();

	bool test();
	bool open(const QString& fileName);
	const QString& error() { return m_errStr; };
	QMap<int, ImageShape>& images() { return m_images; }
	std::unordered_set<int>& imageFilter() { return m_imageFilter; }

private:
	bool decode(QByteArray& buf);
	void readTag(LByteStream& stream);

	template <typename TagType>
	TagType* createTag(LByteStream& stream, const SwfTagHeader& header)
	{
		TagType* pTag = new TagType(header);
		pTag->fromStream(stream);
		m_tags.push_back(pTag);
		return pTag;
	}
	template <typename TagType>
	TagType* createImageTag(LByteStream& stream, const SwfTagHeader& header)
	{
		TagType* pTag = createTag<TagType>(stream, header);
		if (header.length)
		{
			ImageShape& shape = m_images[pTag->character()];
			shape.img = pTag->image();
			if (shape.rect.isEmpty())
				shape.rect.setSize(QSizeF(shape.img.width(), shape.img.height()));
		}
		return pTag;
	}

	bool NeedPaser(SwfTagType type);
	bool IsFilterOut(SwfTagType type, UI16 iCharacter);

private:
	Mode m_mode;
	std::unordered_set<int> m_imageFilter;

	SwfHeader m_header;
	SwfRect m_frameSize;
	SwfFrameInfo m_frameinfo;
	std::vector<TagObject*> m_tags;

	QMap<int, ImageShape> m_images;
	QImage m_jpgeTables;

	QString m_errStr;
};

