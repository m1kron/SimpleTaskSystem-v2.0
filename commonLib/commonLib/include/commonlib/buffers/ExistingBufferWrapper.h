#pragma once
#include <cstring>
#include <commonLib\Macros.h>

///////////////////////////////////////////////////////////
// Wrapper for existing buffers, allows to write and read from buffer, but cannot resize the buffer.
class ExistingBufferWrapper
{
public:
	ExistingBufferWrapper( void* existing_buffer, size_t existing_buffer_size );

	/// Write to buffer from from_buffer.
	// start_offset - indicates where to put data to existing_buffer( offset is relative to existing_buffer )
	// size - size of data to write
	// from_buffer - where to take data from
	void WriteToBuffer( const void* from_buffer, size_t start_offset, size_t size );

	/// Read from existing buffer to out_to_buffer.
	void ReadFromBuffer( void* out_to_buffer, size_t start_offset, size_t size );

	/// Get size of existing buffer.
	size_t GetSize() const;
private:
	void* m_existingBuffer;
	size_t m_existingBufferSize;
};

////////////////////////////////////////////////////////////
// Helper wrapper, base of reader and writer wrappers.
class ExistingBufferWrapperWriterReaderBase : public ExistingBufferWrapper
{
public:
	ExistingBufferWrapperWriterReaderBase( void* existing_buffer, size_t existing_buffer_size );

	// Returns size of currently stored data in existing buffer.
	size_t GetCurrentlyUsedBytes() const;

protected:
	size_t m_currentOffset;
};

////////////////////////////////////////////////////////////
// Wrapper, that allows only to write sequentially to buffer.
class ExistingBufferWrapperWriter : private ExistingBufferWrapperWriterReaderBase
{
public:
	ExistingBufferWrapperWriter( void* existing_buffer, size_t existing_buffer_size );

	// Writes data to buffer.
	template< typename T > void Write( const T& data );

	using ExistingBufferWrapperWriterReaderBase::GetCurrentlyUsedBytes;
	using ExistingBufferWrapperWriterReaderBase::GetSize;
};

////////////////////////////////////////////////////////////
// Wrapper, that allows only to read sequentially form buffer.
class ExistingBufferWrapperReader : private ExistingBufferWrapperWriterReaderBase
{
public:
	ExistingBufferWrapperReader( void* existing_buffer, size_t existing_buffer_size );

	// Read data from buffer.
	template< typename T > void Read( T& out_data );

	using ExistingBufferWrapperWriterReaderBase::GetCurrentlyUsedBytes;
	using ExistingBufferWrapperWriterReaderBase::GetSize;
};


//////////////////////////////////////////////////////////
//
// ExistingBufferWrapper INLINES:
//
//////////////////////////////////////////////////////////
inline ExistingBufferWrapper::ExistingBufferWrapper( void* existing_buffer, size_t existing_buffer_size )
	: m_existingBuffer( existing_buffer )
	, m_existingBufferSize( existing_buffer_size )
{}

//////////////////////////////////////////////////////////
inline void ExistingBufferWrapper::WriteToBuffer( const void* from_buffer, size_t start_offset, size_t size )
{
	ASSERT( start_offset + size <= m_existingBufferSize );
	memcpy( (char*)m_existingBuffer + start_offset, from_buffer, size );
}

//////////////////////////////////////////////////////////
inline void ExistingBufferWrapper::ReadFromBuffer( void* out_to_buffer, size_t start_offset, size_t size )
{
	ASSERT( start_offset + size <= m_existingBufferSize );
	memcpy( out_to_buffer, (const char*)m_existingBuffer + start_offset, size );
}

/////////////////////////////////////////////////////////
inline size_t ExistingBufferWrapper::GetSize() const
{
	return m_existingBufferSize;
}

//////////////////////////////////////////////////////////
//
// ExistingBufferWrapperWriterReaderBase INLINES:
//
//////////////////////////////////////////////////////////
inline ExistingBufferWrapperWriterReaderBase::ExistingBufferWrapperWriterReaderBase( void* existing_buffer, size_t existing_buffer_size )
	: ExistingBufferWrapper( existing_buffer, existing_buffer_size )
	, m_currentOffset( 0 )
{}

//////////////////////////////////////////////////////////
inline size_t ExistingBufferWrapperWriterReaderBase::GetCurrentlyUsedBytes() const
{
	return m_currentOffset;
}

//////////////////////////////////////////////////////////
//
// ExistingBufferWrapperWriter INLINES:
//
//////////////////////////////////////////////////////////
inline ExistingBufferWrapperWriter::ExistingBufferWrapperWriter( void* existing_buffer, size_t existing_buffer_size )
	: ExistingBufferWrapperWriterReaderBase( existing_buffer, existing_buffer_size )
{}

//////////////////////////////////////////////////////////
template < typename T >
inline void ExistingBufferWrapperWriter::Write( const T& data )
{
	size_t size = sizeof( T );
	WriteToBuffer( &data, m_currentOffset, size );
	m_currentOffset += size;
}

//////////////////////////////////////////////////////////
//
// ExistingBufferWrapperReader INLINES:
//
//////////////////////////////////////////////////////////
inline ExistingBufferWrapperReader::ExistingBufferWrapperReader( void* existing_buffer, size_t existing_buffer_size )
	: ExistingBufferWrapperWriterReaderBase( existing_buffer, existing_buffer_size )
{
}

//////////////////////////////////////////////////////////
template < typename T >
inline void ExistingBufferWrapperReader::Read( T& out_data )
{
	size_t size = sizeof( T );
	ReadFromBuffer( &out_data, m_currentOffset, size );
	m_currentOffset += size;
}