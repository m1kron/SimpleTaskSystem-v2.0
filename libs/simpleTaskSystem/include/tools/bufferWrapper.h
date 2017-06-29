#pragma once
#include <cstring>
#include "..\sts_types.h"

namespace sts
{
namespace tools
{

///////////////////////////////////////////////////////////
// WrapperBase for existing buffers.
class BufferWrapperBase
{
public:
	BufferWrapperBase( void* existing_buffer, uint32_t buffer_size );

	// Copies memory to buffer of given size. Returs true if success.
	// If success, increases current offset from begining.
	bool WriteToBuffer( const void* from_buffer, uint32_t size );

	// Copies memory from buffer to out_to_buffer. Returs true if success.
	// If success, increases current offset from begining.
	bool ReadFromBuffer( void* out_to_buffer, uint32_t size );

	// Get size of existing buffer.
	uint32_t GetSize() const;

	// Returns size of currently stored data in existing buffer.
	uint32_t GetCurrentOffset() const;

private:
	void* m_existingBuffer;
	uint32_t m_bufferSize;
	uint32_t m_currentOffset;
};

////////////////////////////////////////////////////////////
// Wrapper, that allows only to write sequentially to buffer.
class BufferWrapperWriter : private BufferWrapperBase
{
public:
	BufferWrapperWriter( void* existing_buffer, uint32_t existing_buffer_size );

	// Writes data to buffer. Returns true if success.
	template< typename T > bool WriteSafe( const T& data );

	using BufferWrapperBase::GetCurrentOffset;
	using BufferWrapperBase::GetSize;
};

////////////////////////////////////////////////////////////
// Wrapper, that allows only to read sequentially form buffer.
class BufferWrapperReader : private BufferWrapperBase
{
public:
	BufferWrapperReader( void* existing_buffer, uint32_t existing_buffer_size );

	// Read data from buffer. Returns true if success.
	template< typename T > bool ReadSafe( T& out_data );

	using BufferWrapperBase::GetCurrentOffset;
	using BufferWrapperBase::GetSize;
};

//////////////////////////////////////////////////////////
//
// BufferWrapper INLINES:
//
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
inline BufferWrapperBase::BufferWrapperBase( void* existing_buffer, uint32_t buffer_size )
	: m_existingBuffer( existing_buffer )
	, m_bufferSize( buffer_size )
	, m_currentOffset( 0 )
{}

//////////////////////////////////////////////////////////
inline bool BufferWrapperBase::WriteToBuffer( const void* from_buffer, uint32_t size )
{
	if( m_currentOffset + size <= m_bufferSize )
	{
		memcpy( ( char* )m_existingBuffer + m_currentOffset, from_buffer, size );
		m_currentOffset += size;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////
inline bool BufferWrapperBase::ReadFromBuffer( void* out_to_buffer, uint32_t size )
{
	if( m_currentOffset + size <= m_bufferSize )
	{
		memcpy( out_to_buffer, ( const char* )m_existingBuffer + m_currentOffset, size );
		m_currentOffset += size;
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////
inline uint32_t BufferWrapperBase::GetSize() const
{
	return m_bufferSize;
}

//////////////////////////////////////////////////////////
inline uint32_t BufferWrapperBase::GetCurrentOffset() const
{
	return m_currentOffset;
}

//////////////////////////////////////////////////////////
//
// BufferWrapperWriter INLINES:
//
//////////////////////////////////////////////////////////
inline BufferWrapperWriter::BufferWrapperWriter( void* existing_buffer, uint32_t existing_buffer_size )
	: BufferWrapperBase( existing_buffer, existing_buffer_size )
{}

//////////////////////////////////////////////////////////
template < typename T >
inline bool BufferWrapperWriter::WriteSafe( const T& data )
{
	return WriteToBuffer( &data, sizeof( T ) );
}

//////////////////////////////////////////////////////////
//
// BufferWrapperReader INLINES:
//
//////////////////////////////////////////////////////////
inline BufferWrapperReader::BufferWrapperReader( void* existing_buffer, uint32_t existing_buffer_size )
	: BufferWrapperBase( existing_buffer, existing_buffer_size )
{
}

//////////////////////////////////////////////////////////
template < typename T >
inline bool BufferWrapperReader::ReadSafe( T& out_data )
{
	return ReadFromBuffer( &out_data, sizeof( T ) );
}

}
}