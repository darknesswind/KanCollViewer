#pragma once
#include <cassert>
#ifndef assert
#define assert(exp)
#endif
#include <QFile>

class LFile : public QFile
{
public:
	using QFile::QFile;

	template <typename T> qint64 readAs(T& var);
	template <typename T> qint64 readAs(T* pBuf, size_t cnt);
	template <typename T> T read();

// 	template <typename T> size_t write(T& var);
// 	template <typename T> size_t write(T* pBuf, size_t cnt);
};

template <typename T>
qint64 LFile::readAs(T& var)
{
	auto sz = QFile::read((char*)&var, sizeof(T));
	assert(sz == sizeof(T));
	return sz;
}

template <typename T>
qint64 LFile::readAs(T* pBuf, size_t cnt)
{
	auto sz = QFile::read((char*)pBuf, sizeof(T) * cnt);
	assert(sz == sizeof(T) * cnt);
	return sz;
}

template <typename T>
T LFile::read()
{
	T res = 0;
	readAs(res);
	return res;
}
/*

template <typename T>
size_t LFile::write(T& var)
{
	size_t sz = fwrite((void*)&var, sizeof(T), 1, m_hFile);
	assert(sizeof(T) == sz);
	return sz;
}

template <typename T>
size_t LFile::write(T* pBuf, size_t cnt)
{
	size_t sz = fwrite(pBuf, sizeof(T), cnt, m_hFile);
	assert(sizeof(T) * cnt == sz);
	return sz;
}
*/
