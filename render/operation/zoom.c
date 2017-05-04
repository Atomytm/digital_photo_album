
#include <pic_operation.h>
#include <stdlib.h>
#include <string.h>

/**
 * ͼƬ���Ŵ���
 * ��򵥵�ʵ�֣��ٽ�ȡֵ�� �㷨�򵥣�ȱ��Ŵ����о��
 */
int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic)
{
    unsigned long dwDstWidth = ptZoomPic->iWidth;
	//�ȼ����һ�е�ȡ����λ��
    unsigned long *pdwSrcXTable = malloc(sizeof(unsigned long) *dwDstWidth);
	unsigned long x;
	unsigned long y;
	unsigned char *pucDest;
	unsigned char *pucSrc;
	unsigned long dwPixelBytes = ptOriginPic->iBpp/8;

	if (ptOriginPic->iBpp != ptZoomPic->iBpp)
	{
		return -1;
	}
	//����ȡ����
	for(x=0; x<dwDstWidth; x++)
	{
		pdwSrcXTable[x] = x * ptOriginPic->iWidth / ptZoomPic->iWidth;
	}
	//��������ͼ
	for(y=0; y<ptZoomPic->iHeight; y++)
	{
		pucDest = ptZoomPic->aucPixelDatas + y * ptZoomPic->iLineBytes;
		pucSrc  = ptOriginPic->aucPixelDatas + y * ptOriginPic->iHeight / ptZoomPic->iHeight * ptOriginPic->iLineBytes;
		for(x=0; x<dwDstWidth; x++)
		{
			/* ԭͼ����: pdwSrcXTable[x]��srcy             
			* ��������: x, y			 */
			memcpy(pucDest+x*dwPixelBytes, pucSrc+pdwSrcXTable[x]*dwPixelBytes, dwPixelBytes);
		}
	}
	
	free(pdwSrcXTable);
	return 0;
}

