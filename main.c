#include <stdio.h>
#include <disp_manager.h>
#include <page_manager.h>

/*
���Է���
��Ҫ��ts.ko
insmod ts.ko
export TSLIB_TSDEVICE=/dev/input/event0
export TSLIB_CALIBFILE=/etc/pointercal
export TSLIB_CONFFILE=/etc/ts.conf
export TSLIB_PLUGINDIR=/lib/ts
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0

��һ��ʹ����ҪУ׼
ts_calibrate
*/
int main(int argc, char **argv)
{
	PT_DispOpr ptDispOpr;

	//��ʼ��LCD
	DisplayInit();
	SelectDefaultDispDev("fb");
	
	ptDispOpr = GetDefaultDispDev();
	ptDispOpr->CleanScreen(0);

	//����5���Դ�
	AllocVideoMem(5);

	//��ʼ���¼�
	InputInit();

	//��ʼ�� ͼƬ����
	InitParse();
	
	//��ʼ��ҳ��
	InitPages();


	//���е�һ��ҳ��
	GetPage("main")->Run(NULL);

	return 0;
}
