using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
namespace CoopAndreasNET.SDK
{
    public class CCivilianPed : CPed
    {
        public CCivilianPed(IntPtr address) : base(address)
        {

        }


        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate IntPtr CPed__new(uint size);
        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate IntPtr CCivilianPed_Ctor(IntPtr _this, int pt, uint m);
        public CCivilianPed(PedType pedType, uint modelIndex) : base((IntPtr)0x00000)
        {
            IntPtr ptr = Memory.CallFunction<CPed__new>(0x5E4720)(0x79C);
            IntPtr baseaddr = Memory.CallFunction<CCivilianPed_Ctor>(0x5DDB70)(ptr, (int)pedType, modelIndex);
            BaseAddress = baseaddr.ToInt32();
        }
    }
}
