#include "MemoryManager.h"
#include "MemoryManagerGlfw.h"

MemoryManager* MemoryManager::_memoryManager = 0;

MemoryManager::MemoryManager()
{

}

MemoryManager* MemoryManager::Instance()
{
	if (_memoryManager == 0)
	{
#ifdef ANDROMEDA_GLFW
		{
			_memoryManager = new MemoryManagerGlfw();
		}
#endif

#ifdef ANDROMEDA_VITA
		{
			_memoryManager = new MemoryManagerGxm();
		}
#endif

#ifdef ANDROMEDA_SWITCH
		{
			_memoryManager = new MemoryManagerSwitch();
		}
#endif

#ifdef ANDROMEDA_RPI
		{
			_memoryManager = new MemoryManagerRpi();
		}
#endif
	}

	return _memoryManager;
}