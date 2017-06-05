#include <conf.h>
#include <render.h>
#include <page_manager.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <file.h>
#define min(a,b) (a<b ? a : b)

/* ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
 *   --------
 *   |  ͼ  |
 *   |  ��  |
 * ------------
 * |   ����   |
 * ------------
 */

//��ǰ�ж��ٸ�����Ŀ
static int g_iDirFilesNum;
//��������Ŀ�Ľṹ��
static PT_DirFiles *g_ptDirFiles;
//��ǰ��ʾ�ĸ�Ŀ¼�µ�����Ŀ
static char g_acDirPath[256] = "/";

//iconͼ��
static T_Layout g_atBrowsePageIconsLayout[] = {
	{0, 0, 0, 0, ICON_DIR"up.bmp"},
	{0, 0, 0, 0, ICON_DIR"select.bmp"},
	{0, 0, 0, 0, ICON_DIR"pre_page.bmp"},
	{0, 0, 0, 0, ICON_DIR"next_page.bmp"},
	{0, 0, 0, 0, ICON_DIR"video_view.bmp"},    /* ʵʱ�鿴 */
	{0, 0, 0, 0, ICON_DIR"video_camera.bmp"},  /* ���� */
	{0, 0, 0, 0, NULL},
};

//Browse Page layout
static T_PageLayout g_tBrowsePageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atBrowsePageIconsLayout,
};

//�ļ� layout
static T_PageLayout g_tBrowseFilesLayout = {
	.iMaxTotalBytes = 0,
};

static void BrowsePageRun(PT_PageParams ptPageParams);

static T_PageAction g_tBrowsePageAction = {
	.name = "browse",
	.Run  = BrowsePageRun,
};

//���� Page Layout λ��
static void CalcBrowsePageLayout(PT_PageLayout ptPageLayout)
{
	int iXres;
	int iYres;
	int iBpp;
	int iIconWidth;
	int iIconHeight;
	int i;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
	//����LCD����Ϣ
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	// ��ť�ĸ߶���ռLCD �ߵ�2/10 һ�з�6����ť
	iIconHeight = iYres * 2 / 10;
	iIconWidth  = iXres / 6;
	
	ptPageLayout->iMaxTotalBytes = iIconWidth * iIconHeight * iBpp;

	//����ÿ���Ĵ�С
	i = 0;
	while(atLayout[i].strIconName)
	{
		atLayout[i].iTopLeftX     = i * iIconWidth;
		atLayout[i].iBottomRightX = atLayout[i].iTopLeftX + iIconWidth;
		atLayout[i].iTopLeftY     = 0;
		atLayout[i].iBottomRightY = atLayout[i].iTopLeftY + iIconHeight;
		
		i++;
	}
	
}

static int GenerateIcon(PT_VideoMem ptVideoMem, PT_Layout ptLayout, char *pcFileName)
{
	int iXres;
	int iYres;
	int iBpp;
	T_PixelDatas tPixelDatas;
	T_PixelDatas tFontPixelDatas;
	
	//����LCD����Ϣ
	GetDispResolution(&iXres, &iYres, &iBpp);
	
	tPixelDatas.iHeight = DIR_FILE_ICON_HEIGHT;
	tPixelDatas.iWidth  = DIR_FILE_ICON_WIDTH;
	tPixelDatas.iBpp    = iBpp;
	tPixelDatas.iLineBytes  = tPixelDatas.iWidth * tPixelDatas.iBpp / 8;
	tPixelDatas.iTotalBytes = tPixelDatas.iLineBytes * tPixelDatas.iHeight;
	
	tPixelDatas.aucPixelDatas = malloc(tPixelDatas.iTotalBytes);
	if(! tPixelDatas.aucPixelDatas)
	{
		DEBUG_PRINTF("malloc error \n");
		return -1;
	}
	//��Ϊ����ɫ
	ClearPixelDatasMem(&tPixelDatas, COLOR_BACKGROUND);

	tFontPixelDatas.iHeight = DIR_FILE_NAME_HEIGHT;
	tFontPixelDatas.iWidth  = DIR_FILE_NAME_WIDTH;
	tFontPixelDatas.iBpp    = iBpp;
	tFontPixelDatas.iLineBytes  = tFontPixelDatas.iWidth * tFontPixelDatas.iBpp / 8;
	tFontPixelDatas.iTotalBytes = tFontPixelDatas.iLineBytes * tFontPixelDatas.iHeight;
	

	tFontPixelDatas.aucPixelDatas = malloc(tFontPixelDatas.iTotalBytes);
	if(! tFontPixelDatas.aucPixelDatas)
	{
		DEBUG_PRINTF("malloc error \n");
		return -1;
	}
	//��Ϊ����ɫ
	ClearPixelDatasMem(&tFontPixelDatas, COLOR_BACKGROUND);
	
	//��ȡicon �Դ�
	if(GetFileICON(&tPixelDatas, ptLayout->strIconName))
	{
		DEBUG_PRINTF("GetFileICON error \n");
		return -1;
	}
	//�ϲ������Դ�
	if(MergePixelDatasToVideoMem(ptLayout->iTopLeftX, ptLayout->iTopLeftY, &tPixelDatas, ptVideoMem))
	{
		DEBUG_PRINTF("MergePixelDatasToVideoMem error \n");
		return -1;
	}
	//�����ļ��� ʹ�� freetype �� ���ֿ� �� ascii ��������
	if(GetPixelDatasForFreetype((unsigned char *)pcFileName, &tFontPixelDatas))
	{
		DEBUG_PRINTF("GetPixelDatasForFreetype error \n");
		return -1;
	}

	//�ϲ������Դ�
	if(MergePixelDatasToVideoMem(ptLayout->iTopLeftX, ptLayout->iTopLeftY + DIR_FILE_ICON_HEIGHT , &tFontPixelDatas, ptVideoMem))
	{
		DEBUG_PRINTF("MergePixelDatasToVideoMem error \n");
		return -1;
	}

	//�ͷ��ڴ�
	free(tPixelDatas.aucPixelDatas);
	free(tFontPixelDatas.aucPixelDatas);
	return 0;
}

static int GenerateBrowseDirIcon(PT_VideoMem ptVideoMem)
{
	int iErr = 0;
	PT_DirFiles ptDirFiles;
	T_Layout *atDirLayout;
	int dirIconX = 0;
	int dirIconY = 0;
	int iXres;
	int iYres;
	int iBpp;
	int iNum;
	int i;
	int margin = 5;

	//����LCD����Ϣ
	GetDispResolution(&iXres, &iYres, &iBpp);
	//��ȡ g_acDirPath �µ������ļ��б�
	iErr = GetDirContents(g_acDirPath, &g_ptDirFiles, &g_iDirFilesNum);
	if(iErr)
	{
		DEBUG_PRINTF("GetDirContents Err %s \n", g_acDirPath);
		return iErr;
	}
	//���������ʾ���ٸ�ͼ�� margin:5px    ��ȥ �ϲ���ťռ�� 2/10
	iNum = (iXres / (DIR_FILE_ALL_WIDTH + margin*2)) * ((iYres * 8 / 10) / (DIR_FILE_ALL_HEIGHT + margin*2));
	
	//ʵ���ļ����� ����ʾ������ ȡС��
	iNum = min(iNum, g_iDirFilesNum);

	//����layout �ڴ�
	atDirLayout = malloc(sizeof(T_Layout) * iNum);
	g_tBrowseFilesLayout.atLayout = atDirLayout;
	
	//���� ICON ��ʾ
	dirIconX   = margin*2;
	dirIconY   = iYres * 2 / 10 + margin;
	while(i < iNum)
	{
		//����ͼ��Ҫ��ʾ��λ�ü��Դ�
		atDirLayout[i].iTopLeftX     = dirIconX;
		atDirLayout[i].iTopLeftY     = dirIconY;
		atDirLayout[i].iBottomRightX = atDirLayout[i].iTopLeftX + DIR_FILE_ALL_WIDTH;
		atDirLayout[i].iBottomRightY = atDirLayout[i].iTopLeftY + DIR_FILE_ALL_HEIGHT;
		atDirLayout[i].strIconName   = malloc(256);
		if(! atDirLayout[i].strIconName)
		{
			DEBUG_PRINTF("malloc error \n");
			return -1;
		}
		ptDirFiles = g_ptDirFiles[i];
		if(GetFileIconName(ptDirFiles, atDirLayout[i].strIconName))
		{
			DEBUG_PRINTF("GetFileIconName Err \n");
			return -1;
		}
		//����ͼ��
		if(GenerateIcon(ptVideoMem, &atDirLayout[i], ptDirFiles->strName))
		{
			DEBUG_PRINTF("GenerateIcon Err \n");
			return -1;
		}
		//�����趨λ��
		dirIconX += DIR_FILE_ALL_WIDTH + margin;
		//����Ҫ���� ��X �Ƿ񳬳�
		if((dirIconX + DIR_FILE_ALL_WIDTH) >= iXres)
		{
			dirIconX = margin*2;
			dirIconY += DIR_FILE_ALL_HEIGHT + margin * 2;
		}
		
		i++;
	}
	
	return iErr;
}

static void ShowBrowsePage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
	//���λ��Ϊ0˵��δ����λ��
	if(0 == atLayout[0].iTopLeftX && 0 == atLayout[0].iBottomRightX)
	{
		CalcBrowsePageLayout(ptPageLayout);
	}
	
	//��ȡ�Դ�
	ptVideoMem = GetVideoMem(ID("browse"), 1);
	if(! ptVideoMem)
	{
		DEBUG_PRINTF("cat't get video mem \n");
		return ;
	}
	
	//����
	if(GeneratePage(ptPageLayout, ptVideoMem) || GenerateBrowseDirIcon(ptVideoMem))
	{
		DEBUG_PRINTF("GeneratePage error \n");
		return ;
	}
	
	//ˢ��LCD
	//FlushVideoMemToDev(ptVideoMem);
	//��Ϊ�ػ�ҳ��������ػ���꣬����Ҫ����ʾ�����طŵ� �����Դ���
	FlushVideoMemToDevSync(ptVideoMem);

	//��ʾ�����ʷλ��
	ShowHistoryMouse();
	
	//�ͷ��Դ�
	PutVideoMem(ptVideoMem);
	
}

static void BrowsePageRun(PT_PageParams ptPageParams)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;/* ���µ�״̬ */
	int iIndexPressed = 0; /* �����±� */
	
	//��ʾ icons
	ShowBrowsePage(&g_tBrowsePageLayout);

	//�����¼�
	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		iIndex = GenericGetInputEvent(&g_tBrowsePageLayout, &tInputEvent);

		/**
		 *�жϰ����¼��㷨
		 *
		 */
		//�ɿ�״̬ ѹ��ֵ Ϊ 0
		if (0 == tInputEvent.iPressure)
		{
			if(bPressed)
			{
				/* �����а�ť������ */
				ReleaseButton(&g_atBrowsePageIconsLayout[iIndexPressed]);
				bPressed = 0;
				//������º��ɿ���ͬһ������
				if(iIndexPressed == iIndex)
				{
					switch(iIndex)
					{
						case 0 : 
						{
						}
						break;

						case 1 : 
						{
							
						}
						break;

						case 2 :
						{
						}
						break;
						case 3 :
						{
						}
						case 4 :
						{
						
						}
						break;
					}
				}
				iIndexPressed = -1;
			}
		}
		else
		{
			//�������ֵ��Ч
			if(-1 != iIndex)
			{
				if(0 == bPressed)
				{
					bPressed      = 1;
					iIndexPressed = iIndex;
					PressButton(&g_atBrowsePageIconsLayout[iIndexPressed]);
				}
			}
		}
	}
}

int BroserPageInit(void)
{
	return RegisterPageAction(&g_tBrowsePageAction);
}



