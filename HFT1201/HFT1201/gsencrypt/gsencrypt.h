// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� GSENCRYPT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// GSENCRYPT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef GSENCRYPT_EXPORTS
#define GSENCRYPT_API __declspec(dllexport)
#else
#define GSENCRYPT_API __declspec(dllimport)
#endif

#define STDCALL __stdcall


//���ܷ�ʽ����FIXЭ������չ�����������µ��޸ģ�������FIXЭ��Լ��
enum em_EncryptMode { 
	EM_ENCRYPEMODE_DES_ECB=2,
	EM_ENCRYPEMODE_BLOWFISH=101
};

/*
	���ļ��ܺ����������ڸ���������� add by luosj 20111108

	����˵����
	int pi_iMode                  ���ܷ�ʽ, ��em_EncryptMode
	char *pi_pszPasswordIn        ��������
	int pi_iDataRawSize			  �������ĳ���
	char *pi_key                  ��Կ
	char *po_pszPasswordOut       ��������
	int pi_iSize                  �������Ļ�������С��һ��128�ֽ��㹻

	����: 0:�ɹ�  <0:ʧ��
*/
//extern "C" int __stdcall gsEncrypt(int pi_iMode, char *pi_pszPasswordIn, char *pi_pszKey, char *po_pszPasswordOut, int pi_iSize);
//extern "C" int __stdcall gsEncrypt(int pi_iMode, char *pi_pszDataIn, int pi_iDataInSize, char *pi_pszKey, char *po_pszDataOut, int pi_iSize)
extern "C" int __stdcall gsEncrypt(int pi_iMode, char *pi_pszDataRaw, int pi_iDataRawSize, char *pi_pszKey, char *po_pszDataEncrypt, int pi_iDataEncryptSize);