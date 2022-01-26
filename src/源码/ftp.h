#pragma once

#include "afxinet.h"
#include "MyFtpClientDlg.h"

UINT Query(LPVOID  pParam) {
	if(pParam == NULL)
		AfxEndThread(NULL);
	SERVER_INFO* sp;	
	sp = (SERVER_INFO*)pParam;
	CListBox* pList;
	CString ip;
	CString user;
	CString pwd;
	CString dicName;
	pList = sp->pList;
	ip = sp->ip;
	user = sp->user;
	pwd = sp->pwd;
	if (sp->path == "")
		dicName = "/";
	else
		dicName = sp->path;
	CInternetSession* pSession = NULL;//�Ự
	CFtpConnection* pConnection = NULL;//����
	CFtpFileFind* pFileFind = NULL;//��ѯ
	CString fileName;
	BOOL isContinue;
	
	while (pList->GetCount()) {
		pList->DeleteString(0);
	}

	//Internet�Ự
	pSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);

	try{  
		// ���ӷ�����
		pConnection = pSession->GetFtpConnection(ip, user, pwd);
	}
	catch (CInternetException* e) {
		// ����ʧ��
		AfxMessageBox("����ʧ��!\n", MB_OK | MB_ICONSTOP);
		e->Delete(); 						
		pConnection = NULL;
	}

	if (pConnection != NULL){
		m_upload.EnableWindow(TRUE);
		m_back.EnableWindow(TRUE);
		
		pFileFind = new CFtpFileFind(pConnection);		// ����CFtpFileFind�������캯������CFtpConnection�����ָ��
		BOOL isContinue = pFileFind->FindFile(dicName);	// ���ҷ������ϵ�ǰĿ¼�������ļ�
		if (!isContinue)								// û���ļ��ͽ�������
		{
			pFileFind->Close();
			pFileFind = NULL;
		}

		while (isContinue) {							// �ҵ��˵�һ���ļ����������������ļ�			
			isContinue = pFileFind->FindNextFile();		// ������һ���ļ�	
			CString fileName = pFileFind->GetFileName();// ����ҵ����ļ����ļ���													
			if (pFileFind->IsDirectory())				// ����ҵ��Ķ����Ǹ�Ŀ¼���ͽ�Ŀ¼������������
				fileName = "[" + fileName + "]";		// ���ҵ����ļ���Ŀ¼����ʾ���б����			
			pList->AddString(fileName);
		}
		
		if (pFileFind != NULL) {
			pFileFind->Close();						// ������ѯ
			pFileFind = NULL;
		}
	}
	delete pFileFind;								// ɾ���ļ���ѯ����
	if (pConnection != NULL){
		pConnection->Close();
		delete pConnection;							// ɾ��FTP���Ӷ���
	}
	delete pSession; 								// ɾ��Internet �Ự���� 
	return 0;
}

//�����ļ����õĺ���
BOOL MyDownload(CString  ip,CString  user,CString  pwd,CString  sourceName,CString  destName){

	CInternetSession* pSession;			//����Ự�������ָ��
	CFtpConnection* pConnection;		//�������Ӷ������ָ��

	pConnection = NULL;

	//����Internet�Ự����
	pSession = new CInternetSession(AfxGetAppName(), 1,PRE_CONFIG_INTERNET_ACCESS);

	try{
		//����FTP����
		pConnection = pSession->GetFtpConnection(ip,user, pwd);
	}
	catch (CInternetException* e) {
		//������
		e->Delete();
		pConnection = NULL;
		return FALSE;
	}

	if (pConnection != NULL){
		//�����ļ�
		if (!pConnection->GetFile(sourceName, destName)){
			//�����ļ�����
			pConnection->Close();
			delete pConnection;
			delete pSession;
			return FALSE;
		}
	}

	//�������
	if (pConnection != NULL){
		pConnection->Close();
		delete pConnection;
	}
	delete pSession;
	return TRUE;
}

UINT DownloadFile(LPVOID  pParam) {
	if (pParam == NULL)
		AfxEndThread(NULL);
	SERVER_INFO* sp;
	CListBox* pList;
	CString ip;
	CString user;
	CString pwd;
	sp = (SERVER_INFO*)pParam;
	pList = sp->pList;
	ip = sp->ip;
	user = sp->user;
	pwd = sp->pwd;
	CString  sourceName = sp->path;
	CInternetSession* pSession = NULL;//�Ự
	CFtpConnection* pConnection = NULL;//����
	CFtpFileFind* pFileFind = NULL;//��ѯ
	int  nSel = pList->GetCurSel();//���ص�ǰѡ�е�item
	CString fileName;
	pList->GetText(nSel, fileName);
	if (sourceName.GetAt(0) != '[') {
		CString destName;
		CFileDialog dlg(FALSE, "", fileName);	//�ļ��Ի���������
		if (dlg.DoModal() == IDOK)     		//�����ļ��Ի���
		{
			//��������ļ��ڱ��ػ��ϴ洢��·��������
			destName = dlg.GetPathName();
			//���ú��������ļ�
			if (MyDownload(ip, user, pwd, sourceName, destName))
				AfxMessageBox("���سɹ���", MB_OK | MB_ICONINFORMATION);
			else {
				AfxMessageBox("����ʧ�ܣ�", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}
		else {
			return FALSE;
		}
	}
	else {
		AfxMessageBox("��������Ŀ¼!\n", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return 0;
}

BOOL MyUpload(CString strFtpSite, CString strName, CString strPwd, CString strSourceName, CString strDestName){
	
	CInternetSession* pSession;
	CFtpConnection* pConnection;

	pConnection = NULL;

	//����Internet�Ự
	pSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);

	try{
		//����FTP����
		pConnection = pSession->GetFtpConnection(strFtpSite, strName, strPwd);
	}
	catch (CInternetException* e){
		//������
		e->Delete();
		pConnection = NULL;
		return FALSE;
	}

	if (pConnection != NULL){
		//�ϴ��ļ�
		if (!pConnection->PutFile(_T(strSourceName), _T(strDestName))){
			//�ϴ��ļ�����
			pConnection->Close();
			delete pConnection;
			delete pSession;
			return FALSE;
		}
	}

	//�������
	if (pConnection != NULL){
		pConnection->Close();
		delete pConnection;
	}
	delete pSession;
	return TRUE;
}

UINT UploadFile(LPVOID  pParam) {
	if (pParam == NULL)  AfxEndThread(NULL);
	//������ȡ�������õĲ����Ĵ��� 
	SERVER_INFO* sp;
	CListBox* pList;
	CString  ip;
	CString  user;
	CString  pwd;
	sp = (SERVER_INFO*)pParam;
	pList = sp->pList;
	ip = sp->ip;
	user = sp->user;
	pwd = sp->pwd;
	CString sourceName;
	CString destPath = sp->path;
	CFileDialog dlg(TRUE, "", "*.*");       //�����ı��Ի���������
	if (dlg.DoModal() == IDOK)
	{
		//��ô��ϴ��ı��ػ��ļ�·�����ļ���
		sourceName = dlg.GetPathName();
		CString destName = destPath + "/" +dlg.GetFileName();

		//����Upload�����ϴ��ļ�
		if (MyUpload(ip, user, pwd, sourceName, destName))
			AfxMessageBox("�ϴ��ɹ���", MB_OK | MB_ICONINFORMATION);
		else
			AfxMessageBox("�ϴ�ʧ�ܣ�", MB_OK | MB_ICONSTOP);
		Query(pParam);
	}
	return 0;
}

UINT MyDeleteFile(LPVOID  pParam) {
	if (pParam == NULL)
		AfxEndThread(NULL);
	SERVER_INFO* sp;
	CListBox* pList;
	CString ip;
	CString user;
	CString pwd;
	sp = (SERVER_INFO*)pParam;
	pList = sp->pList;
	ip = sp->ip;
	user = sp->user;
	pwd = sp->pwd;
	CString  sourceName = sp->path;
	CInternetSession* pSession = NULL;//�Ự
	CFtpConnection* pConnection = NULL;//����
	CFtpFileFind* pFileFind = NULL;//��ѯ
	int  nSel = pList->GetCurSel();//���ص�ǰѡ�е�item
	CString fileName;
	pList->GetText(nSel, fileName);

	//Internet�Ự
	pSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);

	try {
		// ���ӷ�����
		pConnection = pSession->GetFtpConnection(ip, user, pwd);
	}
	catch (CInternetException* e) {
		// ����ʧ��
		AfxMessageBox("����ʧ��!\n", MB_OK | MB_ICONSTOP);
		e->Delete();
		pConnection = NULL;
	}
	if (pConnection != NULL) {
		if (fileName.GetAt(0) != '[') {
			pConnection->Remove(sourceName + "/" + fileName);
			AfxMessageBox("ɾ���ɹ���", MB_OK | MB_ICONINFORMATION);
			Query(pParam);
		}
		else {
			AfxMessageBox("����ɾ��Ŀ¼!\n", MB_OK | MB_ICONSTOP);
			return 0;
		}
	}
	return 0;
}

BOOL putFileToFtp(CString strFilePathName, CString strFileFtpPathName)
{
    CInternetSession *pInternetSession;
    CFtpConnection *pFtpConnection;
    pInternetSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);
    try
    {
        pFtpConnection = pInternetSession->GetFtpConnection
            (_T("127.0.0.1"),//FTP������IP
            _T("user"),//�û���
            _T("pwd"),//����
            21,//FTP�������˿ں�
            TRUE);
        if (pFtpConnection)
        {
            BOOL bPut = pFtpConnection->PutFile(strFilePathName, strFileFtpPathName);
            if (!bPut)
            {
				AfxMessageBox("�ϴ�ʧ��");
                return FALSE;
            }
            if (pFtpConnection)
            {
                pFtpConnection->Close();
                pFtpConnection = NULL;
            }
            if (pInternetSession)
            {
                pInternetSession->Close();
                pInternetSession = NULL;
            }
            return TRUE;
        }
        else
        {
			AfxMessageBox(_T("FTP����������ʧ��"));
            return FALSE;
        }
    }
    catch (CInternetException *pEx)
    {
        TCHAR szError[100];
        if (pEx->GetErrorMessage(szError, 100))
        {
            AfxMessageBox(szError);
        }
        pEx->Delete();
        pFtpConnection = NULL;
        if (pInternetSession)
        {
            pInternetSession->Close();
            pInternetSession = NULL;
        }
        return FALSE;
    }
}

BOOL CreateFolderToFtp(CString& strFtpPath){
	//Ҫ��һ�������������ûɶ�ã��������ļ��й̶�Ϊskonda//SN//image  skonda�ļ����Ѵ���
	CInternetSession* pInternetSession;
	CFtpConnection* pFtpConnection;
	pInternetSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);
	try{
		pFtpConnection = pInternetSession->GetFtpConnection
		(_T("127.0.0.1"),//FTP������IP
			_T("user"),//�û���
			_T("pwd"),//����
			21,//FTP�������˿ں�
			TRUE);
		if (pFtpConnection)
		{
			CString strFtp1Path, strFtp2Path;
			strFtp1Path = _T("/1111");
			strFtp2Path = strFtp1Path + _T("/2222");
			BOOL ret = pFtpConnection->CreateDirectory(strFtp1Path);//���ܴ����༶Ŀ¼
			if (!ret)
			{
				AfxMessageBox("����FTP�ļ���ʧ��");
				strFtpPath.Empty();
				return FALSE;
			}

			ret = pFtpConnection->CreateDirectory(strFtp2Path);//���ܴ����༶Ŀ¼
			if (!ret)
			{
				AfxMessageBox("����FTP�ļ���ʧ��");
				strFtpPath = strFtp1Path;
				return FALSE;
			}
			if (pFtpConnection)
			{
				pFtpConnection->Close();
				pFtpConnection = NULL;
			}
			if (pInternetSession)
			{
				pInternetSession->Close();
				pInternetSession = NULL;
			}
			strFtpPath = strFtp2Path;
			return TRUE;
		}
		else
		{
			AfxMessageBox("FTP����������ʧ��");
			strFtpPath.Empty();
			return FALSE;
		}
	}
	catch (CInternetException* pEx)
	{
		TCHAR szError[100];
		if (pEx->GetErrorMessage(szError, 100))
		{
			AfxMessageBox(szError);
		}
		pEx->Delete();
		pFtpConnection = NULL;
		if (pInternetSession)
		{
			pInternetSession->Close();
			pInternetSession = NULL;
		}
		return FALSE;
	}
}


