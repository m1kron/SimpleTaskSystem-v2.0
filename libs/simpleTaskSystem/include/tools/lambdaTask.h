#pragma once
#include "..\iTaskSystem.h"
#include "taskStorageBuffer.h"

namespace sts
{
namespace tools
{

//////////////////////////////////////////////////////////////////
// Helper function for creating task that will call lambda.
// Returns valid handle if success. Note, that lambda has to fit task storage ( sizeof (TLambda) <= task storage )
//
// Example:
// LambdaTaskMaker( []() { for( int i = 0; i < 1000000; ++i ) { int k = 0; } }, task_system_interface );
// task_system_interface->SubmitTask( task_handle );
template< typename TLambda >
const ITaskHandle* LambdaTaskMaker( const TLambda& functor, ITaskSystem* system_interface );

////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// This function will extract functor and call it.
template< typename TLambda >
bool LambdaTaskFunction( const ITaskContext* context ) 
{
	// This helper class is used to retrive functor from task data, cuz
	// in general TFunctor might not have ctor that we can use here ( without paramters ).
	struct Temp
	{
		char data[ sizeof( TLambda ) ];
	};

	Temp temp = TaskStorageReader( context ).Read< Temp >();

	// Cast to functor type.
	const TLambda& lambda = reinterpret_cast<const TLambda&> ( temp );

	return lambda( context );
}

///////////////////////////////////////////////////////////
template< typename TLambda >
const ITaskHandle* LambdaTaskMaker( const TLambda& functor, ITaskSystem* system_interface )
{
	//Function ptr is nullptr, cuz I will set it manually.
	if( auto task_handle = system_interface->CreateNewTask() )
	{
		if( sizeof( TLambda ) <= task_handle->GetTaskStorageSize() )
		{
			task_handle->SetTaskFunction( &LambdaTaskFunction< TLambda > );
			TaskStorageWriter( task_handle ).WriteSafe( functor );
			return task_handle;
		}
		else
		{
			system_interface->ReleaseTask( task_handle );
			return nullptr;
		}
	}
	
	return nullptr;
}

}
}