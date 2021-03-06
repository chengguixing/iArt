#ifndef _SCOPED_PTR_H_07F913B0_D614_4B3F_9BF2_B509053BC180
#define _SCOPED_PTR_H_07F913B0_D614_4B3F_9BF2_B509053BC180

#include <memory>


namespace IAF
{
	namespace IBase
	{

		template<class T> 
		class scoped_ptr // noncopyable
		{
		private:

			T * px;

			scoped_ptr(scoped_ptr const &);
			scoped_ptr & operator=(scoped_ptr const &);

			typedef scoped_ptr<T> this_type;

			void operator==( scoped_ptr const& ) const;
			void operator!=( scoped_ptr const& ) const;

		public:

			typedef T element_type;

			explicit scoped_ptr( T * p = 0 ): px( p ) // never throws
			{
				;
			}


			explicit scoped_ptr( std::auto_ptr<T> p ) : px( p.release() )
			{
				;
			}

			~scoped_ptr() // never throws
			{
				checked_delete( px );
			}

			void reset(T * p = 0) // never throws
			{
				this_type(p).swap(*this);
			}

			T & operator*() const // never throws
			{
				return *px;
			}

			T * operator->() const // never throws
			{
				return px;
			}

			T * get() const
			{
				return px;
			}

			void swap(scoped_ptr & b)
			{
				T * tmp = b.px;
				b.px = px;
				px = tmp;
			}
		}; 

		template<class T> 
		inline void swap(scoped_ptr<T> & a, scoped_ptr<T> & b) 
		{
			a.swap(b);
		}

		// get_pointer(p) is a generic way to say p.get()

		template<class T> 
		inline T * get_pointer(scoped_ptr<T> const & p) 
		{
			return p.get();
		}
	}
}          


#endif                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              