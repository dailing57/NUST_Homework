#pragma once
#include "pch.h"
#include "ftpControl.h"
//#include "CFolder.h"

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
	BOOL isContinue=0;
	
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
		}
		else {
			CString folderName;
			for (int i = 1; i < fileName.GetLength()-1; i++) {
				folderName += fileName[i];
			}
			pConnection->RemoveDirectory(sourceName + "/" + folderName);
		}
	}
	if (pConnection != NULL) {
		AfxMessageBox("ɾ���ɹ���", MB_OK | MB_ICONINFORMATION);
		Query(pParam);
	}
	return 0;
}

BOOL MyCreateFolder(CString folderName,CString user,CString pwd,CString ip){
	CInternetSession* pInternetSession;
	CFtpConnection* pFtpConnection;
	pInternetSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);
	try{
		pFtpConnection = pInternetSession->GetFtpConnection(ip,user,pwd);
		if (pFtpConnection)
		{
			BOOL ret = pFtpConnection->CreateDirectory(folderName);//���ܴ����༶Ŀ¼
			if (!ret)
			{
				AfxMessageBox("����FTP�ļ���ʧ��");
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
			AfxMessageBox("FTP����������ʧ��");
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

UINT CreateFolder(LPVOID  pParam) {
	if (pParam == NULL)  AfxEndThread(NULL);
	CFolder folder;
	folder.DoModal();
	CString folderName = folder.folderName;
	SERVER_INFO* sp = (SERVER_INFO*)pParam;
	CListBox* pList = sp->pList;
	CString  ip = sp->ip;
	CString  user = sp->user;
	CString  pwd = sp->pwd;
	folderName = sp->path + "/" + folderName;
	MyCreateFolder(folderName, user, pwd, ip);
	Query(pParam);
	return 0;
}

void MyDownloadFolder(CFtpConnection* pConnection,CString path,CString destPath) {
	pConnection->SetCurrentDirectoryA(path);
	CFtpFileFind finder(pConnection);
	BOOL isContinue = finder.FindFile(path);
	queue<CString> q;
	while (isContinue) {							
		isContinue = finder.FindNextFileA();
		CString fileName = finder.GetFileName();
		if (finder.IsDirectory()) {
			q.push(fileName);
			CreateDirectory(destPath + "/" + fileName, NULL);
		}
		else {
			pConnection->GetFile(path + "/" + fileName, destPath + "/" + fileName);
		}
	}
	finder.Close();
	while (!q.empty()) {
		CString curDir = q.front();
		q.pop();
		MyDownloadFolder(pConnection, path + "/" + curDir, destPath + "/" + curDir);
	}
}

CString getFolderName() {
	char szPath[MAX_PATH];     //���ѡ���Ŀ¼·�� 
	CString str;

	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = "��ѡ��Ŀ¼��";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	//����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);

	if (lp && SHGetPathFromIDList(lp, szPath))
		str.Format("%s", szPath);
	else
		AfxMessageBox("��Ч��Ŀ¼");
	return str;
}

UINT DownloadFolder(LPVOID  pParam) {
	if (pParam == NULL)  AfxEndThread(NULL);
	SERVER_INFO* sp = (SERVER_INFO*)pParam;
	CListBox* pList = sp->pList;
	CString  ip = sp->ip;
	CString  user = sp->user;
	CString  pwd = sp->pwd;
	CString  path = sp->path;
	CInternetSession* pSession = NULL;
	CFtpConnection* pConnection = NULL;
	CFtpFileFind* pFileFind = NULL;
	CString fileName;
	BOOL isContinue=0;
	int  nSel = pList->GetCurSel();//���ص�ǰѡ�е�item
	CString dir;
	CString curDir;
	pList->GetText(nSel, dir);
	for (int i = 1; i < dir.GetLength() - 1; i++)
		curDir += dir[i];
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
		CString destPath = getFolderName() + "/" + curDir;
		CreateDirectory(destPath , NULL);
		MyDownloadFolder(pConnection,path, destPath);
		pFileFind = NULL;
		delete pFileFind;								// ɾ���ļ���ѯ����
		if (pConnection != NULL) {
			pConnection->Close();
			delete pConnection;							// ɾ��FTP���Ӷ���
		}
	}
	delete pSession; 								// ɾ��Internet �Ự���� 
	AfxMessageBox("�ļ������سɹ�");
	return 0;
}

UINT FtpRename(LPVOID  pParam) {
	if (pParam == NULL)
		AfxEndThread(NULL);
	SERVER_INFO* sp;
	sp = (SERVER_INFO*)pParam;
	CListBox* pList;
	CString ip;
	CString user;
	CString pwd;
	CString dicName;
	CString path;
	pList = sp->pList;
	ip = sp->ip;
	user = sp->user;
	pwd = sp->pwd;
	path = sp->path;
	CInternetSession* pSession = NULL;//�Ự
	CFtpConnection* pConnection = NULL;//����
	CFtpFileFind* pFileFind = NULL;//��ѯ
	CString fileName;
	BOOL isContinue = 0;
	int  nSel = pList->GetCurSel();
	CString cur, curName;
	pList->GetText(nSel, cur);
	if (cur.GetAt(0) == '[')
		for (int i = 1; i < cur.GetLength() - 1; i++)
			curName += cur[i];
	else
		curName = cur;
	pSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);
	try {
		pConnection = pSession->GetFtpConnection(ip, user, pwd);
	}
	catch (CInternetException* e) {
		AfxMessageBox("����ʧ��!\n", MB_OK | MB_ICONSTOP);
		e->Delete();
		pConnection = NULL;
	}

	if (pConnection != NULL) {
		CFolder newName;
		newName.DoModal();
		pConnection->Rename(path+"/"+curName, path + "/" + newName.folderName);
	}
	if (pConnection != NULL) {
		pConnection->Close();
		delete pConnection;							// ɾ��FTP���Ӷ���
	}
	delete pSession; 								// ɾ��Internet �Ự���� 
	Query(pParam);
	return 0;
}



