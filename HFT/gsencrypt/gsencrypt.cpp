// gsencrypt.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "gsencrypt.h"

#include "des.h"
#include "blowfish.h"

#pragma comment(lib,"libeay32MD.lib")

//-----------------------------------------------------------------------------
char * SerializeData(const char *pszDataSrc, int iSizeSrc, char *pszDataDest, int iSizeDest)
{//���л�
    /*
    static int iSizeLast = 0;
    if (iSizeLast==0)
        iSizeLast = iSizeSrc;

    if (iSizeLast!=iSizeSrc)
    {
        printf("���ȴ���%d:%d:%s\n", iSizeLast, iSizeSrc, pszDataSrc);
    }
    */

    memset(pszDataDest, 0, iSizeDest);
    for (int i=0; i<iSizeSrc; i++)
    {
        char szValue[32] = {0};
        _snprintf(szValue, sizeof(szValue)-1, "%02x", (unsigned char)pszDataSrc[i]);
        strcat(pszDataDest, szValue);
        //sscanf("%i",&nNum);//����0x��ͷ��ʮ������������
    }

    return pszDataDest;
}
//-----------------------------------------------------------------------------
int gsEncryptBuffer(int pi_iMode, char *pi_pszDataRaw, int pi_iDataRawSize, char *pi_pszKey, char *po_pszDataEncrypt, int pi_iDataEncryptSize)
{//�Գ���Ϊpi_iDataRawSize�Ļ�����pi_pszDataRaw���м��ܣ���Կ��pi_pszKey, ���ķ���po_pszDataEncrypt�Ļ�������
    if (pi_iDataEncryptSize<=pi_iDataRawSize) //������岻���Է�������
        return -1;

    char szBuffer[8192*2] = {0};
    char *pszDataRaw = NULL;

    if (pi_iDataRawSize<=sizeof(szBuffer)-8-1)
    {//��ʱ�������������д���������
        pszDataRaw = szBuffer;
    }
    else
    {
        pszDataRaw = new char[pi_iDataRawSize+1+8];
        if (pszDataRaw==NULL)
            return -2;

        memset(pszDataRaw, 0, pi_iDataRawSize+1+8);
    }

    memcpy(pszDataRaw, pi_pszDataRaw, min(pi_iDataRawSize, sizeof(szBuffer)-1));
    
    char szKey[1024] = {0};
    strncpy(szKey, pi_pszKey, sizeof(szKey)-1);

    int iRetSize = 0;
	if (pi_iMode==EM_ENCRYPEMODE_DES_ECB)
	{//des����

		DES_cblock stKey = {0};
		DES_string_to_key(szKey, &stKey);

		DES_key_schedule stSchedule = {0};
		if(DES_set_key_checked(&stKey, &stSchedule)!=0)
        {
            printf("DES_set_key_checked error!!\n");
            if (pszDataRaw!=szBuffer)
                delete []pszDataRaw;
            return -3;
        }

        //	ע��, DES_ecb_encrypt
        //�� OpenSSL �� ECB ����ģʽ��Ӧ�ĺ����� DES_ecb_encrypt() ���ú�����һ�� 8 �ֽ����ķ��� input ���ܳ�Ϊһ�� 8 �ֽ����ķ��� output ����������Կ�ṹ ks ���ú��� DES_set_key() ׼���õģ�����Կ key ����������㷨������ 64 ��������ء����� enc ָʾ�Ǽ��ܻ��ǽ��ܡ��ú���ÿ��ֻ����һ�����飬����������ܺܶ�����ʱ������ʹ�á�
        //void DES_ecb_encrypt(const_DES_cblock *input,DES_cblock *output, DES_key_schedule *ks,int enc);
        //int DES_set_key(const_DES_cblock *key,DES_key_schedule *schedule);
        //
        //1.���ܵ����ı�����8λ��
        //2.ֻ�ܼ���8λ������ж��飬��Ҫ��������DES_ecb_encrypt
        int iBlockCount;
        if (pi_iDataRawSize%8==0)
            iBlockCount = pi_iDataRawSize/8;
        else
            iBlockCount = (pi_iDataRawSize / 8)+1;
        for (int i=0; i<iBlockCount; i++)
        {
    		DES_ecb_encrypt((DES_cblock *)(pszDataRaw+i*8), (DES_cblock *)(po_pszDataEncrypt+i*8), &stSchedule, DES_ENCRYPT);
        }

        iRetSize = iBlockCount*8;
    }
	else if (pi_iMode==EM_ENCRYPEMODE_BLOWFISH)
	{//blowfish ����
		BF_KEY stBF_Key = {0};                
		char ivec[32] = {0};

		BF_set_key(&stBF_Key, strlen(szKey), (unsigned char *)szKey);
		int nNum = 0;
		BF_cfb64_encrypt((unsigned char *)pszDataRaw, (unsigned char *)po_pszDataEncrypt, pi_iDataRawSize, 
			&stBF_Key, (unsigned char *)&ivec, &nNum, BF_ENCRYPT);

        iRetSize = pi_iDataRawSize;
	}
	else
    {
        if (pszDataRaw!=szBuffer)
            delete []pszDataRaw;
		return -1; //��֧�ֵļ��ܷ�ʽ
    }

    if (pszDataRaw!=szBuffer)
        delete []pszDataRaw;

    return iRetSize;
}
//-----------------------------------------------------------------------------
extern "C" int __stdcall gsEncrypt(int pi_iMode, char *pi_pszDataRaw, int pi_iDataRawSize, char *pi_pszKey, char *po_pszDataEncrypt, int pi_iDataEncryptSize)
{//�Գ���Ϊpi_iDataRawSize�Ļ�����pi_pszDataRaw���м��ܣ���Կ��pi_pszKey, ���ķ���po_pszDataEncrypt�Ļ�������
    if (pi_iDataEncryptSize<=pi_iDataRawSize*2) //������岻���Է�������
        return -1;

    char szBuffer[8192*2] = {0};
    char *pszDataRaw = NULL;
    int iDataSize = 0;

    if (pi_iDataRawSize<=sizeof(szBuffer)-8-1)
    {//��ʱ�������������д���������
        pszDataRaw = szBuffer;
        iDataSize = sizeof(szBuffer);
    }
    else
    {
        iDataSize = pi_iDataRawSize+1+8;
        pszDataRaw = new char[iDataSize];
        if (pszDataRaw==NULL)
            return -2;

        memset(pszDataRaw, 0, iDataSize);
    }

    memset(po_pszDataEncrypt, 0, pi_iDataEncryptSize);
    int iRetSize = gsEncryptBuffer(pi_iMode, pi_pszDataRaw, pi_iDataRawSize, pi_pszKey, pszDataRaw, iDataSize);
    if (iRetSize>0)
    {
        //���л�
        SerializeData(pszDataRaw, iRetSize, po_pszDataEncrypt, pi_iDataEncryptSize);
    }

    if (pszDataRaw!=szBuffer)
        delete []pszDataRaw;

    return 0;
}
//-----------------------------------------------------------------------------
