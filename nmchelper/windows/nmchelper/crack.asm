.model flat
.data
__imp__EncodePointer@4 dd dummy4
__imp__DecodePointer@4 dd dummy4
__imp__InterlockedPopEntrySList@4 dd dummy4
__imp__InterlockedPushEntrySList@8 dd dummy8
EXTERNDEF __imp__EncodePointer@4 : DWORD
EXTERNDEF __imp__DecodePointer@4 : DWORD
EXTERNDEF __imp__InterlockedPopEntrySList@4 : DWORD
EXTERNDEF __imp__InterlockedPushEntrySList@8 : DWORD
.code
dummy4 proc
mov eax, [esp+4]
ret 4
dummy4 endp
dummy8 proc
mov eax, [esp+4]
ret 8
dummy8 endp
end