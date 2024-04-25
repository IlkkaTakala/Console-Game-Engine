#include "BaseObject.h"
#include "../GarbageCollector.h"

//void BaseObject::operator delete(void* ptr)
//{ 
//	//GC::RemoveObject(reinterpret_cast<BaseObject*>(ptr));
//	::delete ptr;
//}

void BaseObject::DestroyObject()
{
	GC::RemoveObject(this);
}
