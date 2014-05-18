/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.1.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
Module:  HPSocket for C DLL

Desc: ������ C ���������������ԣ��磺C / C# / Delphi �ȣ��ܷ����ʹ�� HPSocket

Usage:
		����һ��
		--------------------------------------------------------------------------------------
		0. ��C/C++ ���򣩰��� HPSocket4C.h ͷ�ļ�
		1. ���� ::Create_HP_XxxListener() ������������������
		2. ���� ::Create_HP_Xxx(pListener) �������� HPSocket ����
		3. ���� ::HP_Set_FN_Xxx_OnYyy(pListener, ...) �������ü������Ļص�����
		4. ������ص����������� HPSocket ����
		5. ...... ......
		6. ���� ::Destroy_HP_Xxx(pSocket) �������� HPSocket ����
		7. ���� ::Destroy_HP_XxxListener(pListener) �������ټ���������

		��������
		--------------------------------------------------------------------------------------
		1. Ӧ�ó������Ҫ�õ��ĵ���������װ���ض����Եİ�װ����
		2. ͨ����װ���װ�����������ķ�ʽʹ�� HPSocket

Release:
		1. Bin/x86/HPSocket4C.dll		- (32λ/MBCS/Release)
		2. Bin/x86/HPSocket4C_D.dll		- (32λ/MBCS/DeBug)
		3. Bin/x86/HPSocket4C_U.dll		- (32λ/UNICODE/Release)
		4. Bin/x86/HPSocket4C_UD.dll	- (32λ/UNICODE/DeBug)
		5. Bin/x64/HPSocket4C.dll		- (64λ/MBCS/Release)
		6. Bin/x64/HPSocket4C_D.dll		- (64λ/MBCS/DeBug)
		7. Bin/x64/HPSocket4C_U.dll		- (64λ/UNICODE/Release)
		8. Bin/x64/HPSocket4C_UD.dll	- (64λ/UNICODE/DeBug)

******************************************************************************/
