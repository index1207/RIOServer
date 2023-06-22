#pragma once

class RecvBuffer
{
public:
	RecvBuffer(byte* buffer, size_t bufferSize);

	auto inline GetDataSize() -> size_t
	{
		return _writeIdx - _readIdx;
	}
	auto inline GetFreeSize() -> size_t
	{
		return _size - _writeIdx;
	}

	auto inline ReadByteArray()-> std::shared_ptr<byte[]>
	{
		auto readBuffer = std::make_shared<byte[] >(GetDataSize());
		std::copy(_buffer, _buffer + _readIdx, readBuffer.get());
		return readBuffer;
	}
	auto inline WriteByteArray() -> byte*
	{

	}

	auto Clear() -> void;
private:
	byte* _buffer;
	size_t _size;
	size_t _readIdx;
	size_t _writeIdx;
};

