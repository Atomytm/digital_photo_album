#include <input_manager.h>
#include <disp_manager.h>
#include <conf.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <linux/input.h>

#define MOUSE_DEV "/dev/input/mouse0"

static int postion_x;
static int postion_y;
//�޶���� X  Y 	
int postion_max_x;	
int postion_max_y;
//�ƶ����
int mouse_move_step;
static PT_DispOpr ptDispOpr;
static int mouse_fd;


static int MouseDeviceInit(void);
static int MouseDeviceExit(void);
static int MouseGetInputEvent(PT_InputEvent ptInputEvent);
static T_InputOpr g_tMouseinInputOpr = {
	.name          = "mouse",
	.DeviceInit    = MouseDeviceInit,
	.DeviceExit    = MouseDeviceExit,
	.GetInputEvent = MouseGetInputEvent,
};

static int MouseDeviceInit(void)
{
	ptDispOpr = GetDefaultDispDev();
	if(! ptDispOpr)
	{
		DEBUG_PRINTF("mouse cat't get lcd params \n");
		return -1;
	}
	//������豸
	mouse_fd = open(MOUSE_DEV, O_RDONLY);
	if(-1 == mouse_fd)
	{
		DEBUG_PRINTF("mouse cat't open %s \n", MOUSE_DEV);
		return -1;
	}
	//����X Y Ϊ LCD �ĸ߶� ���
	postion_max_x = ptDispOpr->iXres;
	postion_max_y = ptDispOpr->iYres;
	//Ĭ�ϳ�����LCD���м�
	postion_x      = postion_max_x / 2;
	postion_y      = postion_y / 2;
	//���ÿ���ƶ���Ļ��1/10 ����
	mouse_move_step = postion_max_x / 50;
	return 0;
}

static int MouseDeviceExit(void)
{
	close(mouse_fd);
	return 0;
}

static int MouseGetInputEvent(PT_InputEvent ptInputEvent)
{
    //��������¼� Ŀǰֻ���� ��־ �ƶ�
    unsigned char buf[3];
	ptInputEvent->iType = INPUT_TYPE_MOUSE;
	gettimeofday(&ptInputEvent->tTime, NULL); 
	if(read(mouse_fd, buf, sizeof(buf)))
	{
		/**
		 * ԭ�� ����Ϊ0 ʱ˵��������ƶ��������Է��֣�ֵΪ 12 �� 255 254 ��������ȡ��10С���Ǽ���
		 */
		//X �����ƶ�ʱ��� û����
		if(0 < buf[1])
		{
			postion_x += (10 > buf[1]) ? mouse_move_step : (0 - mouse_move_step);
		}
		//Y �����ƶ�ʱ��С ��Ҫ��ת
		if(0 < buf[2])
		{
			postion_y += (10 > buf[2]) ? (0 - mouse_move_step) : mouse_move_step;
		}
		postion_x = (1 > postion_x) ? 0 : postion_x;
		postion_y = (1 > postion_y) ? 0 : postion_y;
		postion_x = (postion_max_x < postion_x) ? postion_max_x : postion_x;
		postion_y = (postion_max_y < postion_y) ? postion_max_y : postion_y;
			
		ptInputEvent->iX = postion_x;
		ptInputEvent->iY = postion_y;
		//�������
		if(0x9 == buf[0])
		{
			ptInputEvent->iPressure = 1;
		}
		//�ɿ�����
		if(0x8 == buf[0])
		{
			ptInputEvent->iPressure = 0;
		}
		return 0;
	}
	return -1;
}

int MouseInit(void)
{
	return RegisterInputOpr(&g_tMouseinInputOpr);
}


