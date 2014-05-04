#include <iostream> 
#include <stdlib.h>
#include <iostream>
#include <boost/intrusive_ptr.hpp>
using namespace std;

/**
* intrusive_ptr_base基类，提供intrusive_ptr_add_ref()和intrusive_ptr_release()函数来提供引用计数功能；
* 使用boost::intrusive_ptr指针存储的用户类类型必须继承自intrusive_ptr_base基类。
*/
#include <ostream>
#include <boost/checked_delete.hpp>
#include <boost/detail/atomic_count.hpp>
 
 
template<class T>
class intrusive_ptr_base {
public:
    /**
    * 缺省构造函数
    */
    intrusive_ptr_base(): ref_count(0) {
        std::cout << "  Default constructor " << std::endl;
    }
     
    /**
    * 不允许拷贝构造，只能使用intrusive_ptr来构造另一个intrusive_ptr
    */
    intrusive_ptr_base(intrusive_ptr_base<T> const&): ref_count(0) {
        std::cout << "  Copy constructor..." << std::endl;
    }
     
    /**
    * 不允许进行赋值操作
    */
    intrusive_ptr_base& operator=(intrusive_ptr_base const& rhs) {
        std::cout << "  Assignment operator..." << std::endl;
        return *this;
    }
     
    /**
    * 递增引用计数（放到基类中以便compiler能找到，否则需要放到boost名字空间中）
    */
    friend void intrusive_ptr_add_ref(intrusive_ptr_base<T> const* s) {
        std::cout << "  intrusive_ptr_add_ref..." << std::endl;
        assert(s->ref_count >= 0);
        assert(s != 0);
        ++s->ref_count;
    }
 
    /**
    * 递减引用计数
    */
    friend void intrusive_ptr_release(intrusive_ptr_base<T> const* s) {
        std::cout << "  intrusive_ptr_release..." << std::endl;
        assert(s->ref_count > 0);
        assert(s != 0);
        if (--s->ref_count == 0)
            boost::checked_delete(static_cast<T const*>(s));  //s的实际类型就是T，intrusive_ptr_base<T>为基类
    }
     
    /**
    * 类似于shared_from_this()函数
    */
    boost::intrusive_ptr<T> self() {
        return boost::intrusive_ptr<T>((T*)this);
    }
     
    boost::intrusive_ptr<const T> self() const {
        return boost::intrusive_ptr<const T>((T const*)this);
    }
     
    int refcount() const {
        return ref_count;
    }
     
private:
    ///should be modifiable even from const intrusive_ptr objects
    mutable boost::detail::atomic_count ref_count;
 
};


class Connection : public intrusive_ptr_base<Connection>
{
public:
	Connection(int id, string tag) : connection_id(id), connection_tag(tag)
	{
		;
	}

	Connection(const Connection& rhs) : connection_id(rhs.connection_id), connection_tag(rhs.connection_tag)
	{
		;
	}
	const Connection operator= (const Connection& rhs)
	{
		if (this != &rhs)
		{
			connection_id = rhs.connection_id;
			connection_tag = rhs.connection_tag;
		}
	}
private:
	int connection_id;
	string connection_tag;
};

int main(void) 
{ 
	{
	cout << "Create an intrusive ptr" << endl;

	boost::intrusive_ptr<Connection> con0(new Connection(4, "sss"));
	cout << "Create an intrusive ptr. Refcount = " << con0->refcount() << endl;
	
	boost::intrusive_ptr<Connection> con1(con0);
	cout << "Create an intrusive ptr. Refcount = " << con1->refcount() << endl;
	
	boost::intrusive_ptr<Connection> con2(con1);
	cout << "Create an intrusive ptr. Refcount = " << con2->refcount() << endl;

	cout << "Destroy an intrusive ptr" << endl;
	}


	system("pause");
	return 0;
} 