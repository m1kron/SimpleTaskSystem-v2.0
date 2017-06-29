#include <gtest\gtest.h>
#include "..\..\libs\basicThreadingLib\include\thread\fiber.h"
#include <vector>

namespace helpers
{
	//////////////////////////////////////////////////////
	// class used for testing fibers
	class TestFiber : public btl::FiberBase
	{
	public:
		TestFiber()
			: btl::FiberBase( true )
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
				btl::this_fiber::SwitchToFiber( m_nextFiber->GetFiberID() );
			}

			*m_integer += m_base * 9;

			if( m_prevFiber )
				btl::this_fiber::SwitchToFiber( m_prevFiber->GetFiberID() );
			else
				btl::this_fiber::SwitchToFiber( m_prevID );
		}

		unsigned m_base;
		unsigned* m_integer;
		TestFiber* m_nextFiber;
		TestFiber* m_prevFiber;
		btl::FIBER_ID m_prevID;
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

	ASSERT_FALSE( btl::this_fiber::IsThreadConvertedToFiber() );

	btl::FIBER_ID thisThreadFiberID = btl::this_fiber::ConvertThreadToFiber();

	ASSERT_TRUE( btl::this_fiber::IsThreadConvertedToFiber() );

	helpers::TestFiber* first = fibers[ 0 ];
	first->m_prevID = thisThreadFiberID;

	btl::this_fiber::SwitchToFiber( first->GetFiberID() );

	ASSERT_TRUE( btl::this_fiber::ConvertFiberToThread() );

	unsigned wantedInt = ( unsigned ) pow( 10, NUM_OF_FIBERS );
	wantedInt -= 1;
	ASSERT_TRUE( globalInt == wantedInt );

	for( auto* fiber : fibers )
		delete fiber;

	fibers.clear();

	auto bla = btl::this_fiber::GetFiberID();

	ASSERT_FALSE( btl::this_fiber::IsThreadConvertedToFiber() );
}