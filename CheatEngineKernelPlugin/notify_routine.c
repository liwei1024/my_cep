#include "cekp.h"


#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)  
#define PROCESS_SET_LIMITED_INFORMATION    (0x2000) 

extern UNICODE_STRING  GetFilePathByFileObject(PVOID FileObject);

/// <summary>打开)进程通知</summary>
OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	if (RegistrationContext == NULL)
	{
		return OB_PREOP_SUCCESS;
	}
	HANDLE pid = PsGetProcessId((PEPROCESS)OperationInformation->Object);
	if (OperationInformation->ObjectType == *PsProcessType && pid == (HANDLE)RegistrationContext)
	{
		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE || OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
		{
			ACCESS_MASK DesiredAccess = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;

			DesiredAccess = DesiredAccess & (
				PROCESS_TERMINATE |
				PROCESS_QUERY_LIMITED_INFORMATION |
				PROCESS_SUSPEND_RESUME |
				PROCESS_VM_OPERATION |
				PROCESS_VM_READ |
				PROCESS_VM_WRITE
				);
			if (DesiredAccess) {
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~DesiredAccess;// DesiredAccess;
			}
		}
	}
	return OB_PREOP_SUCCESS;
}

/// <summary>打开线程通知</summary>
OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	ACCESS_MASK DesiredAccess;
	if (RegistrationContext == NULL)
		return OB_PREOP_SUCCESS;
	if (PsGetCurrentProcessId() == (HANDLE)RegistrationContext)
		return OB_PREOP_SUCCESS;
	if (OperationInformation->ObjectType == *PsThreadType)
	{
		if ((HANDLE)RegistrationContext == PsGetThreadProcessId((PETHREAD)OperationInformation->Object))
		{
			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE || OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
			{
				DesiredAccess = OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess;

				DesiredAccess = DesiredAccess & (THREAD_SET_LIMITED_INFORMATION | THREAD_QUERY_LIMITED_INFORMATION);
				if (DesiredAccess) {
					OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess &= ~DesiredAccess;
				}
			}
		}
	}
	return OB_PREOP_SUCCESS;
}

/// <summary>文件操作 通知</summary>
OB_PREOP_CALLBACK_STATUS FilePreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING uniFilePath;
	UNICODE_STRING currentProcessFilePath;
	PFILE_OBJECT FileObject = (PFILE_OBJECT)OperationInformation->Object;
	HANDLE CurrentProcessId = PsGetCurrentProcessId();

	UNREFERENCED_PARAMETER(status);
	UNREFERENCED_PARAMETER(uniFilePath);
	UNREFERENCED_PARAMETER(FileObject);
	UNREFERENCED_PARAMETER(CurrentProcessId);
	UNREFERENCED_PARAMETER(RegistrationContext);


	if (OperationInformation->ObjectType != *IoFileObjectType)
	{
		return OB_PREOP_SUCCESS;
	}
	//过滤无效指针
	if (FileObject->FileName.Buffer == NULL ||
		!MmIsAddressValid(FileObject->FileName.Buffer) ||
		FileObject->DeviceObject == NULL ||
		!MmIsAddressValid(FileObject->DeviceObject))
	{
		return OB_PREOP_SUCCESS;
	}

	uniFilePath = GetFilePathByFileObject(FileObject);
	if (uniFilePath.Buffer == NULL || uniFilePath.Length == 0)
	{
		return OB_PREOP_SUCCESS;
	}
	
	RtlInitUnicodeString(&currentProcessFilePath, (PCWSTR)PsGetProcessImageFileName(PsGetCurrentProcess()));

	if (RtlEqualUnicodeString(&uniFilePath, &currentProcessFilePath, TRUE))
	{
		if (FileObject->DeleteAccess == TRUE || FileObject->WriteAccess == TRUE)
		{
			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
			{
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			}
			if (OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
			{
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
			}
		}
	}
	return OB_PREOP_SUCCESS;
}

/// <summary>(创建/结束)进程通知</summary>
VOID ProcessNotify(_Inout_  PEPROCESS Process,_In_ HANDLE ProcessId,_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo)
{
	UNREFERENCED_PARAMETER(Process);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(CreateInfo);

	if (NULL != CreateInfo)
	{
		
	}
	else {
		
	}
}

/// <summary>模块加载回调通知</summary>
VOID NotifyImageLoadCallback(_In_opt_ PUNICODE_STRING FullImageName,_In_ HANDLE ProcessId,  _In_ PIMAGE_INFO ImageInfo
)
{
	UNREFERENCED_PARAMETER(FullImageName);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ImageInfo);
	
}
