#include <encoding_manager.h>
#include <fonts_manager.h>
#include <string.h>

static int AsciiIsSupport(unsigned char *pucBufHead);
static int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tAsciiEncodingOpr = {
	.name = "ascii",
	.iHeadLen = 0,
	.isSupport = AsciiIsSupport,
	.GetCodeFrmBuf = AsciiGetCodeFrmBuf,
};
static int AsciiIsSupport(unsigned char *pucBufHead)
{
	const char aStrUtf8[]    = {0xEF, 0xBB, 0xBF, 0};
	if(0 == strncmp(aStrUtf8, (const char *)pucBufHead, 3))
	{
		return 0;
	}
	return 1;
}

static int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
	//С��0x80 ˵���� ascii ����
	if(pucBufStart < pucBufEnd  && 0x80 > *pucBufStart)
	{
		*pdwCode = (unsigned int)*pucBufStart;
		return 1;
	}
	if((pucBufStart+1) < pucBufEnd && 0x80 < *pucBufStart && 0x80 < *(pucBufStart+1))
	{
		*pdwCode = *pucBufStart<<8 | *(pucBufStart+1);
		return 2;
	}
	
	//�ļ�����
	return 0;
}

int AsciiEncodingInit(void)
{
	int iError = 0;
	//��ʼ��ʱ ���� FontOpr
	//���õ�ǰ�����Ѿ���ʼ���� font asc gbk freetype
	//�ȼ��صĺ����
	iError |= AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("freetype"));
	iError |= AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("gbk"));
	iError |= AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("ascii"));
	RegisterEncodingOpr(&g_tAsciiEncodingOpr);
	return iError;
}

