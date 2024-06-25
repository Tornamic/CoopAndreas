using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using GTASDK;

namespace CoopAndreasNET.SDK
{
    public partial class CEntity
    {
        public int BaseAddress;

        public CEntity(IntPtr address)
        {
            BaseAddress = address.ToInt32();
        }

        public CVector Position
        {
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            get => Memory.ReadVector(BaseAddress + 0x4);
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            set => Memory.WriteVector(BaseAddress + 0x4, value);
        }

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void _SetModelIndex(IntPtr _this, short modelindex);
        public virtual void SetModelIndex(short modelIndex)
        {
            Memory.CallFunction<_SetModelIndex>(0x5E4880)((IntPtr)BaseAddress, modelIndex);
        }

    }

}
