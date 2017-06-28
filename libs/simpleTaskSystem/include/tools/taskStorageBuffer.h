#pragma once
#include "bufferWrapper.h"
#include "..\iTaskHandle.h"
#include "..\iTaskContext.h"

namespace sts
{
namespace tools
{
////////////////////////////////////////////////////////////////////
// Task storage writer.
class TaskStorageWriter : private BufferWrapperWriter
{
public:
	TaskStorageWriter( const ITaskContext* context );
	TaskStorageWriter( const ITaskHandle* handle );

	// Chained write operation. You won't get notified if data size is bigger then buffer size!
	// In order to get notification use WriteSafe() method.
	template < typename T > TaskStorageWriter& WriteChain( const T& data );

	using BufferWrapperWriter::WriteSafe;
};

////////////////////////////////////////////////////////////////////
// Task storage reader.
class TaskStorageReader : public BufferWrapperReader
{
public:
	TaskStorageReader( const ITaskContext* context );
	TaskStorageReader( const ITaskHandle* handle );

	// Chained read operation. You won't get notified if data size is bigger then buffer size!
	// In order to get notification use ReadSafe() method.
	template < typename T > TaskStorageReader& ReadChain( T& out_data );

	// Reads objects from buffer. Unsfae if sizeof( T ) is bigger then current buffer size.
	template < typename T > T Read();

	using BufferWrapperReader::ReadSafe;
};

////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
inline TaskStorageWriter::TaskStorageWriter( const ITaskContext* context )
	: BufferWrapperWriter( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() )
{
}

////////////////////////////////////////////////////////////////////
inline TaskStorageWriter::TaskStorageWriter( const ITaskHandle* handle )
	: BufferWrapperWriter( handle->GetTaskStorage(), handle->GetTaskStorageSize() )
{
}

////////////////////////////////////////////////////////////////////
template< typename T >
inline TaskStorageWriter& TaskStorageWriter::WriteChain( const T& data )
{
	BufferWrapperWriter::WriteSafe( data );
	return *this;
}

////////////////////////////////////////////////////////////////////
inline TaskStorageReader::TaskStorageReader( const ITaskContext* context )
	: BufferWrapperReader( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() )
{
}

////////////////////////////////////////////////////////////////////
inline TaskStorageReader::TaskStorageReader( const ITaskHandle* handle )
	: BufferWrapperReader( handle->GetTaskStorage(), handle->GetTaskStorageSize() )
{
}

////////////////////////////////////////////////////////////////////
template< typename T >
inline TaskStorageReader& TaskStorageReader::ReadChain( T& out_data )
{
	BufferWrapperReader::ReadSafe( out_data );
	return *this;
}

////////////////////////////////////////////////////////////////////
template<typename T>
inline T TaskStorageReader::Read()
{
	T data;
	BufferWrapperReader::ReadSafe( data );
	return data;
}

}
}