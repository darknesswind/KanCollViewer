#pragma once
class LBitsField
{
	typedef unsigned char byte;
	static const byte bitsPerByte = 8;
public:
	LBitsField(const byte* begin, const byte* end);

	char getSbyte(size_t offset, size_t size) { return getValue<char>(offset, size); }
	uchar getUbyte(size_t offset, size_t size) { return getValue<uchar>(offset, size); }
	int getSint(size_t offset, size_t size) { return getValue<int>(offset, size); }
	uint getUint(size_t offset, size_t size) { return getValue<uint>(offset, size); }

private:
	template <typename TReturn>
	TReturn getValue(size_t offset, size_t size);
	static const uchar s_mask[8];

private:
	std::vector<byte> m_bytes;
};

template <typename TReturn>
TReturn LBitsField::getValue(size_t offset, size_t size)
{
	const size_t maxbits = sizeof(TReturn) * bitsPerByte;
	if (size > maxbits)
		size = maxbits;
	if ((offset + size) / bitsPerByte + 1 > m_bytes.size())
		return 0;

	size_t iByte = offset / bitsPerByte;
	offset -= iByte * bitsPerByte;
	byte* pBegin = m_bytes.data() + iByte;

	union _Convert
	{
		TReturn val;
		byte bytes[sizeof(TReturn)];
	} result;

	for (int i = sizeof(TReturn) - 1; i >= 0; --i)
	{
		result.bytes[i] = *pBegin;
		++pBegin;
	}

	if (offset > 0)
	{
		result.val = result.val << offset;
		result.val |= ((*pBegin) & s_mask[offset]) >> (bitsPerByte - offset);
	}
	result.val = result.val >> (maxbits - size);

	return result.val;
}
