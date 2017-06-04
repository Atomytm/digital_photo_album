#include <input_manager.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

static PT_InputOpr g_ptInputOprHead;

//ȫ�ֱ���ͨ�����������
static T_InputEvent g_tInputEvent;

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;

int InputInit(void)
{
	int iError = 0;
	
	//iError |= StdInit();
	iError |= TouchScreenInit();
	iError |= MouseInit();
	return iError;
}

int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* ���� */
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);	

	/* �����Ѻ�,�������� */
	*ptInputEvent = g_tInputEvent;
	pthread_mutex_unlock(&g_tMutex);
	return 0;
}

static void *InputEventTreadFunction(void *pVoid)
{
	T_InputEvent tInputEvent;
	/* ���庯��ָ�� */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while (1)
	{
		//��Ϊ����������û������ʱ������
		if(0 == GetInputEvent(&tInputEvent))
		{
			//������ʱ����
			pthread_mutex_lock(&g_tMutex);
			g_tInputEvent = tInputEvent;
			/*  �������߳� */
			pthread_cond_signal(&g_tConVar);
			pthread_mutex_unlock(&g_tMutex);
		}
	}
}

int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTmpInputOpr;
	if(! g_ptInputOprHead)
	{
		g_ptInputOprHead  = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTmpInputOpr = g_ptInputOprHead;
		while(ptTmpInputOpr->ptNext)
		{
			ptTmpInputOpr = ptTmpInputOpr->ptNext;
		}
		ptTmpInputOpr->ptNext = ptInputOpr;
		ptInputOpr->ptNext   = NULL;
	}
	if(0 == ptInputOpr->DeviceInit())
	{
		//��ʼ���ɹ��������߳� �������GetInputEvent ������
		pthread_create(&ptInputOpr->tTreadID, NULL, InputEventTreadFunction, ptInputOpr->GetInputEvent);
	}
	return 0;
}

void ShowInputOpr(void)
{
	int i=0;
	PT_InputOpr ptTmpInputOpr = g_ptInputOprHead;
	while(ptTmpInputOpr)
	{
		printf("display:%d %s \n", i++, ptTmpInputOpr->name);
		ptTmpInputOpr = ptTmpInputOpr->ptNext;
	}
}
