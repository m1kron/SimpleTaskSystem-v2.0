#pragma once
#include <cstring>
#include "..\macros.h"

///////////////////////////////////////////////////////////
// WrapperBase for existing buffers.
class BufferWrapperBase
{
public:
	BufferWrapperBase( void* existing_buffer, size_t buffer_size );

	// Copies memory to buffer of given size. Returs true if success.
	// If success, increases current offset from begining.
	bool WriteToBuffer( const void* from_buffer, size_t size );

	// Copies memory from buffer to out_to_buffer. Returs true if success.
	// If success, increases current offset from begining.
	bool ReadFromBuffer( void* out_to_buffer, size_t size );

	// Get size of existing buffer.
	size_t GetSize() const;

	// Returns size of currently stored data in existing buffer.
	size_t GetCurrentOffset() const;

private:
	void* m_existingBuffer;
	size_t m_bufferSize;
	size_t m_currentOffset;
};

////////////////////////////////////////////////////////////
// Wrapper, that allows only to write sequentially to buffer.
class BufferWrapperWriter : private BufferWrapperBase
{
public:
	BufferWrapperWriter( void* existing_buffer, size_t existing_buffer_size );

	// Writes data to buffer. Returns true if success.
	template< typename T > bool Write( const T& data );

	using BufferWrapperBase::GetCurrentOffset;
	using BufferWrapperBase::GetSize;
};

////////////////////////////////////////////////////////////
// Wrapper, that allows only to read sequentially form buffer.
class BufferWrapperReader : private BufferWrapperBase
{
public:
	BufferWrapperReader( void* existing_buffer, size_t existing_buffer_size );

	// Read data from buffer. Returns true if success.
	template< typename T > bool Read( T& out_data );

	using BufferWrapperBase::GetCurrentOffset;
	using BufferWrapperBase::GetSize;
};

//////////////////////////////////////////////////////////
//
// BufferWrapper INLINES:
//
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
inline BufferWrapperBase::BufferWrapperBase( void* existing_buffer, size_t buffer_size )
	: m_existingBuffer( existing_buffer )
	, m_bufferSize( buffer_size )
	, m_currentOffset( 0 )
{}

//////////////////////////////////////////////////////////
inline bool BufferWrapperBase::WriteToBuffer( const void* from_buffer, size_t size )
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
inline bool BufferWrapperBase::ReadFromBuffer( void* out_to_buffer, size_t size )
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
inline size_t BufferWrapperBase::GetSize() const
{
	return m_bufferSize;
}

//////////////////////////////////////////////////////////
inline size_t BufferWrapperBase::GetCurrentOffset() const
{
	return m_currentOffset;
}

//////////////////////////////////////////////////////////
//
// BufferWrapperWriter INLINES:
//
//////////////////////////////////////////////////////////
inline BufferWrapperWriter::BufferWrapperWriter( void* existing_buffer, size_t existing_buffer_size )
	: BufferWrapperBase( existing_buffer, existing_buffer_size )
{}

//////////////////////////////////////////////////////////
template < typename T >
inline bool BufferWrapperWriter::Write( const T& data )
{
	return WriteToBuffer( &data, sizeof( T ) );
}

//////////////////////////////////////////////////////////
//
// BufferWrapperReader INLINES:
//
//////////////////////////////////////////////////////////
inline BufferWrapperReader::BufferWrapperReader( void* existing_buffer, size_t existing_buffer_size )
	: BufferWrapperBase( existing_buffer, existing_buffer_size )
{
}

//////////////////////////////////////////////////////////
template < typename T >
inline bool BufferWrapperReader::Read( T& out_data )
{
	return ReadFromBuffer( &out_data, sizeof( T ) );
}