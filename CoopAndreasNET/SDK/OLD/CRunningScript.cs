using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public class CRunningScript
    {
        public int BaseAddress;

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void CRunningScript__Init(IntPtr _this);
        public CRunningScript(IntPtr Address)
        {
            BaseAddress = Address.ToInt32();
            Memory.CallFunction<CRunningScript__Init>(0x4386C0)(Address);
        } 

        public CRunningScript Next => new CRunningScript((IntPtr)Memory.ReadInt32(BaseAddress + 0x0));
        public CRunningScript Previous => new CRunningScript((IntPtr)Memory.ReadInt32(BaseAddress + 0x4));


        public string Name
        {
            get => Memory.ReadString(BaseAddress + 0x8, 7);
            set => Memory.WriteString(BaseAddress + 0x8, value);
        }

        public int IP
        {
            get => Memory.ReadInt32(BaseAddress + 0x14);
            set => Memory.WriteInt32(BaseAddress + 0x14, value);
        }

        public bool IsActive
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.Read1bBool(BaseAddress + 0x00C4);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.Write1bBool(BaseAddress + 0x00C4, value);
        }
        public bool CondResult
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.Read1bBool(BaseAddress + 0x00C5);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.Write1bBool(BaseAddress + 0x00C5, value);
        }
        public bool UseMissionCleanup
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.Read1bBool(BaseAddress + 0x00C6);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.Write1bBool(BaseAddress + 0x00C6, value);
        }
        public int WakeTime
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.ReadInt32(BaseAddress + 0x00CC);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.WriteInt32(BaseAddress + 0x00CC, value);
        }
        public short LogicalOp
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.ReadInt16(BaseAddress + 0x00D0);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.WriteInt16(BaseAddress + 0x00D0, value);
        }
        public bool NotFlag
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.Read1bBool(BaseAddress + 0x00D2);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.Write1bBool(BaseAddress + 0x00D2, value);
        }
        public bool WastedBustedCheck
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.Read1bBool(BaseAddress + 0x00D3);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.Write1bBool(BaseAddress + 0x00D3, value);
        }
        public bool WastedOrBusted
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.Read1bBool(BaseAddress + 0x00D4);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.Write1bBool(BaseAddress + 0x00D4, value);
        }
        public bool IsMission
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.Read1bBool(BaseAddress + 0x00DC);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.Write1bBool(BaseAddress + 0x00DC, value);
        }

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        public delegate void _PorcessOneCommand(IntPtr _this);
        public void ProcessOneCommand()
        {
            Memory.CallFunction<_PorcessOneCommand>(0x44FBE0)((IntPtr)BaseAddress);
        }
    }
}