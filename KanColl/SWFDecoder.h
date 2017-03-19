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
	TagType* createTag(const SwfTagHeader& header, size_t size)
	{
		SwfTag* pTag = (SwfTag*)malloc(sizeof(header) + size);
		m_tags.push_back(pTag);
		*(SwfTagHeader*)pTag = header;
		return (TagType*)((char*)pTag + sizeof(header));
	}
	template <typename TagType>
	TagType* createTag(const SwfTagHeader& header)
	{
		return createTag<TagType>(header, sizeof(TagType));
	}
	template <typename TagType>
	TagType* createTag(LByteStream& stream, const SwfTagHeader& header, size_t size)
	{
		TagType* pTag = createTag<TagType>(header, size);
		stream.readAs((uchar*)pTag, size);
		return pTag;
	}

private:
	SwfHeader m_header;
	SwfRect m_frameSize;
	SwfFrameInfo m_frameinfo;
	std::vector<SwfTag*> m_tags;

	QMap<int, QImage> m_images;
	QImage m_jpgeTables;
};

