#pragma once
#include "..\iTaskManager.h"
#include "..\..\..\commonLib\include\tools\existingBufferWrapper.h" //< HACK, but I will fix this later.

namespace sts
{
namespace tools
{

//////////////////////////////////////////////////////////////////
// Helper function for creating task that will call lambda.
// Returns valid handle if success. Note, that lambda has to fit task storage ( sizeof (TLambda) <= task storage )
//
// Example:
// FunctorTaskMaker( []() { for( int i = 0; i < 1000000; ++i ) { int k = 0; } }, task_manager, nullptr );
// task_manager->SubmitTask( task_handle );
template< typename TLambda >
const ITaskHandle* LambdaTaskMaker( const TLambda& funtor, ITaskManager* task_handle, const ITaskHandle* parent_task_handle );

////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// This function will extract functor and call it.
template< typename TLambda >
void LambdaTaskFunction( const ITaskContext* context ) 
{
	// This helper class is used to retrive functor from task data, cuz
	// in general TFunctor might not have ctor that we can use here ( without paramters ).
	struct Temp
	{
		char data[ sizeof( TLambda ) ];
	};

	Temp temp;

	ExistingBufferWrapperReader readBuffer( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() );
	readBuffer.Read( temp );

	// Cast to functor type.
	const TLambda& lambda = reinterpret_cast<const TLambda&> ( temp );

	lambda( context );
}

///////////////////////////////////////////////////////////
template< typename TLambda >
const ITaskHandle* LambdaTaskMaker( const TLambda& funtor, ITaskManager* manager, const ITaskHandle* parent_task_handle )
{
	//Function ptr is nullptr, cuz I will set it manually.
	if( auto task_handle = manager->CreateNewTask( parent_task_handle ) )
	{
		if( sizeof( TLambda ) <= task_handle->GetTaskStorageSize() )
		{
			task_handle->SetTaskFunction( &LambdaTaskFunction< TLambda > );

			ExistingBufferWrapperWriter writeBuffer( task_handle->GetTaskStorage(), task_handle->GetTaskStorageSize() );
			writeBuffer.Write( funtor );

			return task_handle;
		}
		else
		{
			manager->ReleaseTask( task_handle );
			return nullptr;
		}
	}
	
	return nullptr;
}

}
}