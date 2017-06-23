#include "..\include\synchro\spinLock.h"
#include "..\include\thread\thisThreadHelpers.h"

NAMESPACE_BTL_BEGIN

//////////////////////////////////////////////////////////////////
SpinLock::SpinLock()
{
}

//////////////////////////////////////////////////////////////////
SpinLock::~SpinLock()
{
}

//////////////////////////////////////////////////////////////////
void SpinLock::Lock()
{
	unsigned i = 0;
	while( true )
	{
		if( TryToLock() )
			return; // We acuired lock!


		// We have to keep trying.
		// [ NOTE ]: It turns out that on processors that have HT( sts was only tested on such processors.. )
		// yielding on current processor gives best performance for high contantion situation( comparing to mutex ).
		this_thread::YieldThread();
	}
}

/////////////////////////////////////////////////////////////////
bool SpinLock::TryToLock()
{
	if( m_atomic.Exchange( 1, MemoryOrder::Acquire ) == 0 )
		return true; // We acuired lock!

	return false;
}

//////////////////////////////////////////////////////////////////
void SpinLock::Unlock()
{
	m_atomic.Store( 0, MemoryOrder::Release );
}

NAMESPACE_BTL_END
