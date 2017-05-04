#include <conf.h>
#include <draw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <string.h>

//��С˵ fd
static int g_iFdTextFile;
//С˵mmap��ʼ��ַ
static unsigned char *g_pucTextFileMem;
//С˵mmap������ַ
static unsigned char *g_pucTextFileMemEnd;
//�ļ�������
static PT_EncodingOpr g_ptEncodingOprForFile;
//С˵��ʵ���뿪ʼ��ַ
static unsigned char *g_pucLcdFirstPosAtFile;
//��ҳ����
static PT_PageDesc g_ptPages = NULL;
static PT_PageDesc g_ptCurPage = NULL;
//lcd��ʾ
static PT_DispOpr g_ptDispOpr;
//�����С
static int g_dwFontSize;
//��ǰҳ�����ı����ַ
static unsigned char *g_pucLcdNextPosAtFile;


int SetFontsDetail(char *pcHZKFile, char *pcFileFreetype, unsigned dwFontSize)
{
	int iError = 0;
	//��ʼ��font
	PT_FontOpr ptFontOpr = g_ptEncodingOprForFile->ptFontOprSupportedHead;
	PT_FontOpr pttmpFontOpr; 

	while(ptFontOpr)
	{
		if(0 == strcmp(ptFontOpr->name, "ascii"))
		{
			iError = ptFontOpr->FontInit(NULL, dwFontSize);
		}
		else if(0 == strcmp(ptFontOpr->name, "gbk"))
		{
			iError = ptFontOpr->FontInit(pcHZKFile, dwFontSize);
		}
		else if(0 == strcmp(ptFontOpr->name, "freetype"))
		{
			iError = ptFontOpr->FontInit(pcFileFreetype, dwFontSize);
		}
		//���� free �Ժ��Ҳ��� ��һ��
		pttmpFontOpr = ptFontOpr->ptNext;
		if(iError)
		{
			DelFontOprFrmEncoding(g_ptEncodingOprForFile, ptFontOpr);
		}
		ptFontOpr = pttmpFontOpr;
	}
	g_dwFontSize = dwFontSize;
	return 0;
}

static void DrawFontBitMap(PT_FontBitMap pt_FontBitMap)
{
	//��ģ
 	unsigned char *buf;
	int i;
	int x;
	int y;
	
	for(y = pt_FontBitMap->iYTop; y<pt_FontBitMap->iYMax; y++)
	{
		//����freetype buffer �Ŀ�ʼλ�ò�һ���� 1 ����һ����Ҫ2��3������ʾ
		buf = (y - pt_FontBitMap->iYTop) * pt_FontBitMap->iPitch + pt_FontBitMap->pucBuffer;
		for(x = pt_FontBitMap->iXLeft; x<pt_FontBitMap->iXMax; x+=8)
		{
			for(i=0; i<8; i++)
			{
				if(*buf & (1<<(7-i)))
				{
					g_ptDispOpr->ShowPixel(x+i, y, COLOR_FOREGROUND);
				}
			}
			buf++;
		}
	}
}

static int IncLcdY(int iY)
{
	if (iY + g_dwFontSize < g_ptDispOpr->iYres)
		return (iY + g_dwFontSize);
	else
		return 0;
}

static int RelocateFontPos(PT_FontBitMap ptFontBitMap)
{
	int iLcdY;
	int iDeltaX;
	int iDeltaY;
	
	if (ptFontBitMap->iYMax > g_ptDispOpr->iYres)
	{
		/* ��ҳ�� */
		return -1;
	}
	/* ����ұ���ʾ���¾ͻ��� */
	if(ptFontBitMap->iXMax > g_ptDispOpr->iXres)
	{
		iLcdY = IncLcdY(ptFontBitMap->iCurOriginY);
		/* ��ҳ�� */
		if (0 == iLcdY)
		{
			return -1;
		}
		/* û��ҳ */
		iDeltaX = 0 - ptFontBitMap->iCurOriginX;
		iDeltaY = iLcdY - ptFontBitMap->iCurOriginY;

		ptFontBitMap->iCurOriginX  += iDeltaX;
		ptFontBitMap->iCurOriginY  += iDeltaY;

		ptFontBitMap->iNextOriginX += iDeltaX;
		ptFontBitMap->iNextOriginY += iDeltaY;

		ptFontBitMap->iXLeft += iDeltaX;
		ptFontBitMap->iXMax  += iDeltaX;

		ptFontBitMap->iYTop  += iDeltaY;
		ptFontBitMap->iYMax  += iDeltaY;;
	}
	return 0;
}

static int ShowPage(unsigned char *pucTextFileMemCurPos)
{
	int iError;
	unsigned char *pucBuffer = pucTextFileMemCurPos;
	unsigned int textCode;
	int len; 
 	PT_FontOpr ptFontOpr;
	T_FontBitMap tFontBitMap;
	//���ó�ʼ��ʱ�濪ʼλ��
	tFontBitMap.iCurOriginX = 0;
	tFontBitMap.iCurOriginY = g_dwFontSize;
	/* �������� */
	g_ptDispOpr->CleanScreen(COLOR_BACKGROUND);
	//����������Ļ
	while(1)
	{
		len = g_ptEncodingOprForFile->GetCodeFrmBuf(pucBuffer, g_pucTextFileMemEnd, &textCode);
		if(0 == len)
		{
			DEBUG_PRINTF("read file end \n");
			break;
		}
		pucBuffer += len;
		
		//������
		if('\n' == textCode)
		{
			tFontBitMap.iCurOriginX = 0;
			tFontBitMap.iCurOriginY = IncLcdY(tFontBitMap.iCurOriginY);
			if (0 == tFontBitMap.iCurOriginY)
			{
				/* ��ʾ�굱ǰһ���� */
				g_pucLcdNextPosAtFile = pucBuffer;
				return 0;
			}
			else
			{
				continue;
			};
		}
		//\r������
		if('\r' == textCode)
		{
			continue;
		}
		//tab�ո�3��
		if('\t' == textCode)
		{
			textCode = ' ';
		}
		ptFontOpr = g_ptEncodingOprForFile->ptFontOprSupportedHead;
		while(ptFontOpr)
		{
			iError = ptFontOpr->GetFontBitMap(textCode, &tFontBitMap);
			//�����ȡ�� bitmap �ɹ� �ͷ���
			if(0 == iError)
			{
				if (RelocateFontPos(&tFontBitMap))
				{
					/* ʣ�µ�LCD�ռ䲻��������ʾ����ַ� */
					return 0;
				}
				DrawFontBitMap(&tFontBitMap);
				//������һ��ԭ��
				tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
				tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
				//ÿ�μ�¼������ַ ���Զ��˳�ʱ �ͱ��� �����һ��λ��
				g_pucLcdNextPosAtFile = pucBuffer;
				break;
			}
			//�����֧�־͵�����һ��
			ptFontOpr = ptFontOpr->ptNext;
		}
	}
	return 0;
}

void RecordPage(PT_PageDesc pt_PageDescNew)
{
	if(! g_ptPages)
	{
		g_ptPages = pt_PageDescNew;
		g_ptPages->ptNextPage = NULL;
		g_ptPages->ptPrePage  = NULL;
	}
	else
	{
		while(g_ptPages->ptNextPage)
		{
			g_ptPages = g_ptPages->ptNextPage;
		}
		pt_PageDescNew->ptPrePage = g_ptPages;
		g_ptPages->ptNextPage = pt_PageDescNew;
	}
}

int ShowFirstPage(void)
{
	return ShowNextPage();
}

int ShowNextPage(void)
{
	int iError;
	PT_PageDesc ptPage;
	unsigned char *pucTextFileMemCurPos;

	if (g_ptCurPage)
	{
		pucTextFileMemCurPos = g_ptCurPage->pucLcdNextPageFirstPosAtFile;
	}
	else
	{
		pucTextFileMemCurPos = g_pucLcdFirstPosAtFile;
	}
	ShowPage(pucTextFileMemCurPos);

	//���������ҳ�Ͳ���Ҫ�ڴ�����
	if(g_ptPages && g_ptCurPage->ptNextPage)
	{
		g_ptCurPage = g_ptCurPage->ptNextPage;
		return 0;
	}
	
	ptPage = malloc(sizeof(T_PageDesc));
	if(!ptPage)
	{
		DEBUG_PRINTF("malloc pagedesc error \n");
		iError = -1;
	}
	else
	{
		ptPage->pucLcdFirstPosAtFile = pucTextFileMemCurPos;
		ptPage->pucLcdNextPageFirstPosAtFile = g_pucLcdNextPosAtFile;
		ptPage->ptNextPage = NULL;
		ptPage->ptPrePage  = NULL;
		RecordPage(ptPage);
		g_ptCurPage = ptPage;
	}
	return iError;
}

int ShowPrePage(void)
{
	int iError;
	if(! g_ptCurPage || ! g_ptCurPage->ptPrePage)
	{
		iError = -1;
	}
	else
	{
		g_ptCurPage = g_ptCurPage->ptPrePage;
		ShowPage(g_ptCurPage->pucLcdFirstPosAtFile);
		iError = 0;
	}
	return iError;
}

int SelectAndInitDisplay(char *pcName)
{
	g_ptDispOpr = GetDispOpr(pcName);
	if(NULL == g_ptDispOpr)
	{
		return -1;
	}
	g_ptDispOpr->CleanScreen(COLOR_BACKGROUND);
	return 0;
}

int OpenTextFile(char * pcFileName)
{
	struct stat tStat;
	g_iFdTextFile = open(pcFileName, O_RDONLY);
	if(0 > g_iFdTextFile)
	{
		DEBUG_PRINTF("cat't open %s \n", pcFileName);
		return -1;
	}
	//��ȡ�ļ���С
	if(fstat(g_iFdTextFile, &tStat))
	{
		DEBUG_PRINTF("cat't get fstat \n");
		return -1;
	}
	//mmap
	g_pucTextFileMem = (unsigned char *)mmap(NULL, tStat.st_size, PROT_READ, MAP_SHARED, g_iFdTextFile, 0);
	if(MAP_FAILED == g_pucLcdFirstPosAtFile)
	{
		DEBUG_PRINTF("cat't file mmap \n");
		return -1;
	}
	//�����ļ�����
	g_pucTextFileMemEnd = g_pucTextFileMem + tStat.st_size;

	g_ptEncodingOprForFile = SelectEncodingOprForFile(g_pucTextFileMem);
	if(g_ptEncodingOprForFile)
	{
		//�ƶ�ָ����תͷ��
		g_pucLcdFirstPosAtFile = g_pucTextFileMem + g_ptEncodingOprForFile->iHeadLen;
		return 0;
	}
	else
	{
		DEBUG_PRINTF("cat't SelectEncodingOprForFile \n");
		return -1;
	} 
	return 0;
}
