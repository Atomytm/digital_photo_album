#ifndef __INPUT_MANAGER_H
#define __INPUT_MANAGER_H
#include <sys/time.h>
#include <pthread.h>

/* �����¼���� */
#define INPUT_TYPE_STDIN        0
#define INPUT_TYPE_TOUCHSCREEN  1
#define INPUT_TYPE_MOUSE         2

/* �����¼� */
#define INPUT_VALUE_UP          0   
#define INPUT_VALUE_DOWN        1
#define INPUT_VALUE_EXIT        2
#define INPUT_VALUE_UNKNOWN     -1

typedef struct InputEvent {
	struct timeval tTime; /* ʱ�� */
	int iType;             /* stdin, touchsceen */
	int iX;                /* ���X */
	int iY;                /* ���Y */
	int iKey;              /* ����ֵ */
	int iPressure;        /* ѹ��ֵ */
}T_InputEvent, *PT_InputEvent;

typedef struct InputOpr
{
	char *name;             /* ����ģ������� */
	pthread_t tTreadID;     /* ���߳�ID */
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	struct InputOpr *ptNext;
} T_InputOpr, *PT_InputOpr;

int InputInit(void);
int RegisterInputOpr(PT_InputOpr ptInputOpr);
void ShowInputOpr(void);
int GetInputEvent(PT_InputEvent ptInputEvent);
int StdInit(void);
int TouchScreenInit(void);
int MouseInit(void);
#endif


