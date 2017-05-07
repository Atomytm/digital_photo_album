#include <fonts_manager.h>
#include <stdio.h>
#include <string.h>

static PT_FontOpr g_ptFontOprHead;

int RegisterFontOpr(PT_FontOpr ptFontOpr)
{
	PT_FontOpr pt_tempFontOpr;
	//���Ϊ�վ͸�ֵ
	if(! g_ptFontOprHead)
	{
		g_ptFontOprHead = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	else
	{
		pt_tempFontOpr = g_ptFontOprHead;
		//���ҵ����һ��
		while(pt_tempFontOpr->ptNext)
		{
			pt_tempFontOpr = pt_tempFontOpr->ptNext;
		}
		pt_tempFontOpr->ptNext = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	return 0;
}

void ShowFontOpr(void)
{
	int i = 0;
	PT_FontOpr pt_tempFontOpr = g_ptFontOprHead;
	while(pt_tempFontOpr)
	{
		DEBUG_PRINTF("fonts:%d %s \n", i++, pt_tempFontOpr->name);
		pt_tempFontOpr = pt_tempFontOpr->ptNext;
	}
}

int InitFonts(void)
{
	int iError = 0;
	iError |= ASCIIInit();
	iError |= GBKInit();
	iError |= FreeTypeInit();	
	return iError;
}

PT_FontOpr GetFontOpr(char *pcName)
{
	PT_FontOpr pt_tempFontOpr = g_ptFontOprHead;
	while(pt_tempFontOpr)
	{
		
		if(0 == strcmp(pcName, pt_tempFontOpr->name))
		{
			//��ȡ��֮ǰ��ʼ�� �������������ʼ�� ��Ϊ��Ҫ������
			//pt_tempFontOpr->FontInit();
			return pt_tempFontOpr;
		}
		pt_tempFontOpr = pt_tempFontOpr->ptNext;
	}
	return NULL;
}
