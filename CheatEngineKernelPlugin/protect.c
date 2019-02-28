#include "cekp.h"

#pragma  warning( disable: 4214 4201 )

PVOID g_RegistrationHandle = NULL;
PVOID g_RegistrationHandle2 = NULL;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
	LIST_ENTRY64    InLoadOrderLinks;
	LIST_ENTRY64    InMemoryOrderLinks;
	LIST_ENTRY64    InInitializationOrderLinks;
	PVOID            DllBase;
	PVOID            EntryPoint;
	ULONG            SizeOfImage;
	UNICODE_STRING    FullDllName;
	UNICODE_STRING     BaseDllName;
	ULONG            Flags;
	USHORT            LoadCount;
	USHORT            TlsIndex;
	PVOID            SectionPointer;
	ULONG            CheckSum;
	PVOID            LoadedImports;
	PVOID            EntryPointActivationContext;
	PVOID            PatchInformation;
	LIST_ENTRY64    ForwarderLinks;
	LIST_ENTRY64    ServiceTagLinks;
	LIST_ENTRY64    StaticLinks;
	PVOID            ContextInformation;
	ULONG64            OriginalBase;
	LARGE_INTEGER    LoadTime;
} LDR_DATA_TABLE_ENTRY64, *PLDR_DATA_TABLE_ENTRY64;

typedef struct _OBJECT_TYPE_INITIALIZER
{
	UINT16       Length;
	union
	{
		UINT8        ObjectTypeFlags;
		struct
		{
			UINT8        CaseInsensitive : 1;
			UINT8        UnnamedObjectsOnly : 1;
			UINT8        UseDefaultObject : 1;
			UINT8        SecurityRequired : 1;
			UINT8        MaintainHandleCount : 1;
			UINT8        MaintainTypeList : 1;
			UINT8        SupportsObjectCallbacks : 1;
		};
	};
	ULONG32      ObjectTypeCode;
	ULONG32      InvalidAttributes;
	struct _GENERIC_MAPPING GenericMapping;
	ULONG32      ValidAccessMask;
	ULONG32      RetainAccess;
	enum _POOL_TYPE PoolType;
	ULONG32      DefaultPagedPoolCharge;
	ULONG32      DefaultNonPagedPoolCharge;
	PVOID        DumpProcedure;
	PVOID        OpenProcedure;
	PVOID         CloseProcedure;
	PVOID         DeleteProcedure;
	PVOID         ParseProcedure;
	PVOID        SecurityProcedure;
	PVOID         QueryNameProcedure;
	PVOID         OkayToCloseProcedure;
}OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

typedef struct _OBJECT_TYPE_TEMP
{
	struct _LIST_ENTRY TypeList;
	struct _UNICODE_STRING Name;
	VOID*        DefaultObject;
	UINT8        Index;
	UINT8        _PADDING0_[0x3];
	ULONG32      TotalNumberOfObjects;
	ULONG32      TotalNumberOfHandles;
	ULONG32      HighWaterNumberOfObjects;
	ULONG32      HighWaterNumberOfHandles;
	UINT8        _PADDING1_[0x4];
	struct _OBJECT_TYPE_INITIALIZER TypeInfo;
	ULONG64 TypeLock;
	ULONG32      Key;
	UINT8        _PADDING2_[0x4];
	struct _LIST_ENTRY CallbackList;
}OBJECT_TYPE_TEMP, *POBJECT_TYPE_TEMP;

extern OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);
extern OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);
extern OB_PREOP_CALLBACK_STATUS FilePreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);


NTSTATUS ProtectTheCurrentProcess()
{
	NTSTATUS Status = STATUS_SUCCESS;
	OB_CALLBACK_REGISTRATION obReg;
	OB_OPERATION_REGISTRATION opReg[2];

	if (g_RegistrationHandle == NULL) {
		memset(&opReg, 0, sizeof(opReg)); //初始化结构体变量

		opReg[0].ObjectType = PsProcessType;
		opReg[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		opReg[0].PreOperation = ProcessPreCallback;
		opReg[0].PostOperation = NULL;

		opReg[1].ObjectType = PsThreadType;
		opReg[1].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		opReg[1].PreOperation = ThreadPreCallback;
		opReg[1].PostOperation = NULL;

		memset(&obReg, 0, sizeof(obReg));//初始化结构体变量

		obReg.Version = ObGetFilterVersion();//ObGetFilterVersion();OB_FLT_REGISTRATION_VERSION
		obReg.OperationRegistrationCount = (sizeof(opReg) / sizeof(OB_OPERATION_REGISTRATION));
		obReg.RegistrationContext = (PVOID)PsGetCurrentProcessId();
		RtlInitUnicodeString(&obReg.Altitude, L"401900");
		obReg.OperationRegistration = opReg;

		Status = ObRegisterCallbacks(&obReg, &g_RegistrationHandle); //在这里注册回调函数
	}

	return Status;
}

VOID UnprotectTheCurrentProcess()
{
	if (g_RegistrationHandle != NULL) {
		ObUnRegisterCallbacks(g_RegistrationHandle);
		g_RegistrationHandle = NULL;
	}
}

/// <note>win10 会触发PG</note>
NTSTATUS ProtectTheCurrentProcessFile()
{
	NTSTATUS Status = STATUS_SUCCESS;
	OB_CALLBACK_REGISTRATION obReg;
	OB_OPERATION_REGISTRATION opReg;

	if (g_RegistrationHandle2 == NULL) {
		memset(&opReg, 0, sizeof(opReg)); //初始化结构体变量

		POBJECT_TYPE_TEMP  ObjectTypeTemp = (POBJECT_TYPE_TEMP)*IoFileObjectType;
		ObjectTypeTemp->TypeInfo.SupportsObjectCallbacks = 1;

		opReg.ObjectType = IoFileObjectType;
		opReg.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		opReg.PreOperation = FilePreCallback;
		opReg.PostOperation = NULL;


		memset(&obReg, 0, sizeof(obReg));//初始化结构体变量

		obReg.Version = ObGetFilterVersion();//ObGetFilterVersion();
		obReg.OperationRegistrationCount = (sizeof(opReg) / sizeof(OB_OPERATION_REGISTRATION));
		obReg.RegistrationContext = NULL;
		RtlInitUnicodeString(&obReg.Altitude, L"401900");
		obReg.OperationRegistration = &opReg;

		Status = ObRegisterCallbacks(&obReg, &g_RegistrationHandle2); //在这里注册回调函数
	}

	return Status;
}

VOID UnprotectTheCurrentProcessFile()
{
	if (g_RegistrationHandle2 != NULL) {
		ObUnRegisterCallbacks(g_RegistrationHandle2);
		g_RegistrationHandle2 = NULL;
	}
}

