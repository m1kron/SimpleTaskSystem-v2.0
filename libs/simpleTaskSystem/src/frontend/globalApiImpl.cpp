#include "precompiledHeader.h"
#include "..\..\include\globalApi.h"
#include "..\backend\backendTaskSystem.h"
#include "frontendTaskSystem.h"

/////////////////////////////////////////////////////////////////
STS_API sts::ITaskSystem* CreateTaskSystem()
{
	// TODO: prepare that all of this operations can fail!
	void* backend_mem = _aligned_malloc( sizeof( sts::BackendTaskSystem ), BTL_CACHE_LINE_SIZE );
	sts::BackendTaskSystem* backend = new ( backend_mem ) sts::BackendTaskSystem();

	void* frontend_mem = _aligned_malloc( sizeof( sts::frontend::FrontendTaskSystem ), BTL_CACHE_LINE_SIZE );
	sts::frontend::FrontendTaskSystem* frontend = new ( frontend_mem ) sts::frontend::FrontendTaskSystem();
	
	VERIFY_SUCCESS( frontend->Initialize( backend ) );

	return frontend;
}

//////////////////////////////////////////////////////////////////
STS_API void DestroyTaskSystem( sts::ITaskSystem* system )
{
	ASSERT( system );

	sts::frontend::FrontendTaskSystem* frontend = static_cast< sts::frontend::FrontendTaskSystem* >( system );
	sts::BackendTaskSystem* backend = frontend->Deinitialize();

	ASSERT( backend );

	frontend->~FrontendTaskSystem();
	backend->~BackendTaskSystem();
	_aligned_free( frontend );
	_aligned_free( backend );
}