#include <gtest\gtest.h>
#include <array>
#include "..\..\libs\simpleTaskSystem\include\globalApi.h"
#include "..\..\libs\simpleTaskSystem\include\tools\taskBatch.h"
#include "..\..\libs\simpleTaskSystem\include\tools\lambdaTask.h"
#include "..\..\libs\commonLib\include\timer\timerMacros.h"
#include "..\..\libs\basicThreadingLib\include\atomic\atomic.h"
#include "..\..\libs\basicThreadingLib\include\thread\thisThreadHelpers.h"
#include "..\..\libs\commonLib\include\tools\print.h"

namespace helpers
{
	static const int SOME_CONST = 0xdeedbeef;

	//////////////////////////////////////////////////////////////////////
	class StaticTaskSystem
	{
	public:
		StaticTaskSystem() { m_system = CreateTaskSystem(); }
		~StaticTaskSystem() { DestroyTaskSystem( m_system ); }

		sts::ITaskSystem* GetTaskSystem() { return m_system;  }

		static sts::ITaskSystem* GetStaticTaskSystem()
		{
			static StaticTaskSystem system;
			return system.GetTaskSystem();
		}

	private:
		sts::ITaskSystem* m_system;
	};

	//////////////////////////////////////////////////////////////////////
	template< class TIter >
	static void CalculateBigSum( TIter& it ) // WARNING! This method has to take it and fill it, otherwise, it will be optimaized by compiler in release!!!
	{
		static const int IterationNumberBig = 1000000000;
		static const int IterationNumberSmall = 100000000;

		int sum = 0;
		for( int i = 0; i < IterationNumberSmall; ++i )
			sum += ( i % 5 ) / 2;

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
	bool TaskFunction( const sts::ITaskContext* context )
	{
		int sum = 0;
		for( int i = 0; i < 10000000; ++i )
			sum += ( i % 5 ) / 2;

		sts::tools::TaskStorageWriter( context ).WriteSafe( sum );
		return true;
	}

	//////////////////////////////////////////////////////////////////////
	bool TaskFunctionFast( const sts::ITaskContext* context )
	{
		int sum = 0;
		for( int i = 0; i < 100000; ++i )
			sum += ( i % 5 ) / 2;

		sts::tools::TaskStorageWriter( context ).WriteSafe( sum );
		return true;
	}

	//////////////////////////////////////////////////////////////////////
	template< int CHILDS_SIZE >
	bool TaskFunctionDynamicTree( const sts::ITaskContext* context )
	{
		sts::tools::TaskBatch< CHILDS_SIZE > batch( context->GetTaskSystem() );

		for( int i = 0; i < CHILDS_SIZE; ++i )
		{
			auto task_handle = context->GetTaskSystem()->CreateNewTask( helpers::TaskFunctionFast, nullptr );
			helpers::WriteToTask< int >( task_handle, 0 );
			batch.Add( task_handle );
		}

		batch.SubmitAll();

		context->GetTaskSystem()->WaitUntil( [ &batch ]() { return batch.AreAllTaskFinished(); } );

		bool ok = true;
		for( auto task_handle : batch )
		{
			if( helpers::ReadFromTask<int>( task_handle ) == 0 )
			{
				ok = false;
				break;
			}
		}

		if( ok )
			sts::tools::TaskStorageWriter( context ).WriteSafe( SOME_CONST );

		return true;
	}

	//////////////////////////////////////////////////////////////////////
	int CalculateItem( int item )
	{
		int sum = item;
		for( int i = 0; i < 100000; ++i )
			sum += ( i % 4 ) / 2;

		return sum;
	}

	////////////////////////////////////////////////////////////////////////////
	template< typename T >
	T ReadFromTask( const sts::ITaskHandle* handle )
	{
		return sts::tools::TaskStorageReader( handle ).Read< T >();
	}

	////////////////////////////////////////////////////////////////////////////
	template< typename T >
	void WriteToTask( const sts::ITaskHandle* handle, const T& val )
	{
		sts::tools::TaskStorageWriter( handle ).WriteSafe( val );
	}
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, SimpleSingleTask )
{
	sts::ITaskSystem* system = helpers::StaticTaskSystem::GetStaticTaskSystem();
	const sts::ITaskHandle* task_handle = system->CreateNewTask( &helpers::TaskFunction, nullptr );

	ASSERT_TRUE( system->SubmitTask( task_handle ) );
	ASSERT_TRUE( system->WaitUntil( [ &task_handle ]() { return task_handle->IsFinished(); } ) );
	ASSERT_TRUE( helpers::ReadFromTask<int>( task_handle ) > 0 );

	system->ReleaseTask( task_handle );
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, SimpleSingleLambdaTask )
{
	sts::ITaskSystem* system = helpers::StaticTaskSystem::GetStaticTaskSystem();
	auto task_handle = sts::tools::LambdaTaskMaker( []( const sts::ITaskContext* )
	{
		int sum = 0;
		for( int i = 0; i < 10000; ++i )
			sum += i;
		return true;
	}, system );

	ASSERT_TRUE( system->SubmitTask( task_handle ) );
	ASSERT_TRUE( system->WaitUntil( [ &task_handle ]() { return task_handle->IsFinished(); } ) );

	system->ReleaseTask( task_handle );
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, SimpleChainTask )
{
	sts::ITaskSystem* system = helpers::StaticTaskSystem::GetStaticTaskSystem();
	auto root_task_handle = system->CreateNewTask( &helpers::TaskFunction, nullptr );
	auto child_task_handle = system->CreateNewTask( &helpers::TaskFunction, root_task_handle );

	ASSERT_TRUE( system->SubmitTask( root_task_handle ) );
	ASSERT_TRUE( system->SubmitTask( child_task_handle ) );
	ASSERT_TRUE( system->WaitUntil( [ &root_task_handle ]() { return root_task_handle->IsFinished(); }  ) );

	ASSERT_TRUE( helpers::ReadFromTask<int>( root_task_handle ) > 0 );
	ASSERT_TRUE( helpers::ReadFromTask<int>( child_task_handle ) > 0 );

	system->ReleaseTask( root_task_handle );
	system->ReleaseTask( child_task_handle );
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, SimpleFlatTree )
{
	sts::ITaskSystem* system_interface = helpers::StaticTaskSystem::GetStaticTaskSystem();
	{
		sts::tools::TaskBatch< 1000 > batch( system_interface );

		for( int i = 0; i < 1000; ++i )
		{
			auto task_handle = system_interface->CreateNewTask( helpers::TaskFunctionFast, nullptr );
			ASSERT_TRUE( task_handle != nullptr );
			helpers::WriteToTask( task_handle, 0 );
			ASSERT_TRUE( batch.Add( task_handle ) );
		}

		ASSERT_TRUE( batch.SubmitAll() );

		system_interface->WaitUntil( [ &batch ]() { return batch.AreAllTaskFinished(); } );

		for( auto task_handle : batch )
			ASSERT_TRUE( helpers::ReadFromTask<int>( task_handle ) > 0 );
	}
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, SimpleDynamicTree )
{
	sts::ITaskSystem* system_interface = helpers::StaticTaskSystem::GetStaticTaskSystem();
	{
		sts::tools::TaskBatch< 1 > batch( system_interface );

		auto task_handle = system_interface->CreateNewTask( helpers::TaskFunctionDynamicTree< 2 >, nullptr );
		batch.Add( task_handle );
		helpers::WriteToTask( task_handle, 0 );

		ASSERT_TRUE( batch.SubmitAll() );
		system_interface->WaitUntil( [ &batch ]() { return batch.AreAllTaskFinished(); } );

		for( auto task_handle : batch )
			ASSERT_TRUE( helpers::ReadFromTask<int>( task_handle ) == helpers::SOME_CONST );
	}
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, 2lvlDynamicTree )
{
	sts::ITaskSystem* system_interface = helpers::StaticTaskSystem::GetStaticTaskSystem();
	{
		sts::tools::TaskBatch< 20 > batch( system_interface );

		for( int i = 0; i < 20; ++i )
		{
			auto task_handle = system_interface->CreateNewTask( helpers::TaskFunctionDynamicTree< 100 >, nullptr );
			batch.Add( task_handle );
			helpers::WriteToTask( task_handle, 0 );
		}

		ASSERT_TRUE( batch.SubmitAll() );
		system_interface->WaitUntil( [ &batch ]() { return batch.AreAllTaskFinished(); } );

		for( auto task_handle : batch )
			ASSERT_TRUE( helpers::ReadFromTask<int>( task_handle ) == helpers::SOME_CONST );
	}
}

//////////////////////////////////////////////////////////////////////
TEST( STSTest, DynamicTaskTreeTestWithLambdas )
{
	sts::ITaskSystem* system_interface = helpers::StaticTaskSystem::GetStaticTaskSystem();

	// This is arrray that we will work on.
	std::array< int, 200 > arrayToFill = { 0 };

	// Create lambda that will process the array in parallel.
	auto root_lambda = [ &arrayToFill ]( const sts::ITaskContext* context )
	{
		sts::tools::TaskBatch< 205 >  batch( context->GetTaskSystem() );

		for( unsigned i = 0; i < arrayToFill.size(); ++i )
		{
			// Create a task that will calculate single item.
			int item = arrayToFill[ i ];
			auto child_lambda_functor = [ item ]( const sts::ITaskContext* context )
			{
				int calculated_item = helpers::CalculateItem( item );

				// Store item in data task data storage.
				sts::tools::TaskStorageWriter( context ).WriteSafe( calculated_item );
				return true;
			}; ///< end of child_lambda_functor

			// Create new task using item_functor:
			const sts::ITaskHandle* handle = sts::tools::LambdaTaskMaker( child_lambda_functor, context->GetTaskSystem() );
			batch.Add( handle );
		}

		// Submit whole batch.
		bool submitted = batch.SubmitAll();

		// Wait until whole batch is done.
		context->GetTaskSystem()->WaitUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );

		// Get results from child tasks and calculate final sum:
		int final_sum = 0;
		for( unsigned i = 0; i < batch.GetSize(); ++i )
		{
			const sts::ITaskHandle* handle = batch[ i ];
			int sum = helpers::ReadFromTask<int>( handle );

			// Fill array with apropriate results:
			arrayToFill[ i ] = sum;
			final_sum += sum;
		}

		// Write final sum:
		// NOTE: I am aware that I am overriding lambda here, but i am not going to touch any ot its stuff anymore..
		sts::tools::TaskStorageWriter( context ).WriteSafe( final_sum );
		return true;
	}; ///< end of root_lambda

	// Create main task using array_functor:
	auto root_task_handle = sts::tools::LambdaTaskMaker( root_lambda, system_interface );

	// Submit main task..
	bool submitted = system_interface->SubmitTask( root_task_handle );
	ASSERT_TRUE( submitted );

	// and help processing until main task is done:
	system_interface->WaitUntil( [ &root_task_handle ] { return root_task_handle->IsFinished(); } );

	// Read the result:
	ASSERT_TRUE( helpers::ReadFromTask<int>( root_task_handle ) == 10000000 );

	// Release main task:
	system_interface->ReleaseTask( root_task_handle );
}

//////////////////////////////////////////////////////////////////////
TEST(STSTest, DynamicTaskTreeTestWithLambdas2 )
{
	sts::ITaskSystem* system_interface = helpers::StaticTaskSystem::GetStaticTaskSystem();
	
	double numOfTries = 0;
	double accumulator = 0;
	
	for( int iteration = 0; iteration < 10; ++iteration )
	{
		auto functor = []( const sts::ITaskContext* context )
		{
			sts::tools::TaskBatch< 110 > batch( context->GetTaskSystem() );
	
			auto parent_task_handle = context->GetTaskSystem()->CreateNewTask( &helpers::TaskFunctionFast, nullptr );
	
			for( unsigned i = 0; i < 100; ++i )
			{
				auto handle = context->GetTaskSystem()->CreateNewTask( &helpers::TaskFunctionFast, parent_task_handle );
				batch.Add( handle );
			}
	
			batch.Add( parent_task_handle );
	
			bool submitted = batch.SubmitAll();
	
			context->GetTaskSystem()->WaitUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );
	
			int final_sum = 0;
			for( auto handle : batch )
			{
				int sum = helpers::ReadFromTask<int>( handle );
				final_sum += sum;
			}	
	
			sts::tools::TaskStorageWriter( context ).WriteSafe( final_sum );
			return true;
		}; //< end of functor
		////////////////////////////////////////////////////////////////////
	
		{
			sts::tools::TaskBatch< 20 > batch( system_interface );
	
			for( unsigned i = 0; i < 20; ++i )
			{
				auto handle = sts::tools::LambdaTaskMaker( functor, system_interface );
				ASSERT_TRUE( handle != nullptr );
				batch.Add( handle );
			}
	
			bool submitted = batch.SubmitAll();
			ASSERT_TRUE( submitted );
	
			system_interface->WaitUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );
	
			for( auto handle : batch )
			{
				auto bla = helpers::ReadFromTask<int>( handle );
				ASSERT_TRUE( helpers::ReadFromTask<int>( handle ) == 8080000 );
				ASSERT_TRUE( handle->IsFinished() );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
TEST(STSTest, StaticTaskTreeTest)
{
	sts::ITaskSystem* system_interface = helpers::StaticTaskSystem::GetStaticTaskSystem();
	
	for( int iteration = 0; iteration < 10; ++iteration )
	{
		sts::tools::TaskBatch< 2040 > batch( system_interface );

		auto root_task_handle = system_interface->CreateNewTask( &helpers::TaskFunctionFast, nullptr );
		ASSERT_TRUE( root_task_handle != nullptr );
	
		// Build static tree:
		for( unsigned i = 0; i < 20; ++i )
		{
			auto parent_handle_lvl2 = system_interface->CreateNewTask( &helpers::TaskFunctionFast, root_task_handle );
			ASSERT_TRUE( parent_handle_lvl2 != nullptr );
	
			for( unsigned i = 0; i < 100; ++i )
			{
				auto handle = system_interface->CreateNewTask( &helpers::TaskFunctionFast, parent_handle_lvl2 );
				ASSERT_TRUE( handle != nullptr );
				batch.Add( handle );
			}
	
			batch.Add( parent_handle_lvl2 );
		}
	
		batch.Add( root_task_handle );
	
		bool submitted = batch.SubmitAll();
		ASSERT_TRUE( submitted );
	
		system_interface->WaitUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );
	
		for( auto handle : batch )
		{
			auto bla = helpers::ReadFromTask<int>( handle );
			ASSERT_TRUE( helpers::ReadFromTask<int>( handle ) == 80000 );
			ASSERT_TRUE( handle->IsFinished() );
		}
	}
}

///////////////////////////////////////////////////////////////////
TEST( STSTest, FlushingSuspendedTasks )
{
	btl::Atomic< int > fence;

	sts::ITaskSystem* system_interface = helpers::StaticTaskSystem::GetStaticTaskSystem();
	{
		static const int TASK_SIZE = 64;

		sts::tools::TaskBatch< TASK_SIZE > batch( system_interface );

		for( int i = 0; i < TASK_SIZE; ++i )
		{
			auto lambda = [&fence ]( const sts::ITaskContext* context )
			{
				context->GetTaskSystem()->WaitUntil( [ &fence ]() { return fence.Load( btl::MemoryOrder::Acquire ) == 1; } );
				return true;
			};

			auto handle = sts::tools::LambdaTaskMaker( lambda, system_interface );
			batch.Add( handle );
		}

		HighResolutionTimer timer;
		timer.Start();

		ASSERT_TRUE( batch.SubmitAll() );
		system_interface->WaitUntil( [ &timer ]() { return timer.ElapsedTimeInSeconds() > 1.0; } );

		fence.Store( 1, btl::MemoryOrder::Release );
		btl::this_thread::SleepFor( 1000 );
		system_interface->WaitUntil( [ &batch ]() { return batch.AreAllTaskFinished();  } );
	}
}