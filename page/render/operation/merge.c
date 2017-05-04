
#include <pic_operation.h>
#include <string.h>

//��Сͼ�ϲ�����ͼ��ָ��λ��
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic)
{
	int i;
	unsigned char *pucSrc;
	unsigned char *pucDst;

	//�жϴ�С�Ƿ񳬹� BPP �Ƿ���ͬ
	if((ptSmallPic->iBpp != ptBigPic->iBpp) || ((ptSmallPic->iHeight + iY) > ptBigPic->iHeight) 
		|| ((ptSmallPic->iWidth + iX) > ptBigPic->iWidth))
	{
		return -1;
	}
	//�ϲ��ļ�
	#if 1
	pucSrc = ptSmallPic->aucPixelDatas;
	pucDst = ptBigPic->aucPixelDatas + iY*ptBigPic->iLineBytes + iX*ptBigPic->iBpp/8;
	for(i=0; i<ptSmallPic->iHeight; i++)
	{
		memcpy(pucDst, pucSrc, ptSmallPic->iLineBytes);
		pucSrc += ptSmallPic->iLineBytes;
		pucDst += ptBigPic->iLineBytes; 
	}
	#else
	//�ҵ��㷨Ч�ʲ���
	pucSrc = ptSmallPic->aucPixelDatas;
	pucDst = ptBigPic->aucPixelDatas;
	for(i=iY; i<(iY + ptSmallPic->iHeight); i++)
	{
		pucSrc += ptSmallPic->iLineBytes; 
		pucDst += ptBigPic->iLineBytes + iX*ptBigPic->iBpp/8;;
		memcpy(pucDst, pucSrc, ptSmallPic->iLineBytes);
	}
	#endif
	return 0;
}

