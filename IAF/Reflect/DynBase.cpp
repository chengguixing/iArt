#include "DynBase.h"
#include <map>

static std::map<std::string, ClassInfo*>* classInfoMap = NULL;

IMPLEMENT_CLASS(Object)

bool Object::Register(ClassInfo* ci)
{
	if (!classInfoMap)
	{
		classInfoMap = new std::map<std::string, ClassInfo*>();
	}
	if (ci)
	{
		if (classInfoMap->find(ci->m_className) == classInfoMap->end())
		{
			classInfoMap->insert(std::map<std::string, ClassInfo*>::value_type(ci->m_className, ci));
		}
		return true;
	}
}


Object* Object::CreateObject(std::string name)
{
	std::map<std::string, ClassInfo*>::const_iterator iter = classInfoMap->find(name);
	if (classInfoMap->end() != iter)
	{
		return iter->second->CreateObject();
	}
	return NULL;
}

bool Register(ClassInfo* ci)
{
	return Object::Register(ci);
}