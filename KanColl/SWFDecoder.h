#pragma once
#include "swfstruct.h"
class LByteStream;

class SWFDecoder
{
public:
	SWFDecoder();
	~SWFDecoder();

	bool test();
	bool open(const QString& fileName);
	QMap<int, QImage>& images() { return m_images; }

private:
	bool decode(QByteArray& buf);
	void readTag(LByteStream& stream);

	template <typename TagType>
	TagType* createTagObj(LByteStream& stream, const SwfTagHeader& header)
	{
		TagType* pTag = new TagType(header);
		pTag->fromStream(stream);
		m_tags.push_back(pTag);
		return pTag;
	}

private:
	SwfHeader m_header;
	SwfRect m_frameSize;
	SwfFrameInfo m_frameinfo;
	std::vector<TagObject*> m_tags;

	QMap<int, QImage> m_images;
	QImage m_jpgeTables;
};

