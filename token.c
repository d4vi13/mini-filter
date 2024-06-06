#include "token.h"
#include "common.h"

static inline void ImpersonationLevelCopy(
	PSECURITY_IMPERSONATION_LEVEL Dest,
	PSECURITY_IMPERSONATION_LEVEL Src,
	TOKEN_TYPE Type
) {
	if (Type == TokenImpersonation)
		RtlCopyMemory(Dest, Src, sizeof *Src);
	else
		RtlZeroMemory(Dest, sizeof * Dest);
}

void TokenInfoCopy(PTOKEN_INFO_STATIC Dest, PTOKEN_INFO Src) {
	ULONG PrivSize = sizeof * Src->Privileges.Privileges * Src->Privileges.PrivilegeCount;

	RtlCopyMemory(&Dest->Type, &Src->Type, sizeof Src->Type);
	RtlCopyMemory(&Dest->TokenElevation, &Src->TokenElevation, sizeof Src->TokenElevation);
	
	ImpersonationLevelCopy(&Dest->ImpersonationLevel, &Src->ImpersonationLevel, Src->Type);

	Dest->Privileges.PrivilegeCount = Src->Privileges.PrivilegeCount;

	RtlCopyMemory(Dest->Privileges.Privileges, Src->Privileges.Privileges, PrivSize);
}

/**
 * AcquireTokenInfo - Acquires various token information.
 * @TokenHandle: Handle to the token.
 * @TokenInfo: Pointer to a TOKEN_INFO structure to receive the token information.
 *
 * This function queries the specified token for its type, privileges, elevation,
 * and impersonation level, and stores this information in the provided TOKEN_INFO
 * structure. It returns an NTSTATUS code indicating success or failure.
 *
 * Return: NTSTATUS code.
 */
NTSTATUS AcquireTokenInfo(
	_In_ HANDLE TokenHandle,
	_Out_ PTOKEN_INFO TokenInfo)
{
	NTSTATUS Status;
	ULONG ReturnLength;

	Status = NtQueryInformationToken(TokenHandle, TokenType, &TokenInfo->Type, sizeof(TokenInfo->Type), &ReturnLength);
	if (!NT_SUCCESS(Status))
		return Status;

	Status = NtQueryInformationToken(TokenHandle, TokenPrivileges, &TokenInfo->Privileges, sizeof(TokenInfo->Privileges), &ReturnLength);
	if (!NT_SUCCESS(Status))
		return Status;

	Status = NtQueryInformationToken(TokenHandle, TokenElevation, &TokenInfo->TokenElevation, sizeof(TokenInfo->TokenElevation), &ReturnLength);
	if (!NT_SUCCESS(Status))
		return Status;

	Status = NtQueryInformationToken(TokenHandle, TokenImpersonationLevel, &TokenInfo->ImpersonationLevel, sizeof(TokenInfo->ImpersonationLevel), &ReturnLength);
	if (!NT_SUCCESS(Status))
		return Status;

	return STATUS_SUCCESS;
}

/**
 * TokenInfoGet - Retrieves token information for a specified process.
 * @TokenInfo: Pointer to a TOKEN_INFO structure to receive the token information.
 * @PID: Handle to the process.
 *
 * This function opens the process token for the specified process and retrieves
 * various token information by calling AcquireTokenInfo. It returns an NTSTATUS
 * code indicating success or failure.
 *
 * Return: NTSTATUS code.
 */
NTSTATUS TokenInfoGet(
	_Out_ PTOKEN_INFO TokenInfo,
	_In_ HANDLE PID)
{
	NTSTATUS Status;
	HANDLE TokenHandle;

	Status = NtOpenProcessToken(PID, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &TokenHandle);
	if (!NT_SUCCESS(Status))
		return Status;

	Status = AcquireTokenInfo(TokenHandle, TokenInfo);
	NtClose(TokenHandle); // Ensure to close the token handle after use

	return Status;
}
