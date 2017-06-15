#include <gtest\gtest.h>
#include <sts\lowlevel\thread\fiber.h>
#include <vector>

namespace helpers
{
	//////////////////////////////////////////////////////
	// class used for testing fibers
	class TestFiber : public sts::FiberBase
	{
	public:
		TestFiber()
			: sts::FiberBase( true )
			, m_integer( nullptr )
			, m_base( 1 )
			, m_nextFiber( nullptr )
			, m_prevFiber( nullptr )
			, m_prevID()
		{
		}

		~TestFiber() {}

		void FiberFunction() override
		{
			if( m_nextFiber )
			{
				m_nextFiber->m_prevFiber = this;
				sts::this_fiber::SwitchToFiber( m_nextFiber->GetFiberID() );
			}

			*m_integer += m_base * 9;

			if( m_prevFiber )
				sts::this_fiber::SwitchToFiber( m_prevFiber->GetFiberID() );
			else
				sts::this_fiber::SwitchToFiber( m_prevID );
		}

		unsigned m_base;
		unsigned* m_integer;
		TestFiber* m_nextFiber;
		TestFiber* m_prevFiber;
		sts::FIBER_ID m_prevID;
	};
}

//////////////////////////////////////////////////////////////////////////
TEST( FiberTests, FiberSwitching )
{
	std::vector< helpers::TestFiber* > fibers;
	static const unsigned NUM_OF_FIBERS = 5;

	unsigned globalInt = 0;
	for( unsigned i = 0; i < NUM_OF_FIBERS; ++i )
	{
		fibers.push_back( new helpers::TestFiber() );
		helpers::TestFiber* fiber = fibers.back();
		fiber->m_base = ( unsigned )pow( 10, i );
		fiber->m_integer = &globalInt;

		if( i > 0 )
			fibers[ i - 1 ]->m_nextFiber = fiber;
	}

	sts::FIBER_ID thisThreadFiberID = sts::this_fiber::ConvertThreadToFiber();

	helpers::TestFiber* first = fibers[ 0 ];
	first->m_prevID = thisThreadFiberID;

	sts::this_fiber::SwitchToFiber( first->GetFiberID() );

	ASSERT_TRUE( sts::this_fiber::ConvertFiberToThread() );

	unsigned wantedInt = ( unsigned ) pow( 10, NUM_OF_FIBERS );
	wantedInt -= 1;
	ASSERT_TRUE( globalInt == wantedInt );

	for( auto* fiber : fibers )
		delete fiber;

	fibers.clear();
}