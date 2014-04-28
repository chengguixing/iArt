#ifndef _CHECKED_DELETE_H_AB60EA73_F979_4033_8EAE_34DB3ACEA0F0
#define _CHECKED_DELETE_H_AB60EA73_F979_4033_8EAE_34DB3ACEA0F0

namespace IAF
{
	namespace IBase
	{
		template <typename T>
		inline void checked_delete(T* x)
		{
			typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
			(void)sizeof(type_must_be_complete);
			delete x;
		}

		template <typename T>
		inline void checked_array_delete(T* x)
		{
			typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
			(void)sizeof(type_must_be_complete);
			delete [] x;
		}

		template <typename T>
		struct checked_deleter
		{
			typedef void result_type;
			typedef T* argument_type;

			void operator() (T* x) const
			{
				checked_delete(x);
			}
		};

		template <typename T>
		struct checked_array_deleter
		{
			typedef void result_type;
			typedef T* argument_type;

			void operator() (T* x) const
			{
				checked_array_delete(x);
			}
		};
	}
}


#endif