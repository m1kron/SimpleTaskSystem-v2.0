#include "precompiledHeader.h"
#include "..\..\include\globalApi.h"
#include "..\backend\backendTaskSystem.h"
#include "..\frontend\frontendTaskSystem.h"

/////////////////////////////////////////////////////////////////
STS_API sts::ITaskSystem* CreateTaskSystem()
{
	// TODO: prepare that all of this operations can fail!
	void* backend_mem = _aligned_malloc( sizeof( sts::BackendTaskSystem ), BTL_CACHE_LINE_SIZE );
	sts::BackendTaskSystem* backend = new ( backend_mem ) sts::BackendTaskSystem();

	void* frontend_mem = _aligned_malloc( sizeof( sts::FrontendTaskSystem ), BTL_CACHE_LINE_SIZE );
	sts::FrontendTaskSystem* frontend = new ( frontend_mem ) sts::FrontendTaskSystem();
	
	VERIFY_SUCCESS( frontend->Initialize( backend ) );

	return frontend;
}

//////////////////////////////////////////////////////////////////
STS_API void DestroyTaskSystem( sts::ITaskSystem* system )
{
	ASSERT( system );

	sts::FrontendTaskSystem* frontend = static_cast< sts::FrontendTaskSystem* >( system );
	sts::BackendTaskSystem* backend = frontend->Deinitialize();

	ASSERT( backend );

	frontend->~FrontendTaskSystem();
	backend->~BackendTaskSystem();
	_aligned_free( frontend );
	_aligned_free( backend );
}