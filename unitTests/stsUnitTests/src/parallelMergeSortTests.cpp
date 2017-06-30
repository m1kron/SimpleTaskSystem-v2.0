#include <gtest\gtest.h>
#include <vector>
#include <random>
#include "..\..\libs\simpleTaskSystem\include\globalApi.h"
#include "..\..\libs\simpleTaskSystem\include\tools\parallelMergeSort.h"


namespace helpers
{
	const size_t SIZE = 50000000;

	////////////////////////////////////////////////////////////////////////////
	std::vector< int > CreateRandomVector( size_t size )
	{
		auto container = std::vector< int >( size );
		std::generate( container.begin(), container.end(), std::rand );
		return container;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
TEST( STSParallelMergeSortTest, SortSequential )
{
	auto container = helpers::CreateRandomVector( helpers::SIZE );
	std::sort( container.begin(), container.end() );
	ASSERT_TRUE( std::is_sorted( container.begin(), container.end() ) );
}

/////////////////////////////////////////////////////////////////////////////////////
TEST( STSParallelMergeSortTest, SortParallel )
{
	sts::ITaskSystem* system = CreateTaskSystem();

	auto container = helpers::CreateRandomVector( helpers::SIZE );
	ASSERT_TRUE( sts::tools::ParallelMergeSort( container, system ) );
	ASSERT_TRUE( std::is_sorted( container.begin(), container.end() ) );

	DestroyTaskSystem( system );
}