#include <gtest\gtest.h>
#include "..\..\libs\commonLib\include\timer\timerMacros.h"
#include "..\..\libs\basicThreadingLib\include\atomic\atomic.h"
#include "..\..\libs\basicThreadingLib\include\thread\functorThread.h"
#include "..\..\libs\basicThreadingLib\include\synchro\spinLock.h"
#include "..\..\libs\basicThreadingLib\include\synchro\lockGuards.h"
#include "..\..\libs\basicThreadingLib\include\synchro\mutex.h"

namespace helpers
{
	///////////////////////////////////////////////////////////////////////////
	template< class TMutex >
	void SleepOnTest()
	{
		TMutex m_lock;

		btl::FunctorThread thread1;
		thread1.SetFunctorAndStartThread( [ &m_lock ]
		{
			btl::LockGuard< TMutex > guard( m_lock );
			btl::this_thread::SleepFor( 1000 );
		} );

		btl::this_thread::SleepFor( 10 );

		HighResolutionTimer timer;
		timer.Start();

		btl::FunctorThread thread2;
		thread2.SetFunctorAndStartThread( [ &m_lock, &timer ]
		{
			btl::LockGuard< TMutex > guard( m_lock );
			timer.Stop();
		} );

		thread1.Join();
		thread2.Join();

		auto elapsed = timer.ElapsedTimeInSeconds();
		ASSERT_TRUE( elapsed > 0.99 );
	}

	//////////////////////////////////////////////////////////////////////////////////
	template< class TMutex >
	void SimpleAddingTest()
	{
		for( int i = 0; i < 50; ++i )
		{
			TMutex m_lock;
			struct SomeStruct
			{
				int val1;
				int val2;
				int val3;
				int val4;
			};

			SomeStruct globalStruct{ 0, 0, 0, 0 };

			static const int iterationCount = 1000000;

			btl::FunctorThread thread1;
			thread1.SetFunctorAndStartThread( [ &m_lock, &globalStruct ]
			{
				for( int i = 0; i < iterationCount; ++i )
				{
					btl::LockGuard< TMutex > guard( m_lock );
					globalStruct.val1 += 20;
					globalStruct.val2 -= 10;
					globalStruct.val3 += 20;
					globalStruct.val4 -= 10;
				}
			} );

			btl::FunctorThread thread2;

			thread2.SetFunctorAndStartThread( [ &m_lock, &globalStruct ]
			{
				for( int i = 0; i < iterationCount; ++i )
				{
					btl::LockGuard< TMutex > guard( m_lock );
					globalStruct.val1 -= 10;
					globalStruct.val2 += 20;
					globalStruct.val3 -= 10;
					globalStruct.val4 += 20;
				}
			} );

			thread1.Join();
			thread2.Join();

			ASSERT_TRUE( globalStruct.val1 == 10 * iterationCount );
			ASSERT_TRUE( globalStruct.val2 == 10 * iterationCount );
			ASSERT_TRUE( globalStruct.val3 == 10 * iterationCount );
			ASSERT_TRUE( globalStruct.val4 == 10 * iterationCount );
		}
	}

	///////////////////////////////////////////////////////////////////////////
	template< class TMutex >
	void StressTest()
	{
		TMutex m_lock;
		static const int ITERATIONS = 10000000;
		int globalint = 0;

		btl::FunctorThread thread1;
		thread1.SetFunctorAndStartThread( [ &m_lock, &globalint ]
		{
			for( int i = 0; i < ITERATIONS; ++i )
			{
				btl::LockGuard< TMutex > guard( m_lock );
				globalint++;
			}
		} );

		btl::FunctorThread thread2;
		thread2.SetFunctorAndStartThread( [ &m_lock, &globalint ]
		{
			for( int i = 0; i < ITERATIONS; ++i )
			{
				btl::LockGuard< TMutex > guard( m_lock );
				globalint++;
			}
		} );


		thread1.Join();
		thread2.Join();

		ASSERT_TRUE( globalint = 2 * ITERATIONS );
	}
}

//////////////////////////////////////////////////////////////////////
TEST( STSCommonLib, CompileTimeUtilsTest )
{
	STATIC_ASSERT( IsPowerOf2<2048>::value == 1, "IsPowerOf2 does not work!" );
	STATIC_ASSERT( IsPowerOf2<2049>::value == 0, "IsPowerOf2 does not work!" );
	STATIC_ASSERT( IsPowerOf2<128>::value == 1, "IsPowerOf2 does not work!" );
	STATIC_ASSERT( IsPowerOf2<130>::value == 0, "IsPowerOf2 does not work!" );
}

////////////////////////////////////////////////////////////
TEST( AtomicUnitTests, SimpleArithmeticTest )
{
	btl::Atomic< int > atm;

	// UNIT TESTS OF ATOMICS:
	ASSERT_TRUE( ( atm.Store( 2 ), atm.Load() == 2 ) );
	ASSERT_TRUE( atm.Exchange( 5 ) == 2 );
	ASSERT_TRUE( atm.Load() == 5 );

	int k = 5;
	ASSERT_TRUE( ( atm.CompareExchange( k, 10 ), atm.Load() == 10 ) );
	ASSERT_TRUE( ( atm.FetchAdd( 10 ), atm.Load() == 20 ) );
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
	while( iteration != 0 ) // repeated 10000
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

///////////////////////////////////////////////////////////////////////////
TEST( SynchronizationTests, SimpleStressTest_SpinLock )
{
	helpers::StressTest< btl::SpinLock >();
}

///////////////////////////////////////////////////////////////////////////
TEST( SynchronizationTests, SimpleStressTest_Mutex )
{
	helpers::StressTest< btl::Mutex >();
}

///////////////////////////////////////////////////////////////////////////
TEST( SynchronizationTests, AddingOnSpinLock )
{
	helpers::SimpleAddingTest< btl::SpinLock >();
}

///////////////////////////////////////////////////////////////////////////
TEST( SynchronizationTests, AddingOnMutex )
{
	helpers::SimpleAddingTest< btl::Mutex >();
}

///////////////////////////////////////////////////////////////////////////
TEST( SynchronizationTests, SleepOnSpinLock )
{
	helpers::SleepOnTest< btl::SpinLock >();
}

///////////////////////////////////////////////////////////////////////////
TEST( SynchronizationTests, SleepOnMutex )
{
	helpers::SleepOnTest< btl::Mutex >();
}
