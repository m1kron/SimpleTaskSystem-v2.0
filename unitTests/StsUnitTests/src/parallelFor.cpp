#include <gtest\gtest.h>
#include <array>
#include <sts\tools\ParallelFor.h>

typedef std::array<int, 16 > TArray16;
typedef std::array<int, 8 > TArray8;

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
}

//////////////////////////////////////////////////////////////////////
TEST(STSParallelFor, SingleThread_8_Test )
{
	TArray8 m_TestArray;

	for( auto it = m_TestArray.begin(); it != m_TestArray.end(); ++it ) 
		helpers::CalculateBigSum( it );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing2Threads_8_Test )
{
	TArray8 m_TestArray;

	sts::ParallelForEach( m_TestArray.begin(), m_TestArray.end(), 
		[] ( TArray8::iterator& it )
		{
			helpers::CalculateBigSum( it );
		}, 2 );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing4Threads_8_Test )
{
	TArray8 m_TestArray;

	sts::ParallelForEach( m_TestArray.begin(), m_TestArray.end(),
		[]( TArray8::iterator& it )
		{
			helpers::CalculateBigSum( it );
		}, 4 );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing8Threads_8_Test )
{
	TArray8 m_TestArray;

	sts::ParallelForEach( m_TestArray.begin(), m_TestArray.end(),
		[]( TArray8::iterator& it )
		{
			helpers::CalculateBigSum( it );
		}, 8 );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsingTasks_8_Test )
{
	TArray8 m_TestArray;
	sts::TaskManager manager;
	manager.Setup();

	sts::ParallelForEachUsingTasks( m_TestArray.begin(), m_TestArray.end(),
		[]( TArray8::iterator& it )
	{
		helpers::CalculateBigSum( it );
	}, manager );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, SingleThread_16_Test )
{
	TArray16 m_TestArray;

	for( auto it = m_TestArray.begin(); it != m_TestArray.end(); ++it )
		helpers::CalculateBigSum( it );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing2Threads_16_Test )
{
	TArray16 m_TestArray;

	sts::ParallelForEach( m_TestArray.begin(), m_TestArray.end(),
		[]( TArray16::iterator& it )
	{
		helpers::CalculateBigSum( it );
	}, 2 );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing4Threads_16_Test )
{
	TArray16 m_TestArray;

	sts::ParallelForEach( m_TestArray.begin(), m_TestArray.end(),
		[]( TArray16::iterator& it )
	{
		helpers::CalculateBigSum( it );
	}, 4 );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing8Threads_16_Test )
{
	TArray16 m_TestArray;

	sts::ParallelForEach( m_TestArray.begin(), m_TestArray.end(),
		[]( TArray16::iterator& it )
	{
		helpers::CalculateBigSum( it );
	}, 8 );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsingTasks_16_Test )
{
	TArray16 m_TestArray;
	sts::TaskManager manager;
	manager.Setup();

	sts::ParallelForEachUsingTasks( m_TestArray.begin(), m_TestArray.end(),
		[]( TArray16::iterator& it )
	{
		helpers::CalculateBigSum( it );
	}, manager );

	ASSERT_TRUE( helpers::ChcekIfEqual( m_TestArray.begin(), m_TestArray.end() ) );
}