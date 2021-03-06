#include "OeasyMidSdk.h"
#include "devicefactory.h"
#include "BaseType.h"
#include "oeasylog.h"
#include "OeasyMidDefines.h"
#include "HandleManager.h"
#include "Tick.h"
//hikvison
#include "HCNetSDK.h"
//dahua
#include "avglobal.h"
#include "dhconfigsdk.h"
#include "dhnetsdk.h"

using namespace std;

#define OEASYMID_VERSION "0.0.1"

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		{
			SOCK_INIT
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			SOCK_DEINIT
		}
		break;
	}
	return TRUE;
}
#else
#include <signal.h>
void sigPipe(int sig){}
#endif

OEASY_API void stdcall Oeasy_GetOeasyMidVersion( char* version )
{
	OEASYLOG_I("Oeasy_getOeasyMidVersion");
	OEASY_ASSERT(version,,)
	memcpy(version, OEASYMID_VERSION, sizeof(OEASYMID_VERSION));
}

OEASY_API int stdcall Oeasy_InitAll()
{
	OEASYLOG_I("Oeasy_InitAll");
#ifndef _WIN32
		signal(SIGPIPE, &sigPipe);
#endif
	CTick::GetInstance();
#ifdef _USE_IPC
	//hk
	NET_DVR_Init();
	//NET_DVR_SetLogToFile(0);
	//dahua
	CLIENT_Init(NULL, 0);
#endif
	
	return 0;
}

OEASY_API void stdcall Oeasy_DeinitAll()
{	
	OEASYLOG_I("Oeasy_DeinitAll");
#ifdef _USE_IPC
	//hk
	NET_DVR_Cleanup();
	//dahua
	CLIENT_Cleanup();
#endif
	CHandleManager::ReleaseInstance();
	CTick::ReleaseInstance();
}

OEASY_API void stdcall Oeasy_ReleaseHandle( OEASYHANDLE handle)
{
	OEASYLOG_I("Oeasy_releaseHandle");
	CHandleManager::DeleteApiManager(handle);
}

OEASY_API OEASYHANDLE stdcall Oeasy_CreateHandle()
{
	OEASYLOG_I("Oeasy_CreateHandle");
	OEASYHANDLE handle = OEASY_HANDLE_INVALIDE;
	CApiImpl* pCore = CHandleManager::CreateApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	return handle;
}


OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_Create(OEASYHANDLE handle, CAMERATYPE cameraType)
{
	int ret  = -1;
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_Init handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	ret = pCore->IPC_Create(cameraType);
	CHandleManager::ReleaseApiManager(handle);
#endif
	return ret;
}

OEASY_API int  stdcall OeasyMidIPC::Oeasy_IPC_Destroy(OEASYHANDLE handle)
{
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_Deinit handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	pCore->IPC_Destroy();
	CHandleManager::ReleaseApiManager(handle);
#endif
	return 0;
}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_Login( OEASYHANDLE handle, OEASY_LOGINFO* loginfo, OEASY_LOGIN_RESULTINFO* resultInfo )
{
	int ret = -1;
#ifdef _USE_IPC
	OEASY_ASSERT(loginfo && resultInfo,,-1)
	OEASYLOG_I("Oeasy_IPC_Login handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	ret = pCore->IPC_Login(loginfo, resultInfo);
	CHandleManager::ReleaseApiManager(handle);
#endif
	return ret;

}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_Logout( OEASYHANDLE handle )
{
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_Logout handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	pCore->IPC_LoginOut();
	CHandleManager::ReleaseApiManager(handle);
#endif
	return 0;

}

OEASY_API IPCLiveHandle stdcall OeasyMidIPC::Oeasy_IPC_StartLive( OEASYHANDLE handle, STREAMTYPE streamtype, bool bstartsms /*= false*/, char* mediaserverurl /*= ""*/ )
{
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_StartLive handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	pCore->IPC_StartLive(streamtype, bstartsms, mediaserverurl);
	CHandleManager::ReleaseApiManager(handle);
	return 0;
#else
	return 0;
#endif
}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_StopLive( OEASYHANDLE handle, IPCLiveHandle livehandle )
{
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_StopLive handle = %d ,livehandle = %d", handle, livehandle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	pCore->IPC_StopLive(livehandle);
	CHandleManager::ReleaseApiManager(handle);
	return 0;
#else
return 0;
#endif
}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_SetLiveDataCB( OEASYHANDLE handle, LIVEDATACALLBACK videoDataCB, void *pUser )
{
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_SetLiveDataCB handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	pCore->IPC_SetLiveDataCB(videoDataCB, pUser);
	CHandleManager::ReleaseApiManager(handle);
#endif
	return 0;
}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_CaptureImage( OEASYHANDLE handle, unsigned char *picBuffer, unsigned long bufferSize, unsigned long* sizeReturned )
{
	int ret = 0;
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_CaptureImage handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	ret = pCore->IPC_CaptureImage(picBuffer, bufferSize, sizeReturned);
	CHandleManager::ReleaseApiManager(handle);
#endif
	return ret;
}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_SetAlarmParam( OEASYHANDLE handle, ALARMSETTING *param )
{
	int ret = 0;
#ifdef _USE_IPC

	OEASYLOG_I("Oeasy_IPC_SetAlarmParam handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	ret = pCore->IPC_SetAlarmParam(param);
	CHandleManager::ReleaseApiManager(handle);
#endif
	return ret; 
}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_StartAlarm( OEASYHANDLE handle, ALARMMESGCALLBACK alarmMsgCB, void *pUser )
{
	int ret = 0;
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_StartAlarm handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	ret = pCore->IPC_StartAlarm(alarmMsgCB, pUser);
	CHandleManager::ReleaseApiManager(handle);
#endif
	return ret; 
}

OEASY_API int stdcall OeasyMidIPC::Oeasy_IPC_StopAlarm( OEASYHANDLE handle )
{
	int ret = 0;
#ifdef _USE_IPC
	OEASYLOG_I("Oeasy_IPC_StopAlarm handle = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	OEASY_ASSERT (pCore, {}, handle);
	ret = pCore->IPC_StopAlarm();
	CHandleManager::ReleaseApiManager(handle);
#endif
	return ret; 
}

OEASY_API void stdcall OeasyMidIntercom::Oeasy_Intercom_startServer( OEASYHANDLE handle, unsigned short serverPort )
{
#ifdef _USE_INTERCOM
	OEASYLOG_I("Oeasy_Intercom_startServer = %d, serverport = %d", handle, serverPort);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	pCore->Intercom_startServer(serverPort);
	CHandleManager::ReleaseApiManager(handle);
#endif

}

OEASY_API void stdcall OeasyMidIntercom::Oeasy_Intercom_stopServer( OEASYHANDLE handle )
{
#ifdef _USE_INTERCOM
	OEASYLOG_I("Oeasy_Intercom_stopServer = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	pCore->Intercom_stopServer();
	CHandleManager::ReleaseApiManager(handle);
#endif
}

OEASY_API int stdcall OeasyMidIntercom::Oeasy_Intercom_Create( OEASYHANDLE handle, INTERCOMTYPE type )
{
#ifdef _USE_INTERCOM
	OEASYLOG_I("Oeasy_Intercom_Create = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	return pCore->Intercom_Create(type);
	CHandleManager::ReleaseApiManager(handle);
#endif
}

OEASY_API void stdcall OeasyMidIntercom::Oeasy_Intercom_Destroy( OEASYHANDLE handle )
{
#ifdef _USE_INTERCOM
	OEASYLOG_I("Oeasy_Intercom_Destroy = %d", handle);
	CApiImpl* pCore = CHandleManager::GetApiManager(handle);
	pCore->Intercom_Destroy();
	CHandleManager::ReleaseApiManager(handle);
#endif
}
