#include "StatTime.h"
#include <stdlib.h>



int main(void)
{
	for (int i = 0; i < 10; ++i)
	{
		StatTimeInstance.Start();
		Sleep(i * 1000);
		StatTimeInstance.Stop();
	}

	system("pause");
	return 0;
}