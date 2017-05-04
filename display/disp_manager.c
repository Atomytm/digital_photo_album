#include <conf.h>
#include <string.h>
#include <disp_manager.h>
#include <stdio.h>
#include <stdlib.h>

static PT_DispOpr  g_ptDispOprHead;
static PT_DispOpr  g_ptDefaultDispOpr;
static PT_VideoMem g_ptVideoMemHead;

int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmpDispOpr;
	
	//���û�ж����
	if(! g_ptDispOprHead)
	{
		g_ptDispOprHead  = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmpDispOpr = g_ptDispOprHead;
		while(ptTmpDispOpr->ptNext)
		{
			ptTmpDispOpr = ptTmpDispOpr->ptNext;
		}
		ptTmpDispOpr->ptNext = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	return 0;
}

void ShowDispOpr(void)
{
	int i=0;
	PT_DispOpr ptTmpDispOpr = g_ptDispOprHead;
	while(ptTmpDispOpr)
	{
		printf("display:%d %s \n", i++, ptTmpDispOpr->name);
		ptTmpDispOpr = ptTmpDispOpr->ptNext;
	}
}

PT_DispOpr GetDispOpr(char *pcName)
{
	PT_DispOpr ptTmpDispOpr = g_ptDispOprHead;
	while(ptTmpDispOpr)
	{
		if(0 == strcmp(ptTmpDispOpr->name, pcName))
		{
			//����ʱ��ʼ��
			ptTmpDispOpr->DeviceInit();
			return ptTmpDispOpr;
		}
		ptTmpDispOpr = ptTmpDispOpr->ptNext;
	}
	return NULL;
}

int DisplayInit(void)
{
	int iError;
	iError = FBInit();
	return iError;
}

PT_DispOpr GetDefaultDispDev(void)
{
	return g_ptDefaultDispOpr;
}

void SelectDefaultDispDev(char *pcName)
{
	g_ptDefaultDispOpr = GetDispOpr(pcName);
}

int GetDispResolution(int *piXres, int *piYres, int *piBpp)
{
	if(g_ptDefaultDispOpr)
	{
		*piXres = g_ptDispOprHead->iXres;
		*piYres = g_ptDispOprHead->iYres;
		*piBpp  = g_ptDispOprHead->iBpp;
		return 0;
	}
	return -1;
}

int AllocVideoMem(int iNum)
{
	int i;
	int iXres = 0;
	int iYres = 0;
	int iBpp  = 0;
	int iVMSize;
	int iLineBytes;

	PT_VideoMem ptNew;
	
	//��ȡ��ʾ��С
	GetDispResolution(&iXres, &iYres, &iBpp);
	iLineBytes = iXres * iBpp / 8;
	iVMSize    = iLineBytes * iYres;

	//�Ȱ� �豸�� freebuffer ����
	ptNew = malloc(sizeof(T_VideoMem));
	if(NULL == ptNew)
	{
		return -1;
	}

	ptNew->iID = 0;
	ptNew->bDevFrameBuffer = 1;
	ptNew->ePicState       = VMS_FREE;
	ptNew->eVideoMemState  = PS_BLANK;
	ptNew->tPixelDatas.aucPixelDatas = g_ptDefaultDispOpr->pucDispMem;
	ptNew->tPixelDatas.iBpp          = iBpp;
	ptNew->tPixelDatas.iHeight       = iYres;
	ptNew->tPixelDatas.iWidth        = iXres;
	ptNew->tPixelDatas.iLineBytes    = iLineBytes;
	ptNew->tPixelDatas.iTotalBytes   = iVMSize;

	if(0 != iNum)
	{
		ptNew->eVideoMemState = VMS_USED_FOR_CUR;
	}
	//��������
	ptNew->ptNext = g_ptVideoMemHead;
	g_ptVideoMemHead = ptNew;

	//����������
	for(i=0; i<iNum; i++)
	{
		ptNew = malloc(sizeof(T_VideoMem) + iVMSize);
		if(NULL == ptNew)
		{
			return -1;
		}
		ptNew->iID = 0;
		ptNew->bDevFrameBuffer = 0;
		ptNew->ePicState       = VMS_FREE;
		ptNew->eVideoMemState  = PS_BLANK;
		ptNew->tPixelDatas.aucPixelDatas = (unsigned char *)(ptNew + 1);
		ptNew->tPixelDatas.iBpp          = iBpp;
		ptNew->tPixelDatas.iHeight       = iYres;
		ptNew->tPixelDatas.iWidth        = iXres;
		ptNew->tPixelDatas.iLineBytes    = iLineBytes;
		ptNew->tPixelDatas.iTotalBytes   = iVMSize;
		//��������
		ptNew->ptNext = g_ptVideoMemHead;
		g_ptVideoMemHead = ptNew;
	}
	return 0;
}

PT_VideoMem GetVideoMem(int iID, int bCur)
{
	PT_VideoMem ptTmp = g_ptVideoMemHead;
	//1,����ȡ�����е� ID ��ͬ��VMem
	while(ptTmp)
	{
		if(ptTmp->iID == iID && VMS_FREE == ptTmp->eVideoMemState)
		{
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}

	//2, ���ǰ�治�ɹ�, ȡ��һ�����еĲ�������û������(ptVideoMem->ePicState = PS_BLANK)��VideoMem
	ptTmp = g_ptVideoMemHead;
	while(ptTmp)
	{
		if(VMS_FREE == ptTmp->eVideoMemState && PS_BLANK == ptTmp->ePicState)
		{
			ptTmp->iID            = iID;
			ptTmp->ePicState      = PS_BLANK;
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	
	//3,ȡ��������е�
	ptTmp = g_ptVideoMemHead;
	while(ptTmp)
	{
		if(VMS_FREE == ptTmp->eVideoMemState)
		{
			ptTmp->iID            = iID;
			ptTmp->ePicState      = PS_BLANK;
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}

	/* 4. ���û�п��е�VideoMem����bCurΪ1, ��ȡ������һ��VideoMem(�������Ƿ����) */
	ptTmp  = g_ptVideoMemHead;
    if (bCur)
    {
    	ptTmp->iID            = iID;
		ptTmp->ePicState      = PS_BLANK;
		ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
		return ptTmp;
    }
	return NULL;
}

void PutVideoMem(PT_VideoMem ptVideoMem)
{
	ptVideoMem->eVideoMemState = VMS_FREE;
}

void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	if(! ptVideoMem->bDevFrameBuffer)
	{
		memcpy(g_ptDefaultDispOpr->pucDispMem, ptVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem->tPixelDatas.iLineBytes * ptVideoMem->tPixelDatas.iHeight);
	}
}

void ClearVideoMem(PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	unsigned char *pucVM;
	unsigned short *pwVM16bpp;
	unsigned int *pdwVM32bpp;
	unsigned short wColor16bpp; /* 565 */
	int i;
	int r,g,b;

	pucVM = ptVideoMem->tPixelDatas.aucPixelDatas;
	pwVM16bpp  = (unsigned short *)pucVM;
	pdwVM32bpp = (unsigned int *)pucVM;

	//תΪ565
	r = (dwColor >>(16+3)) & 0x1f;
	g = (dwColor >>(8+2))  & 0x3f;
	b = (dwColor >>3)  & 0x1f;
	wColor16bpp = (r<<11) | (g<<5) | (b<<0);

	switch(ptVideoMem->tPixelDatas.iBpp)
	{
		case 8:
		{
			memset(ptVideoMem->tPixelDatas.aucPixelDatas, dwColor, ptVideoMem->tPixelDatas.iTotalBytes);
		}
		break;
		case 16:
		{
			for(i=0; i<ptVideoMem->tPixelDatas.iTotalBytes; i+=2)
			{
				*pwVM16bpp = wColor16bpp;
				pwVM16bpp++;
			}
		}
		break;
		case 32:
		{
			for(i=0; i<ptVideoMem->tPixelDatas.iTotalBytes; i+=4)
			{
				*pdwVM32bpp = dwColor;
				pdwVM32bpp++;
			}
		}
		break;
	}
}

