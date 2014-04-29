#ifndef _DYNBASE_H_247E1E63_3955_4A70_A9CE_DD56AE32E303
#define _DYNBASE_H_247E1E63_3955_4A70_A9CE_DD56AE32E303

#include <string>

class Object;
class ClassInfo;

typedef Object* (*ObjectConstructorFn)(void);
bool Register(ClassInfo* ci);

class ClassInfo
{
public:
	ClassInfo(const std::string className, ObjectConstructorFn ctor) : m_className(className), m_objectConstructor(ctor)
	{
		Register(this);
	};
	virtual ~ClassInfo()
	{
		NULL;
	}
	Object* CreateObject() const
	{
		return m_objectConstructor ? (*m_objectConstructor)() : 0;
	}
	bool IsDynamic() const
	{
		return NULL != m_objectConstructor;
	}
	const std::string GetClassName() const
	{
		return m_className;
	}
	ObjectConstructorFn GetConstructor() const
	{
		return m_objectConstructor;
	}
public:
	std::string m_className;
	ObjectConstructorFn m_objectConstructor;
};


#define DECLARE_CLASS(name) \
	protected: \
		static ClassInfo ms_classinfo; \
	public: \
		virtual ClassInfo* GetClassInfo() const; \
		static Object* CreateObject();
#define IMPLEMENT_CLASS_COMMON(name, func) \
		ClassInfo name::ms_classinfo((#name), \
			(ObjectConstructorFn) func); \
		ClassInfo* name::GetClassInfo() const \
		{ \
			return &name::ms_classinfo; \
		}
#define IMPLEMENT_CLASS(name) \
	IMPLEMENT_CLASS_COMMON(name, name::CreateObject) \
	Object* name::CreateObject() \
		{ \
			return new name; \
		}

class Object
{
	DECLARE_CLASS(Object);
	public:
	Object()
	{
		NULL;
	}
	virtual ~Object()
	{
		NULL;
	}
	static bool Register(ClassInfo* ci);
	static Object* CreateObject(std::string name);
};


#endif
