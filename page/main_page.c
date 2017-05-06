#include <conf.h>
#include <render.h>
#include <page_manager.h>
#include <stdlib.h>

//iconͼ��
static T_Layout g_atMainPageIconsLayout[] = {
	{0, 0, 0, 0, "browse_mode.bmp"},
	{0, 0, 0, 0, "continue_mod.bmp"},
	{0, 0, 0, 0, "setting.bmp"},
	{0, 0, 0, 0, NULL},
};

//main Page layout
static T_PageLayout g_tMainPageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atMainPageIconsLayout,
};

static void MainPageRun(PT_PageParams ptPageParams);

static T_PageAction g_tMainPageAction = {
	.name = "main",
	.Run  = MainPageRun,
};

//���� Page Layout λ��
static void CalcMainPageLayout(PT_PageLayout ptPageLayout)
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
	while(atLayout[i].strIconName)
	{
		atLayout[i].iTopLeftX     = (iXres - iIconWidth )/2;
		atLayout[i].iBottomRightX = atLayout[i].iTopLeftX + iIconWidth;
		atLayout[i].iTopLeftY     = iIconMargin;
		atLayout[i].iBottomRightY = atLayout[i].iTopLeftY + iIconHeight;
		iIconMargin +=  iIconHeight + iYres * 1 / 10;
		
		i++;
	}
	
}

static void ShowMainPage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
	//���λ��Ϊ0˵��δ����λ��
	if(0 == atLayout[0].iTopLeftX && 0 == atLayout[0].iBottomRightX)
	{
		CalcMainPageLayout(ptPageLayout);
	}
	
	//��ȡ�Դ�
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if(! ptVideoMem)
	{
		DEBUG_PRINTF("cat't get video mem \n");
		return ;
	}
	
	//����
	if(GeneratePage(ptPageLayout, ptVideoMem))
	{
		DEBUG_PRINTF("GeneratePage error \n");
		return ;
	}
	
	//ˢ��LCD
	FlushVideoMemToDev(ptVideoMem);

	//��ʾ�����ʷλ��
	ShowHistoryMouse();
	
	//�ͷ��Դ�
	PutVideoMem(ptVideoMem);
	
}

static void MainPageRun(PT_PageParams ptPageParams)
{
	int iIndex;
	T_InputEvent tInputEvent;
	T_PageParams tPageParams;
	tPageParams.iPageID = ID("main");
	int bPressed = 0;/* ���µ�״̬ */
	int iIndexPressed = 0; /* �����±� */
	
	//��ʾ icons
	ShowMainPage(&g_tMainPageLayout);

	//�����¼�
	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		iIndex = GenericGetInputEvent(&g_tMainPageLayout, &tInputEvent);

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
				ReleaseButton(&g_atMainPageIconsLayout[iIndexPressed]);
				bPressed = 0;
				//������º��ɿ���ͬһ������
				if(iIndexPressed == iIndex)
				{
					switch(iIndex)
					{
						case 1 : 
						break;

						case 2 : 
						break;

						case 3 : 
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
					PressButton(&g_atMainPageIconsLayout[iIndexPressed]);
				}
			}
		}
	}
}

int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}

