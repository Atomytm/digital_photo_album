#ifndef __PAGE_MANAGER_H
#define __PAGE_MANAGER_H
#include <input_manager.h>
#include <disp_manager.h>
#include <render.h>

/* ҳ����� */
typedef struct PageParams{
	int iPageID; /* ҳ���ID */
	char strCurPictureFile[256]; /* Ҫ����ĵ�һ��ͼƬ�ļ� */
}T_PageParams, *PT_PageParams;

/* ҳ�沽�� */
typedef struct PageLayout{
	int iTopLeftX;
	int iTopLeftY;
	int iBottomRightX;
	int iBottomRightY;
	int iBpp;
	int iMaxTotalBytes;
	PT_Layout atLayout;/* icon���� */
}T_PageLayout, *PT_PageLayout;

/* ҳ�� */
typedef struct PageAction{
	char *name;
	void (*Run)(PT_PageParams ptPageParams);
	int (*GetInputEvent)(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);
	struct PageAction *ptNext;
}T_PageAction, *PT_PageAction;

/* ҳ��������Ϣ */
typedef struct PageCfg{
	int intvalSceond;
	char strSelectDir[256];
}T_PageCfg, *PT_PageCfg;

//���� ID Ψһ
int ID(char *pcName);
//��ҳ Page
int MainPageInit(void);
//���ò����� Page
int SetingPageInit(void);
//��ʱ���ŵ� Page
int IntevalPageInit(void);
//����ļ��� Page
int BroserPageInit(void);
//�Զ����ŵ� Page
int AutoPageInit(void);
//�����Ʋ˵��� Page
int ManualPageInit(void);
//ע�� Page
int RegisterPageAction(PT_PageAction ptPageAction);
//�� Page ��ʼ��
int InitPages(void);
//����һҳ
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem);
//ͨ�õĻ�ȡ�¼�
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);
//����Page Name ���� Page
PT_PageAction GetPage(char *pcName);
//�Ƚ�2�� timeval ���ĺ���
int TimeMSBetween(struct timeval tTimeStart, struct timeval tTimeEnd);
//�õ�������Ϣ
void GetPageCfg(PT_PageCfg ptPagecfg);
//��ʼ�����
int InitMouse(void);
//��ʾ���
int ShowMouse(int x, int y);
//�ػ���ʷ���
int ShowHistoryMouse(void);
#endif

