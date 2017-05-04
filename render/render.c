#include <render.h>
#include <conf.h>
#include <file.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic_operation.h>

int GetFontPixel()
{
	return 0;
}

int GetPicPixel()
{
	return 0;
}

int GetDrawPixel()
{
	return 0;
}

int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas)
{
	T_FileMap tFileMap;
	PT_PicFileParser ptPicFileParser;

	//���ļ����� mmap
	snprintf(tFileMap.strFileName, 256, "%s/%s", ICON_DIR, strFileName);
	
	tFileMap.strFileName[255] = '\0';
	if(MapFile(&tFileMap))
	{
		DEBUG_PRINTF("cat't GetPixelDatasForIcon error \n");
		return -1;
	}
	
	//���Һ��ʵĽ�����
	ptPicFileParser = MatchParser(tFileMap.pucFileMapMem);
	//ptPicFileParser = Parser("bmp");
	
	//����ͼƬ�ŵ� ptPixelDatas
	if(ptPicFileParser->GetPixelDatas(tFileMap.pucFileMapMem, ptPixelDatas))
	{
		UnMapFile(&tFileMap);
		DEBUG_PRINTF("cat't ptPicFileParser GetPixelDatas error \n");
		return -1;
	}
	UnMapFile(&tFileMap);
	return 0;
}

//��ָ����ʾ�������ɫȡ��
static void InvertButton(PT_Layout ptLayout)
{
	int i;
	int y;
	unsigned char *videoMem;
	int iButtonWidthBytes;
	PT_DispOpr ptDispOpr = GetDefaultDispDev();
	videoMem = ptDispOpr->pucDispMem + ptLayout->iTopLeftY * ptDispOpr->iLineWidth;
	videoMem += ptLayout->iTopLeftX * ptDispOpr->iBpp / 8;
	iButtonWidthBytes = (ptLayout->iBottomRightX - ptLayout->iTopLeftX) * ptDispOpr->iBpp / 8;
	//��ͼƬ����ȡ��
	for(y = ptLayout->iTopLeftY; y< ptLayout->iBottomRightY; y++)
	{
		for(i = 0; i< iButtonWidthBytes; i++)
		{
			videoMem[i] = ~videoMem[i];
		}
		videoMem += ptDispOpr->iLineWidth;
	}
}

void PressButton(PT_Layout ptLayout)
{
	InvertButton(ptLayout);
}

void ReleaseButton(PT_Layout ptLayout)
{
	InvertButton(ptLayout);
}