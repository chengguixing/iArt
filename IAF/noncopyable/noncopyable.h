#ifndef _NONCOPYABLE_H_426FF062_EB93_45CA_A135_0027492AD021
#define _NONCOPYABLE_H_426FF062_EB93_45CA_A135_0027492AD021


namespace IAF
{
	namespace IBase
	{
		class noncopyable
		{
		protected:
			noncopyable(){}
			~noncopyable(){}
		private:
			noncopyable(const noncopyable&);
			noncopyable& operator= (const noncopyable&);
		};
	}
}


#endif