/*
//一般用法
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>


#define STR(s) #s                    //#与参数之间可以有空格
#define TOCHAR(c) #@c          
#define CONS(a, b) int(a##e##b)      // ##与参数之间可以有空格

int main(void)
{
    printf(STR(pele));               // 输出字符串"pele"
    printf("%c\n", TOCHAR(z));       // 输出字符z
    printf("%d\n", CONS(2, 3));      // 2e3 输出:2000

	system("pause");
    return 0;
}
*/
/*
//当宏参数是另一个宏的时候
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>


#define A                 (2)
#define _STR(s) #s
#define STR(s) _STR(s)

#define _CONS(a,b)  int(a##e##b)
#define CONS(a,b)    _CONS(a,b)

int main(void)
{
    //printf("int max : %s\n", STR(INT_MAX));




	system("pause");
    return 0;
}
*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;


#define FILL(a) {a,#a}
enum IDD{OPEN, CLOSE};

typedef struct MSG
{
	IDD id;
	const char* msg;
}MSG;



#define _GET_FILE_NAME(f) #f
#define GET_FILE_NAME(f) _GET_FILE_NAME(f)
static char FILE_NAME[] = GET_FILE_NAME(__FILE__);


#define _TYPE_BUF_SIZE(type) sizeof#type
#define TYPE_BUF_SIZE(type) _TYPE_BUF_SIZE(type)
char buf[TYPE_BUF_SIZE(INT_MAX)];


int main(void)
{

	MSG _msg[] = {FILL(OPEN), FILL(CLOSE)};

	cout << FILE_NAME << endl;
	cout << sizeof(buf) << endl; 


	system("pause");
    return 0;
}