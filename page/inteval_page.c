#include <conf.h>
#include <render.h>
#include <page_manager.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int g_iInteval = 1;
static int GenerateInterval(PT_VideoMem ptVideoMem);
static int GenerateInterval(PT_VideoMem ptVideoMem);

//iconͼ��
static T_Layout g_atIntevalPageIconsLayout[] = {
	{0, 0, 0, 0, "time_inc.bmp"},
	{0, 0, 0, 0, "time.bmp"},
	{0, 0, 0, 0, "time_add.bmp"},
	{0, 0, 0, 0, "ok.bmp"},
	{0, 0, 0, 0, "cancel.bmp"},
	{0, 0, 0, 0, NULL},
};

//Inteval Page layout
static T_PageLayout g_tIntevalPageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atIntevalPageIconsLayout,
};

static int ReGenerateInterval()
{
	PT_VideoMem ptVideoMem = GetVideoMem(ID("sync"), 1);
	return GenerateInterval(ptVideoMem);
}

static int GenerateInterval(PT_VideoMem ptVideoMem)
{
	int iXres;
	int iYres;
	int iBpp;
	char strNumber[3];
	T_PixelDatas tPixelDatas;
	snprintf(strNumber, 3, "%02d", g_iInteval);

	//����Ҫ��ʾ��λ�ü���С
	tPixelDatas.iWidth  = g_atIntevalPageIconsLayout[1].iBottomRightX - g_atIntevalPageIconsLayout[1].iTopLeftX;
	tPixelDatas.iHeight = g_atIntevalPageIconsLayout[1].iBottomRightY - g_atIntevalPageIconsLayout[1].iTopLeftY;
	tPixelDatas.iWidth  = tPixelDatas.iWidth  /2;
	tPixelDatas.iHeight = tPixelDatas.iHeight /2;
	
	//����LCD����Ϣ
	GetDispResolution(&iXres, &iYres, &iBpp);
	tPixelDatas.iBpp = iBpp;
	tPixelDatas.iLineBytes = tPixelDatas.iWidth * tPixelDatas.iBpp / 8;
	tPixelDatas.iTotalBytes = tPixelDatas.iLineBytes * tPixelDatas.iHeight;
	//������ʾ�ַ��õ��ڴ�
	tPixelDatas.aucPixelDatas = (unsigned char *)malloc(tPixelDatas.iTotalBytes);
	if(! tPixelDatas.aucPixelDatas)
	{
		DEBUG_PRINTF("GenerateInterval malloc error \n");
		return -1;
	}

	//��Ϊ����ɫ
	ClearPixelDatasMem(&tPixelDatas, COLOR_BACKGROUND);
	
	//ʹ�� freetype �� ���ֿ� �� ascii ��������
	if(GetPixelDatasForFreetype(strNumber, &tPixelDatas))
	{
		DEBUG_PRINTF("GetPixelDatasForFreetype error \n");
		return -1;
	}
	
	//д�� videoMem
	if(MergePixelDatasToVideoMem(tPixelDatas.iHeight + g_atIntevalPageIconsLayout[1].iTopLeftX, 
									tPixelDatas.iHeight /2 + g_atIntevalPageIconsLayout[1].iTopLeftY, 
									&tPixelDatas, ptVideoMem))
	{
		DEBUG_PRINTF("MergePixelDatasToVideoMem error \n");
		return -1;
	}
	
	//�ͷ��ڴ�
	free(tPixelDatas.aucPixelDatas);
	
	return 0;
}

static void IntevalPageRun(PT_PageParams ptPageParams);

static T_PageAction g_tIntevalPageAction = {
	.name = "inteval",
	.Run  = IntevalPageRun,
};

//���� Page Layout λ��
static void CalcIntevalPageLayout(PT_PageLayout ptPageLayout)
{
	int iXres;
	int iYres;
	int iBpp;
	int iIconWidth;
	int iIconHeight;
	int iIconMargin;
	int i;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
	//����LCD����Ϣ
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	// ��ť�ĸ߶���ռLCD �ߵ�2/10 ������� 1/10 ,����� ��*2
	iIconHeight = iYres * 2 / 10;
	iIconWidth  = iIconHeight * 2;
	iIconMargin = iYres * 1 / 10;

	ptPageLayout->iMaxTotalBytes = iIconWidth * iIconHeight * iBpp;

	//����ÿ���Ĵ�С
	i = 0;
	while(atLayout[i].strIconName)
	{
		//��3������ͼ��Ҫ��ʾ Ϊ������
		switch(i)
		{
			case 0:
			{
				iIconWidth  = iYres * 2 / 10;
				iIconHeight = iIconWidth / 4;
				iIconMargin += iYres * 1 / 10;
			}break;
			case 1:
			{
				iIconWidth  = iYres * 4 / 10;
				iIconHeight = iIconWidth / 2;
				iIconMargin += iYres * 1 / 10 / 4;
			}break;
			case 2:
			{
				iIconWidth  = iYres * 2 / 10;
				iIconHeight = iIconWidth / 4;
				iIconMargin += iYres * 1 / 10 / 4;
			}break;
			case 3:
			{
				iIconWidth  = iYres * 2 / 10;
				iIconHeight = iIconWidth;
				iIconMargin += iYres * 1 / 10;
			}break;
			case 4:
			{
				iIconMargin -= iIconHeight;
			}
			break;
		}
		atLayout[i].iTopLeftX     = (iXres - iIconWidth )/2;
		atLayout[i].iBottomRightX = atLayout[i].iTopLeftX + iIconWidth;
		atLayout[i].iTopLeftY     = iIconMargin;
		atLayout[i].iBottomRightY = atLayout[i].iTopLeftY + iIconHeight;
		iIconMargin +=  iIconHeight;

		switch(i)
		{
			case 3 :
			{
				atLayout[i].iTopLeftX -= iYres * 2 / 10;
				atLayout[i].iBottomRightX -= iYres * 2 / 10;
			}
			break;
			case 4 :
			{
				atLayout[i].iTopLeftX += iYres * 2 / 10;	
				atLayout[i].iBottomRightX += iYres * 2 / 10;
			}
			break;
		}
		i++;
	}
	
}

static void ShowIntevalPage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
	//���λ��Ϊ0˵��δ����λ��
	if(0 == atLayout[0].iTopLeftX && 0 == atLayout[0].iBottomRightX)
	{
		CalcIntevalPageLayout(ptPageLayout);
	}
	
	//��ȡ�Դ�
	ptVideoMem = GetVideoMem(ID("inteval"), 1);
	if(! ptVideoMem)
	{
		DEBUG_PRINTF("cat't get video mem \n");
		return ;
	}
	
	//����
	if(GeneratePage(ptPageLayout, ptVideoMem) || GenerateInterval(ptVideoMem))
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

static void IntevalPageRun(PT_PageParams ptPageParams)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;/* ���µ�״̬ */
	int iIndexPressed = 0; /* �����±� */
	
	//��ʾ icons
	ShowIntevalPage(&g_tIntevalPageLayout);

	//�����¼�
	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		iIndex = GenericGetInputEvent(&g_tIntevalPageLayout, &tInputEvent);

		/**
		 *�жϰ����¼��㷨
		 *
		 */
		//�ɿ�״̬ ѹ��ֵ Ϊ 0
		if (tInputEvent.iPressure == 0)
		{
			if(bPressed)
			{
				/* �����а�ť������ */
				ReleaseButton(&g_atIntevalPageIconsLayout[iIndexPressed]);
				bPressed = 0;
				//������º��ɿ���ͬһ������
				if(iIndexPressed == iIndex)
				{
					switch(iIndex)
					{
						case 0 : 
						{
							//�޸Ķ�ʱ���� ���ػ�
							g_iInteval++;
							ReGenerateInterval();
						}
						break;

						case 1 : 
						{
							
						}
						break;

						case 2 :
						{
							//�޸Ķ�ʱ���� ���ػ�
							g_iInteval = (1 >= g_iInteval) ? 1 : --g_iInteval;
							ReGenerateInterval();
						}
						break;
						case 4 :
						{
							GetPage("setting")->Run(NULL);
						}
						case 5 :
						{
							GetPage("setting")->Run(NULL);
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
					PressButton(&g_atIntevalPageIconsLayout[iIndexPressed]);
				}
			}
		}
	}
}

int IntevalPageInit(void)
{
	return RegisterPageAction(&g_tIntevalPageAction);
}

