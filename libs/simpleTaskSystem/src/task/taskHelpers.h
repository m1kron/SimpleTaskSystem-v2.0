#pragma once
#include "task.h"

NAMESPACE_STS_BEGIN

//////////////////////////////////////////////////////////////////
// Helper function for creating task that will call functor( e.g. lambda )
// Example:
// FunctorTaskMaker( task_handle, []() { for( int i = 0; i < 1000000; ++i ) { int k = 0; } } );
// task_manager.SubmitTask( task_handle );
template< typename TFunctor >
void FunctorTaskMaker( TaskHandle& task_handle, const TFunctor& funtor );

////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// This function will extract functor and call it.
template< typename TFunctor >
void FunctorTaskFunction( const ITaskContext* context ) 
{
	// This helper class is used to retrive functor from task data, cuz
	// in general TFunctor might not have ctor that we can use here ( without paramters ).
	struct Temp
	{
		char data[ sizeof( TFunctor ) ];
	};

	Temp temp;

	ExistingBufferWrapperReader readBuffer( context.GetThisTask()->GetRawDataPtr(), context.GetThisTask()->GetDataSize() );
	readBuffer.Read( temp );

	// Cast to functor type.
	const TFunctor& functor = reinterpret_cast<const TFunctor&> ( temp );

	functor( context );
}

///////////////////////////////////////////////////////////
template< typename TFunctor >
void FunctorTaskMaker( TaskHandle& task_handle, const TFunctor& funtor )
{
	STATIC_ASSERT( sizeof( TFunctor ) <= Task::DATA_SIZE, "Unfortunately, functor is too big to be hold in task data segment." );

	task_handle->SetTaskFunction( &FunctorTaskFunction< TFunctor > );

	ExistingBufferWrapperWriter writeBuffer( task_handle->GetRawDataPtr(), task_handle->GetDataSize() );
	writeBuffer.Write( funtor );
}

NAMESPACE_STS_END