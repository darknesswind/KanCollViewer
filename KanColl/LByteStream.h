#pragma once
#ifdef min
#undef min
#endif
class QByteArray;
class LByteStream
{
	typedef unsigned char Byte;
	typedef const Byte* RawPtr;
public:
	LByteStream(RawPtr buf, size_t size);
	LByteStream(const QByteArray& buf);
	~LByteStream();

	inline void skip(size_t size);
	template <typename T> void skip();
	template <typename T> size_t readAs(T& var);
	template <typename T> size_t readAs(T* pBuf, size_t cnt);
	template <typename T> T read();
	template <typename T> size_t readTo(T& buf);
	template <typename T> size_t readTo(T& buf, size_t size);
	template <typename T> size_t getAs(T& var);
	template <typename T> size_t getAs(T* pBuf, size_t cnt);

	RawPtr current() const { return m_cur; }
	size_t size() const { return m_size; }
	size_t tell() const { return m_cur - m_buf; }
	size_t remindSize() const { return m_size - tell(); }
	bool eof() const { return m_cur >= m_end; }

private:
	RawPtr m_buf;
	RawPtr m_cur;
	RawPtr m_end;
	size_t m_size;
};

void LByteStream::skip(size_t size)
{
	m_cur += size;
	if (m_cur > m_end)
		m_cur = m_end;
}

template <typename T>
void LByteStream::skip()
{
	static_assert(std::is_pod<T>::value, "Must be POD Type");
	skip(sizeof(T));
}

template <typename T>
size_t LByteStream::readAs(T& var)
{
	static_assert(std::is_pod<T>::value, "Must be POD Type");
	size_t maxToRead = std::min((size_t)(m_end - m_cur), sizeof(T));
	memcpy(&var, m_cur, maxToRead);
	m_cur += maxToRead;
	return maxToRead;
}

template <typename T>
size_t LByteStream::readAs(T* pBuf, size_t cnt)
{
	static_assert(std::is_pod<T>::value, "Must be POD Type");
	size_t maxToRead = std::min((size_t)(m_end - m_cur), sizeof(T) * cnt);
	if (maxToRead > 0)
	{
		memcpy(pBuf, m_cur, maxToRead);
		m_cur += maxToRead;
	}
	return maxToRead;
}

template <typename T>
T LByteStream::read()
{
	T res{ 0 };
	readAs(res);
	return res;
}

template <typename T>
size_t LByteStream::readTo(T& buf)
{
	if (buf.empty())
		return 0;

	return readAs(&buf[0], buf.size());
}

template <typename T>
size_t LByteStream::readTo(T& buf, size_t size)
{
	buf.resize(size);
	return readTo(buf);
}

template <typename T>
size_t LByteStream::getAs(T& var)
{
	static_assert(std::is_pod<T>::value, "Must be POD Type");
	size_t maxToRead = std::min((size_t)(m_end - m_cur), sizeof(T));
	memcpy(&var, m_cur, maxToRead);
	return maxToRead;
}

template <typename T>
size_t LByteStream::getAs(T* pBuf, size_t cnt)
{
	static_assert(std::is_pod<T>::value, "Must be POD Type");
	size_t maxToRead = std::min((size_t)(m_end - m_cur), sizeof(T) * cnt);
	memcpy(pBuf, m_cur, maxToRead);
	return maxToRead;
}
