#include <conf.h>
#include <render.h>
#include <page_manager.h>
#include <stdlib.h>

//iconͼ��
static T_Layout g_atSettingPageIconsLayout[] = {
	{0, 0, 0, 0, "select_fold.bmp"},
	{0, 0, 0, 0, "interval.bmp"},
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, NULL},
};

//Setting Page layout
static T_PageLayout g_tSettingPageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atSettingPageIconsLayout,
};

static void SettingPageRun(PT_PageParams ptPageParams);

static T_PageAction g_tSettingPageAction = {
	.name = "setting",
	.Run  = SettingPageRun,
};

//���� Page Layout λ��
static void CalcSettingPageLayout(PT_PageLayout ptPageLayout)
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
		if(2 == i)
		{
			iIconWidth = iIconHeight;
		}
		atLayout[i].iTopLeftX     = (iXres - iIconWidth )/2;
		atLayout[i].iBottomRightX = atLayout[i].iTopLeftX + iIconWidth;
		atLayout[i].iTopLeftY     = iIconMargin;
		atLayout[i].iBottomRightY = atLayout[i].iTopLeftY + iIconHeight;
		iIconMargin +=  iIconHeight + iYres * 1 / 10;
		
		i++;
	}
	
}

static void ShowSettingPage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
	//���λ��Ϊ0˵��δ����λ��
	if(0 == atLayout[0].iTopLeftX && 0 == atLayout[0].iBottomRightX)
	{
		CalcSettingPageLayout(ptPageLayout);
	}
	
	//��ȡ�Դ�
	ptVideoMem = GetVideoMem(ID("setting"), 1);
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
	//FlushVideoMemToDev(ptVideoMem);
	//��Ϊ�ػ�ҳ��������ػ���꣬����Ҫ����ʾ�����طŵ� �����Դ���
	FlushVideoMemToDevSync(ptVideoMem);

	//��ʾ�����ʷλ��
	ShowHistoryMouse();
	
	//�ͷ��Դ�
	PutVideoMem(ptVideoMem);
	
}

static void SettingPageRun(PT_PageParams ptPageParams)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;/* ���µ�״̬ */
	int iIndexPressed = 0; /* �����±� */
	
	//��ʾ icons
	ShowSettingPage(&g_tSettingPageLayout);

	//�����¼�
	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		iIndex = GenericGetInputEvent(&g_tSettingPageLayout, &tInputEvent);

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
				ReleaseButton(&g_atSettingPageIconsLayout[iIndexPressed]);
				bPressed = 0;
				//������º��ɿ���ͬһ������
				if(iIndexPressed == iIndex)
				{
					switch(iIndex)
					{
						case 0 : 
						break;

						case 1 : 
						{
							GetPage("inteval")->Run(NULL);
						}
						break;

						case 2 :
						{
							GetPage("main")->Run(NULL);
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
					PressButton(&g_atSettingPageIconsLayout[iIndexPressed]);
				}
			}
		}
	}
}

int SettingPageInit(void)
{
	return RegisterPageAction(&g_tSettingPageAction);
}

