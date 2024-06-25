using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public partial class CPed
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate IntPtr _FindPlayerPed();
        public static CPed FindPlayerPed()
        {
            IntPtr ptr = Memory.CallFunction<_FindPlayerPed>(0x56E210)();
            return new CPed(ptr);
        }
    }
}
