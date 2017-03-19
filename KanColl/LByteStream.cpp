#include "stdafx.h"
#include "LByteStream.h"


LByteStream::LByteStream(RawPtr buf, size_t size)
	: m_buf(buf), m_size(size), m_cur(buf), m_end(buf + size)
{
}


LByteStream::LByteStream(const QByteArray& buf)
	: LByteStream((RawPtr)buf.data(), buf.size())
{
}

LByteStream::~LByteStream()
{
}
