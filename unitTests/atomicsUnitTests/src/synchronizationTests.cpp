#include <gtest\gtest.h>
#include "..\..\libs\commonLib\include\timer\timerMacros.h"
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

		HighResolutionTimer timer;
		timer.Start();

		btl::FunctorThread thread1;
		thread1.SetFunctorAndStartThread( [ &m_lock ]
		{
			btl::LockGuard< TMutex > guard( m_lock );
			btl::this_thread::SleepFor( 1000 );
		} );

		btl::this_thread::SleepFor( 10 );

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
