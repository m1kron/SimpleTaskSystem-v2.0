#include <gtest\gtest.h>
#include <array>
#include "..\..\libs\simpleTaskSystem\include\globalApi.h"
#include "..\..\libs\simpleTaskSystem\include\tools\taskBatch.h"
#include "..\..\libs\simpleTaskSystem\include\tools\lambdaTask.h"
#include "..\..\libs\commonLib\include\tools\existingBufferWrapper.h"

namespace helpers
{
	//////////////////////////////////////////////////////////////////////
	template< class TIter >
	static void CalculateBigSum( TIter& it ) // WARNING! This method has to take it and fill it, otherwise, it will be optimaized by compiler in release!!!
	{
		static const int IterationNumberBig = 1000000000;
		static const int IterationNumberSmall = 100000000;

		int sum = 0;
		for( int i = 0; i < IterationNumberSmall; ++i )
		{
			sum += ( i % 4 ) / 2;
		}

		*it = sum;
	}

	//////////////////////////////////////////////////////////////////////
	template < class TIter >
	static bool ChcekIfEqual( const TIter& begin, const TIter& end )
	{
		auto val = *begin;
		for( auto it = begin; it != end; ++it )
			if( *it != val )
				return false;
		return true;
	}

	//////////////////////////////////////////////////////////////////////
	void TaskFunction( const sts::ITaskContext* context )
	{
		int sum = 0;
		for( int i = 0; i < 10000000; ++i )
		{
			sum += ( i % 4 ) / 2;
		}

		ExistingBufferWrapperWriter writeBuffer( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() );
		writeBuffer.Write( sum );
	}

	//////////////////////////////////////////////////////////////////////
	void TaskFunctionFast( const sts::ITaskContext* context )
	{
		int sum = 0;
		for( int i = 0; i < 100000; ++i )
		{
			sum += ( i % 4 ) / 2;
		}

		ExistingBufferWrapperWriter writeBuffer( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() );
		writeBuffer.Write( sum );
	}

	//////////////////////////////////////////////////////////////////////
	int CalculateItem( int item )
	{
		int sum = item;
		for( int i = 0; i < 100000; ++i )
		{
			sum += ( i % 4 ) / 2;
		}

		return sum;
	}

	//////////////////////////////////////////////////////////////////////
	void CalcualteItemAndWriteToArray( const sts::ITaskContext* context )
	{
		std::array<int, 200>* array_ptr = nullptr;
		int my_index = -1;

		// Read needed parameters.
		ExistingBufferWrapperReader reader( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() );
		reader.Read( array_ptr );
		reader.Read( my_index );

		// Calculate item:
		int new_item = CalculateItem( ( *array_ptr )[ my_index ] );

		// and write it back to array:
		( *array_ptr )[ my_index ] = new_item;
	}

	//////////////////////////////////////////////////////////////////////
	void ArraySummer( const sts::ITaskContext* context )
	{
		std::array<int, 200>* array_ptr = nullptr;

		// Read the array.
		ExistingBufferWrapperReader reader( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() );
		reader.Read( array_ptr );

		// Calculate sum.
		int sum = 0;
		for( size_t i = 0; i < array_ptr->size(); ++i )
		{
			sum += ( *array_ptr )[ i ];
		}

		// Write result.
		ExistingBufferWrapperWriter writeBuffer( context->GetThisTaskStorage(), context->GetThisTaskStorageSize() );
		writeBuffer.Write( sum );
	}

	////////////////////////////////////////////////////////////////////////////
	template< typename T >
	T ReadFromTask( const sts::ITaskHandle* handle )
	{
		ExistingBufferWrapperReader reader( handle->GetTaskStorage(), handle->GetTaskStorageSize() );
		T val;
		reader.Read( val );
		return val;
	}
}

//////////////////////////////////////////////////////////
TEST( STSTest, SimpleSingleTask )
{
	sts::ITaskManager* manager = CreateTaskSystem();
	const sts::ITaskHandle* task_handle = manager->CreateNewTask( &helpers::TaskFunction, nullptr );

	ASSERT_TRUE( manager->SubmitTask( task_handle ) );
	ASSERT_TRUE( manager->RunTasksUsingThisThreadUntil( [ &task_handle ]() { return task_handle->IsFinished(); } ) );

	ASSERT_TRUE( helpers::ReadFromTask<int>( task_handle ) > 0 );

	manager->ReleaseTask( task_handle );
	DestroyTaskSystem( manager );
}

//////////////////////////////////////////////////////////
TEST( STSTest, SimpleSingleLambdaTask )
{
	sts::ITaskManager* manager = CreateTaskSystem();
	auto task_handle = sts::tools::LambdaTaskMaker( []( const sts::ITaskContext* )
	{
		int sum = 0;
		for( int i = 0; i < 10000; ++i )
			sum += i;
	}, manager, nullptr );

	ASSERT_TRUE( manager->SubmitTask( task_handle ) );
	ASSERT_TRUE( manager->RunTasksUsingThisThreadUntil( [ &task_handle ]() { return task_handle->IsFinished(); } ) );

	manager->ReleaseTask( task_handle );
	DestroyTaskSystem( manager );
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, SimpleChainTask )
{
	sts::ITaskManager* manager = CreateTaskSystem();
	auto root_task_handle = manager->CreateNewTask( &helpers::TaskFunction, nullptr );
	auto child_task_handle = manager->CreateNewTask( &helpers::TaskFunction, root_task_handle );

	ASSERT_TRUE( manager->SubmitTask( root_task_handle ) );
	ASSERT_TRUE( manager->SubmitTask( child_task_handle ) );
	ASSERT_TRUE( manager->RunTasksUsingThisThreadUntil( [ &root_task_handle ]() { return root_task_handle->IsFinished(); }  ) );

	ASSERT_TRUE( helpers::ReadFromTask<int>( root_task_handle ) > 0 );
	ASSERT_TRUE( helpers::ReadFromTask<int>( child_task_handle ) > 0 );

	manager->ReleaseTask( root_task_handle );
	manager->ReleaseTask( child_task_handle );
	DestroyTaskSystem( manager );
}

TEST( STSTest, SimpleFlatTree )
{
	sts::ITaskManager* manager = CreateTaskSystem();
	{
		sts::tools::TaskBatch_AutoRelease batch( manager );

		for( int i = 0; i < 1000; ++i )
		{
			auto taskHandle = manager->CreateNewTask( helpers::TaskFunctionFast, nullptr );
			ASSERT_TRUE( taskHandle != nullptr );
			batch.Add( taskHandle );
		}

		ASSERT_TRUE( batch.SubmitAll() );

		manager->RunTasksUsingThisThreadUntil( [ &batch ]() { return batch.AreAllTaskFinished(); } );

		for( auto task_handle : batch )
			ASSERT_TRUE( helpers::ReadFromTask<int>( task_handle ) > 0 );
	}
	DestroyTaskSystem( manager );
}

////////////////////////////////////////////////////////////////////////
//TEST( STSTest, DynamicTaskTreeTest1 )
//{
//	sts::TaskManager manager;
//	manager.Setup();
//
//	double numOfTries = 0;
//	double accumulator = 0;
//
//	for( int iteration = 0; iteration < 100; ++iteration )
//	{
//		// This is arrray that we will work on.
//		std::array< int, 200 > arrayToFill = { 0 };
//
//		// Create lambda that will process the array in parallel.
//		auto array_functor = [ &arrayToFill ]( sts::TaskContext& context )
//		{
//			sts::TaskBatch_AutoRelease batch( context.GetTaskManager() );
//
//			for( unsigned i = 0; i < arrayToFill.size(); ++i )
//			{
//				// Create a task that will calculate single item.
//				int item = arrayToFill[ i ];
//				auto item_functor = [ item ]( sts::TaskContext& context )
//				{
//					int calculated_item = helpers::CalculateItem( item );
//
//					// Store item in data task data storage.
//					ExistingBufferWrapperWriter writer( context.GetThisTask()->GetRawDataPtr(), context.GetThisTask()->GetDataSize() );
//					writer.Write( calculated_item );
//
//				}; ///< end of item_functor
//
//				// Create new task using item_functor:
//				sts::TaskHandle handle = context.GetTaskManager().CreateNewTask( item_functor );
//				batch.Add( std::move( handle ) );
//			}
//
//			// Submit whole batch.
//			bool submitted = context.GetTaskManager().SubmitTaskBatch( batch );
//			ASSERT_TRUE( submitted );
//
//			// Wait until whole batch is done.
//			context.WaitFor( [ &batch ] { return batch.AreAllTaskFinished(); } );
//
//			// Get results from child tasks and calculate final sum:
//			int final_sum = 0;
//			for( unsigned i = 0; i < batch.GetSize(); ++i )
//			{
//				const sts::TaskHandle& handle = batch[ i ];
//				int sum = 0;
//				ExistingBufferWrapperReader read_buffer( handle->GetRawDataPtr(), handle->GetDataSize() );
//				read_buffer.Read( sum );
//
//				// Fill array with apropriate results:
//				arrayToFill[ i ] = sum;
//				final_sum += sum;
//			}
//
//			// Write final sum:
//			ExistingBufferWrapperWriter writer( context.GetThisTask()->GetRawDataPtr(), context.GetThisTask()->GetDataSize() );
//			writer.Write( final_sum );
//
//		}; ///< end of array_functor
//
//		// Create main task using array_functor:
//		sts::TaskHandle root_task_handle = manager.CreateNewTask( array_functor );
//
//		// Submit main task..
//		bool submitted = manager.SubmitTask( root_task_handle );
//		ASSERT_TRUE( submitted );
//
//		// and help processing until main task is done:
//		manager.RunTasksUsingThisThreadUntil( [ &root_task_handle ] { return root_task_handle->IsFinished(); } );
//
//		// Read the result:
//		int sum = 0;
//		ExistingBufferWrapperReader read_buffer( root_task_handle->GetRawDataPtr(), root_task_handle->GetDataSize() );
//		read_buffer.Read( sum );
//
//		ASSERT_TRUE( sum == 10000000 );
//
//		// Release main task:
//		manager.ReleaseTask( root_task_handle );
//
//		ASSERT_TRUE( manager.AreAllTasksReleased() );
//	}
//}
//
////////////////////////////////////////////////////////////////////////
//TEST(STSTest, DynamicTaskTreeTest2)
//{
//	sts::TaskManager manager;
//	manager.Setup();
//	
//	double numOfTries = 0;
//	double accumulator = 0;
//	
//	for( int iteration = 0; iteration < 10; ++iteration )
//	{
//		auto functor = []( sts::TaskContext& context )
//		{
//			sts::TaskBatch_AutoRelease batch( context.GetTaskManager() );
//	
//			sts::TaskHandle parent_task_handle = context.GetTaskManager().CreateNewTask( &helpers::TaskFunctionFast );
//			ASSERT_TRUE( parent_task_handle != sts::INVALID_TASK_HANDLE );
//	
//			for( unsigned i = 0; i < 100; ++i )
//			{
//				sts::TaskHandle handle = context.GetTaskManager().CreateNewTask( &helpers::TaskFunctionFast, parent_task_handle );
//				ASSERT_TRUE( handle != sts::INVALID_TASK_HANDLE );
//				batch.Add( std::move( handle ) );
//			}
//	
//			batch.Add( std::move( parent_task_handle ) );
//	
//			bool submitted = context.GetTaskManager().SubmitTaskBatch( batch );
//			ASSERT_TRUE( submitted );
//	
//			context.WaitFor( [ &batch ] { return batch.AreAllTaskFinished(); } );
//	
//			int final_sum = 0;
//			for( const sts::TaskHandle& handle : batch )
//			{
//				int sum = 0;
//				ExistingBufferWrapperReader read_buffer( handle->GetRawDataPtr(), handle->GetDataSize() );
//				read_buffer.Read( sum );
//	
//				ASSERT_TRUE( sum == 50000 );
//				ASSERT_TRUE( handle->IsFinished() );
//	
//				final_sum += sum;
//			}	
//	
//			ExistingBufferWrapperWriter writer( context.GetThisTask()->GetRawDataPtr(), context.GetThisTask()->GetDataSize() );
//			writer.Write( final_sum );
//		};
//		////////////////////////////////////////////////////////////////////
//	
//		{
//			sts::TaskBatch_AutoRelease batch( manager );
//	
//			for( unsigned i = 0; i < 20; ++i )
//			{
//				sts::TaskHandle& handle = manager.CreateNewTask( functor );
//				ASSERT_TRUE( handle != sts::INVALID_TASK_HANDLE );
//				batch.Add( std::move( handle ) );
//			}
//	
//			bool submitted = manager.SubmitTaskBatch( batch );
//			ASSERT_TRUE( submitted );
//	
//			manager.RunTasksUsingThisThreadUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );
//	
//			for( const sts::TaskHandle& handle : batch )
//			{
//				int sum = 0;
//				ExistingBufferWrapperReader read_buffer( handle->GetRawDataPtr(), handle->GetDataSize() );
//				read_buffer.Read( sum );
//	
//				ASSERT_TRUE( sum == 5050000 );
//				ASSERT_TRUE( handle->IsFinished() );
//			}
//		}
//	
//		ASSERT_TRUE( manager.AreAllTasksReleased() );
//	}
//
//}
//
////////////////////////////////////////////////////////////////////////
//TEST(STSTest, StaticTaskTreeTest)
//{
//	sts::TaskManager manager;
//	manager.Setup();
//	
//	for( int iteration = 0; iteration < 50; ++iteration )
//	{
//		{
//			sts::TaskHandle root_task_handle = manager.CreateNewTask( &helpers::TaskFunctionFast );
//			ASSERT_TRUE( root_task_handle != sts::INVALID_TASK_HANDLE );
//	
//			sts::TaskBatch_AutoRelease batch( manager );
//	
//			// Build static tree:
//			for( unsigned i = 0; i < 20; ++i )
//			{
//				sts::TaskHandle parent_handle_lvl2 = manager.CreateNewTask( &helpers::TaskFunctionFast, root_task_handle );
//				ASSERT_TRUE( parent_handle_lvl2 != sts::INVALID_TASK_HANDLE );
//	
//				for( unsigned i = 0; i < 100; ++i )
//				{
//					sts::TaskHandle handle = manager.CreateNewTask( &helpers::TaskFunctionFast, parent_handle_lvl2 );
//					ASSERT_TRUE( handle != sts::INVALID_TASK_HANDLE );
//					batch.Add( std::move( handle ) );
//				}
//	
//				batch.Add( std::move( parent_handle_lvl2 ) );
//			}
//	
//			batch.Add( std::move( root_task_handle ) );
//	
//			bool submitted = manager.SubmitTaskBatch( batch );
//			ASSERT_TRUE( submitted );
//	
//			manager.RunTasksUsingThisThreadUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );
//	
//			for( const sts::TaskHandle& handle : batch )
//			{
//				int sum = 0;
//				ExistingBufferWrapperReader read_buffer( handle->GetRawDataPtr(), handle->GetDataSize() );
//				read_buffer.Read( sum );
//	
//				ASSERT_TRUE( sum == 50000 );
//				ASSERT_TRUE( handle->IsFinished() );
//			}
//		}
//		ASSERT_TRUE( manager.AreAllTasksReleased() );
//	}
//}


//#pragma region SYNTETIC_TEST_OF_TASK_STEALING
//		//  for( int iteration = 0; iteration < 1000; ++iteration )
//		//  {
//		//PREPARE_PROFILING;
//		//BEGIN_PROFILE_SECTION;
//
//		//sts::TaskWorkersPool workersPool;
//		//sts::TaskAllocator allocator;
//
//		//workersPool.InitializePool( sts::tools::GetLogicalCoresCount() );
//
//		//sts::TaskHandle task_handle = allocator.AllocateNewTask();
//		//task_handle->SetTaskFunction( &TaskFunction );
//		//std::vector<sts::TaskHandle> tasks;
//
//		//for( unsigned i = 0; i < 20; ++i )
//		//{
//		//	tasks.push_back( allocator.AllocateNewTask() );
//		//	tasks[ i ]->SetTaskFunction( &TaskFunction );
//		//	tasks[ i ]->AddParent( task_handle );
//	//}
////for( unsigned i = 0; i < 20; ++i )
////{
//
//		//	workersPool.GetTaskAt( 0 )->AddTask( tasks[ i ].operator->( ) );
//
//		//}
//
//		//// wake all threads:
//		//unsigned workers_count = workersPool.GetPoolSize();
//		//for( unsigned worker_id = 0; worker_id < workers_count; ++worker_id )
//		//{
//		//	workersPool.GetTaskAt( worker_id )->WakeUp();
//		//}
//
//		//while( !task_handle->IsFinished() )
//		//{
//		//	sts::this_thread::YieldThread();
//		//}
//
//		//int sum = 0;
//		//ExistingBufferWrapperReader read_buffer( task_handle->GetRawDataPtr(), task_handle->GetDataSize() );
//		//read_buffer.Read( sum );
//		//ASSERT( sum == 5000000 );
//
//		//for( unsigned i = 0; i < tasks.size(); ++i )
//		//{
//		//	int sum = 0;
//		//	ExistingBufferWrapperReader read_buffer( tasks[ i ]->GetRawDataPtr(), tasks[ i ]->GetDataSize() );
//		//	read_buffer.Read( sum );
//
//		//	ASSERT( sum == 5000000 );
//		//	ASSERT( tasks[ i ]->IsFinished() );
//		//	allocator.ReleaseTask( tasks[ i ] );
//		//}
//
//		//allocator.ReleaseTask( task_handle );
//		//ASSERT( task_handle == sts::INVALID_TASK_HANDLE );
//
//		//// Finish workers:
//		//// Singnal all worker that they should finish right now.
//		//for( unsigned worker_id = 0; worker_id < workers_count; ++worker_id )
//		//{
//		//	workersPool.GetTaskAt( worker_id )->FinishWork();
//		//}
//
//		//// We have to wait for threads to finish their work.
//		//unsigned worker_id = 0;
//		//while( worker_id < workers_count )
//		//{
//		//	// Check if thread has finish it's work.
//		//	if( workersPool.GetTaskAt( worker_id )->HasFinishedWork() )
//		//	{
//		//		// Go to next thread.
//		//		++worker_id;
//		//	}
//		//	else
//		//	{
//		//		// Yield exection to give worker threads processor time.
//		//		sts::this_thread::YieldThread();
//		//	}
//		//}
//
//		//workersPool.ReleasePool();
//
//		//END_PROFILE_SECTION("Section done in time ");
//
//		//std::cout << "Stealing tasks test " << iteration << " succeeded" << std::endl;
//  //  }
//#pragma endregion SYNTETIC_TEST_OF_TASK_STEALING