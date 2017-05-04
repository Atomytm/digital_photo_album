#include <encoding_manager.h>
#include <fonts_manager.h>
#include <string.h>

static int Utf8IsSupport(unsigned char *pucBufHead);
static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tUtf8EncodingOpr = {
	.name = "utf8",
	.iHeadLen = 3,
	.isSupport = Utf8IsSupport,
	.GetCodeFrmBuf = Utf8GetCodeFrmBuf,
};
static int Utf8IsSupport(unsigned char *pucBufHead)
{
	const char aStrUtf8[]    = {0xEF, 0xBB, 0xBF, 0};
	if(0 == strncmp(aStrUtf8, (const char*)pucBufHead, 3))
	{
		return 1;
	}
	return 0;
}

/* ���ǰ��Ϊ1��λ�ĸ���
 * ����������� 11001111 ��ǰ��1��2λ
 *              11100001 ��ǰ��1��3λ
 */
static int GetPreOneBits(unsigned char ucchar)
{
	int len = 0;
	int i;
	for(i=7; i>=0; i--)
	{
		if(ucchar & (1<<i))
		{
			len++;
		}
		else
		{
			break;
		}
	}
	return len;
}

static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
	//utf8 �������
	/*
    ����UTF-8�����е������ֽ�B�����B�ĵ�һλΪ0����BΪASCII�룬����B�����ı�ʾһ���ַ�;
    ���B�ĵ�һλΪ1���ڶ�λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���е�һ���ֽڣ����Ҳ�Ϊ�ַ��ĵ�һ���ֽڱ���;
    ���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ��������ֽڱ�ʾ;
    ���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ��������ֽڱ�ʾ;
    ���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ����ĸ��ֽڱ�ʾ;

    ��ˣ���UTF-8�����е������ֽڣ����ݵ�һλ�����ж��Ƿ�ΪASCII�ַ�;
    ����ǰ��λ�����жϸ��ֽ��Ƿ�Ϊһ���ַ�����ĵ�һ���ֽ�; 
    ����ǰ��λ�����ǰ��λ��Ϊ1������ȷ�����ֽ�Ϊ�ַ�����ĵ�һ���ֽڣ����ҿ��ж϶�Ӧ���ַ��ɼ����ֽڱ�ʾ;
    ����ǰ��λ�����ǰ��λΪ1�������жϱ����Ƿ��д�������ݴ���������Ƿ��д���
	*/
	int preBit = GetPreOneBits(*pucBufStart);
	if (pucBufStart >= pucBufEnd)
	{
		/* �ļ����� */
		return 0;
	}
	//ascii ����
	if((0 == preBit) && pucBufStart < pucBufEnd)
	{
		*pdwCode = *pucBufStart;
		pucBufStart++;
		return 1;
	}
	if((pucBufStart + preBit) < pucBufEnd)
	{
		//���ϡ���UTF-8�����ǡ�11100100 10111000 10100101����ת����ʮ�����ƾ���E4B8A5��
		if(2 == preBit)
		{
			*pdwCode = (*pucBufStart & 0x1f)<<6 | (*(pucBufStart+1) & 0x3f);
		}
		if(3 == preBit)
		{
			*pdwCode = (*pucBufStart & 0xf)<<12 | (*(pucBufStart+1) & 0x3f)<<6 | (*(pucBufStart+2) & 0x3f);
		}
		pucBufStart += preBit;
		return preBit;
	}
	//�ļ�����
	return 0;
}

int Utf8EncodingInit(void)
{
	int iError = 0;
	//��ʼ��ʱ ���� FontOpr
	//���õ�ǰ�����Ѿ���ʼ���� font asc gbk freetype
	iError |= AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("freetype"));
	iError |= AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("ascii"));
	RegisterEncodingOpr(&g_tUtf8EncodingOpr);
	return iError;
}
