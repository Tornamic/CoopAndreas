using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public partial class CVehicle : CPhysical
    {
        public CVehicle(IntPtr Address) : base(Address) { }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate IntPtr _New(int s, int arg);
        public static IntPtr New(int size, int arg)
        {
            return Memory.CallFunction<_New>(0x5BAB20)(0x5BAB00, arg);
        }

    }

}
