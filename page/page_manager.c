#include <page_manager.h>
#include <disp_manager.h>
#include <conf.h>
#include <stdlib.h>
#include <string.h>

PT_PageAction g_ptPageActionHead;

//���ͼƬ
static T_Layout g_tMouseLayout = {
	0,0,0,0, "mouse.bmp"
};

//��������Ժ����������
static PT_PixelDatas g_ptMousePixelDatas;

//����ڸ�LCD����������
static PT_PixelDatas g_ptMouseMaskPixelDatas;

//����ƶ����ػ���ʾ �� ����ʾ ͬ�� ��������ػ� LCD
static PT_VideoMem g_ptSyncVideoMem;

//LCD
static PT_DispOpr g_ptDispOpr;

//��ʾ���
int ShowMouse(int x, int y)
{
	unsigned char *videoMem;
	unsigned char *mouseMem;
	int i,j;
	//ʹ�þ����Դ��ػ�LCD
	FlushVideoMemToDev(g_ptSyncVideoMem);
	if((x + g_ptMousePixelDatas->iWidth) <= g_ptDispOpr->iXres && (y + g_ptMousePixelDatas->iHeight) <= g_ptDispOpr->iYres)
	{
		//�㷨�Ͱ�ť��ɫȡ������
		mouseMem  = g_ptMousePixelDatas->aucPixelDatas;
		videoMem  = g_ptDispOpr->pucDispMem;
		videoMem += y * g_ptDispOpr->iLineWidth;
		videoMem += x * g_ptDispOpr->iBpp / 8;
		for(j = y; j < (y + g_ptMousePixelDatas->iHeight); j++)
		{
			for(i=0; i<g_ptMousePixelDatas->iWidth * g_ptMousePixelDatas->iBpp / 8; i++)
			{
				videoMem[i] = *mouseMem;
				mouseMem++;
			}
			videoMem += g_ptDispOpr->iLineWidth;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

//��ʼ�����
int InitMouse(void)
{
	//ԭͼ
	T_PixelDatas tOriginIconPixelDatas;
	//��Сͼ
 	T_PixelDatas tIconPixelDatas;
	int iXres, iYres, iBpp;
	int mouse_height;
	int mouse_width;

	//��ȡһ���Դ��������� �ػ� LCD ������
	g_ptSyncVideoMem = GetVideoMem(ID("sync"), 1);
	//��ȡĬ��dispopr
	g_ptDispOpr = GetDefaultDispDev();
	GetDispResolution(&iXres, &iYres, &iBpp);
	//�������ָ���С Ϊ �߶ȵ� 1/20
	mouse_height = iXres / 20;
	mouse_width  = mouse_height;
	g_tMouseLayout.iBottomRightX = mouse_width;
	g_tMouseLayout.iBottomRightY = mouse_height;

	//�������Ŵ�С
	tOriginIconPixelDatas.iBpp  = iBpp;
	tIconPixelDatas.iBpp        = iBpp;
	tIconPixelDatas.iHeight     = mouse_height;
	tIconPixelDatas.iWidth      = mouse_width;
	tIconPixelDatas.iLineBytes  = tIconPixelDatas.iWidth * tIconPixelDatas.iBpp / 8;
	tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;

	//�������ź� icon ͼƬ���Դ�
	tIconPixelDatas.aucPixelDatas = (unsigned char *)malloc(tIconPixelDatas.iTotalBytes);
	if(NULL == tIconPixelDatas.aucPixelDatas)
	{
		DEBUG_PRINTF("malloc tIconPixelDatas error \n");
		return -1;
	}
	//��ȡ mouse.bmp ������
	if(GetPixelDatasForIcon(g_tMouseLayout.strIconName, &tOriginIconPixelDatas))
	{
		free(tIconPixelDatas.aucPixelDatas);
		DEBUG_PRINTF("GetPixelDatasForIcon error \n");
		return -1;
	}
	//���ŵ�ָ����С
	PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);
	
	//�����ŵ�Сͼָ�����ȫ�ֱ���
	g_ptMousePixelDatas = &tIconPixelDatas;

	g_ptMousePixelDatas                = malloc(sizeof(T_PixelDatas) + tIconPixelDatas.iTotalBytes);
	g_ptMousePixelDatas->iBpp          = tIconPixelDatas.iBpp;
	g_ptMousePixelDatas->iHeight       = tIconPixelDatas.iHeight;
	g_ptMousePixelDatas->iWidth        = tIconPixelDatas.iWidth;
	g_ptMousePixelDatas->iLineBytes    = tIconPixelDatas.iLineBytes;
	g_ptMousePixelDatas->iTotalBytes   = tIconPixelDatas.iTotalBytes;
	g_ptMousePixelDatas->aucPixelDatas = (unsigned char *)(g_ptMousePixelDatas + 1);
	memcpy(g_ptMousePixelDatas->aucPixelDatas, tIconPixelDatas.aucPixelDatas, tIconPixelDatas.iTotalBytes);

	//����һ��������� �ڸ�LCD������ ���Դ�
	g_ptMouseMaskPixelDatas                = malloc(sizeof(T_PixelDatas) + tIconPixelDatas.iTotalBytes);
	g_ptMouseMaskPixelDatas->iBpp          = tIconPixelDatas.iBpp;
	g_ptMouseMaskPixelDatas->iHeight       = tIconPixelDatas.iHeight;
	g_ptMouseMaskPixelDatas->iWidth        = tIconPixelDatas.iWidth;
	g_ptMouseMaskPixelDatas->iLineBytes    = tIconPixelDatas.iLineBytes;
	g_ptMouseMaskPixelDatas->iTotalBytes   = tIconPixelDatas.iTotalBytes;
	g_ptMouseMaskPixelDatas->aucPixelDatas = (unsigned char *)(g_ptMouseMaskPixelDatas + 1);

	free(tIconPixelDatas.aucPixelDatas);
	free(tOriginIconPixelDatas.aucPixelDatas);
	return 0;
}

//���� ID Ψһ
int ID(char *pcName)
{
	return pcName[0] + pcName[1] + pcName[2] + pcName[3];
}

//����һҳ
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem)
{
	//��ȡIconͼƬ���ŵ����ʴ�С
	T_PixelDatas tOriginIconPixelDatas;
 	T_PixelDatas tIconPixelDatas;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
	/* �軭����: VideoMem�е�ҳ������δ���ɵ�����²�ִ��������� */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);
		tOriginIconPixelDatas.iBpp = ptPageLayout->iBpp;
		tIconPixelDatas.iBpp       = ptPageLayout->iBpp;

		//�������ź� icon ͼƬ���Դ�
		tIconPixelDatas.aucPixelDatas = (unsigned char *)malloc(ptPageLayout->iMaxTotalBytes);
		if(NULL == tIconPixelDatas.aucPixelDatas)
		{
			DEBUG_PRINTF("malloc tIconPixelDatas error \n");
			return -1;
		}
		
		//��ʾÿһ�� icon ͼƬ
		while(atLayout->strIconName)
		{
			//�õ� BMP ͼƬ����
			if(GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas))
			{
				free(tIconPixelDatas.aucPixelDatas);
				DEBUG_PRINTF("GetPixelDatasForIcon error \n");
				return -1;
			}
			
			//�������Ŵ�С
			tIconPixelDatas.iHeight     = atLayout->iBottomRightY - atLayout->iTopLeftY;
			tIconPixelDatas.iWidth      = atLayout->iBottomRightX - atLayout->iTopLeftX;
			tIconPixelDatas.iLineBytes  = tIconPixelDatas.iWidth * tIconPixelDatas.iBpp / 8;
			tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;

			//ִ������
			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);
			
			//д���Դ�
			PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);
			
			//�ͷ� BMP ͼƬ�����ڴ�
			free(tOriginIconPixelDatas.aucPixelDatas);
			atLayout++;
		}
		//�ͷ������Դ�
		free(tIconPixelDatas.aucPixelDatas);

		//�����Դ澵��
		memcpy(g_ptSyncVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem->tPixelDatas.iTotalBytes);
		 
		ptVideoMem->ePicState = PS_GENERATED;
	}
	return 0;
}

//ͨ�õĻ�ȡ�¼�
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	PT_Layout atLayout;
	int i;

	//����ԭʼ�Ĵ���������
	//inputevent ���Զ�����
	GetInputEvent(&tInputEvent);
	*ptInputEvent = tInputEvent;
	
	//�������켣
	ShowMouse(ptInputEvent->iX, ptInputEvent->iY);
	
	//�������ݣ�ȷ���ǵ���ĸ���ť��
	i = 0;
	atLayout = ptPageLayout->atLayout;
	while(atLayout[i].strIconName)
	{
		//�ж�����
		if( ptInputEvent->iX >= atLayout[i].iTopLeftX    && 
			ptInputEvent->iX <= atLayout[i].iBottomRightX && 
			ptInputEvent->iY >= atLayout[i].iTopLeftY     &&
			ptInputEvent->iY <= atLayout[i].iBottomRightY)
		{
			//���ذ�ť���±�
			return i;
		}
		i++;
	}
	return -1;
}

//ע�� Page
int RegisterPageAction(PT_PageAction ptPageAction)
{
	PT_PageAction ptTmp;
	if(! g_ptPageActionHead)
	{
		g_ptPageActionHead  = ptPageAction;
	}
	else
	{
		ptTmp = g_ptPageActionHead;
		while(ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext = ptPageAction;
	}
	ptPageAction->ptNext = NULL;
	//���û�����������¼���ʹ��ͨ�õ�
	if(! ptPageAction->GetInputEvent)
	{
		ptPageAction->GetInputEvent = GenericGetInputEvent;
	}
	return 0;
}

//����Page Name ���� Page
PT_PageAction GetPage(char *pcName)
{
 	PT_PageAction ptTmp = g_ptPageActionHead;
	while(ptTmp)
	{
		if(0 == strcmp(ptTmp->name, pcName))
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

//�� Page ��ʼ��
int InitPages(void)
{
	int iError;
	iError  = InitMouse();
	iError |= MainPageInit();
	return iError;
}
