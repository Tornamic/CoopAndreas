using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public class CWorld
    {


        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void CWorld__Add(IntPtr ptr);
        public static void Add(CEntity entity)
        {
            Memory.CallFunction<CWorld__Add>(0x563220)((IntPtr)entity.BaseAddress);
        }
    }
}
