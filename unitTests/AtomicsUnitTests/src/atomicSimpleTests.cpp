#include <gtest\gtest.h>
#include "..\..\libs\basicThreadingLib\include\atomic\atomic.h"
#include "..\..\libs\basicThreadingLib\include\thread\functorThread.h"

////////////////////////////////////////////////////////////
TEST( AtomicUnitTests, SimpleArithmeticTest )
{
	btl::Atomic< int > atm;

	// UNIT TESTS OF ATOMICS:
	ASSERT_TRUE( ( atm.Store( 2 ), atm.Load() == 2 ) );
	ASSERT_TRUE( atm.Exchange( 5 ) == 2 );
	ASSERT_TRUE( atm.Load() == 5 );

	int k = 5;
	ASSERT_TRUE( ( atm.CompareExchange( k, 10 ), atm == 10 ) );
	ASSERT_TRUE( ( atm.FetchAdd( 10 ), atm == 20 ) );
	ASSERT_TRUE( ( atm.FetchSub( 5 ), atm == 15 ) );
	ASSERT_TRUE( ( atm.FetchOr( 5 ), atm == ( 15 | 5 ) ) );
	atm.Store( 100 );
	ASSERT_TRUE( ( atm.FetchAnd( 5 ), atm == ( 100 & 5 ) ) );
	atm.Store( 120 );
	ASSERT_TRUE( ( ++atm, atm == 121 ) );
	ASSERT_TRUE( ( --atm, atm == 120 ) );
}

/////////////////////////////////////////////////////////////
TEST( AtomicUnitTests, SimpleTwoThreadedInLoop )
{
	struct Test
	{
		void Add() { for( unsigned int i = 0; i < 10000; ++i ) { ++atomic_int; } }
		void Sub() { for( unsigned int i = 0; i < 1000; ++i ) { atomic_int.FetchSub(5); } }
		void Exchange()
		{
			unsigned int expected = 20000;
			while ( !atomic_int.CompareExchange( expected, -1 ) )
			{ 
				expected = 20000;
			}
		}
	
		btl::Atomic< unsigned int > atomic_int;
	};

	int iteration = 1000;
	while( iteration != 0 ) 
	{
		Test test;
	
		btl::FunctorThread thread1;
		thread1.SetFunctorAndStartThread( [ &test ] { test.Add(); } );
	
		btl::FunctorThread thread2;
		thread2.SetFunctorAndStartThread( [ &test ] { test.Sub(); } );
	
		thread1.Join();
		thread2.Join();
	
		ASSERT_TRUE( test.atomic_int == ( 10000 - 5000 ) );
	
		Test test2;
	
		btl::FunctorThread thread12;
		thread12.SetFunctorAndStartThread( [ &test2 ] { test2.Add(); } );
	
		btl::FunctorThread thread22;
		thread22.SetFunctorAndStartThread( [ &test2 ] { test2.Add(); } );
	
		btl::FunctorThread thread32;
		thread32.SetFunctorAndStartThread( [ &test2 ] { test2.Exchange(); } );
	
		thread12.Join();
		thread22.Join();
		thread32.Join();
	
		ASSERT_TRUE( test2.atomic_int == ( -1 ) );
		--iteration;
	}
}

/////////////////////////////////////////////////////////////
TEST( AtomicUnitTests, AquireReleaseTest )
{
	// NOTE: this test will pass without using atomic on x86 architecture..

	static const int MAX_ITERATIONS = 1000000000;

	btl::Atomic<int> g_atomic;
	struct Number
	{
		int a;
		int b;
		int c;
	};

	Number g_number = { 0, 0, 0 };

	btl::FunctorThread thread1;
	thread1.SetFunctorAndStartThread( [ &g_atomic, &g_number ] 
	{ 
		for ( int i = 0; i < MAX_ITERATIONS; ++i )
		{
			auto num = g_atomic.Load( btl::MemoryOrder::Acquire );
			if( num == 0 )
			{
				g_number.a++;
				g_number.b += 2;
				g_number.c += 4;
				g_atomic.Store( 1, btl::MemoryOrder::Release );
			}
		}
	} );

	btl::FunctorThread thread2;
	thread2.SetFunctorAndStartThread( [ &g_atomic, &g_number ]
	{
		for( int i = 0; i < MAX_ITERATIONS; ++i )
		{
			auto num = g_atomic.Load( btl::MemoryOrder::Acquire );
			if( num == 1 )
			{
				g_number.a++;
				g_number.b += 2;
				g_number.c += 4;
				g_atomic.Store( 0, btl::MemoryOrder::Release );
			}
		}
	} );

	thread1.Join();
	thread2.Join();

	ASSERT_TRUE( g_number.b == 2 * g_number.a );
	ASSERT_TRUE( g_number.c == 4 * g_number.a );
}