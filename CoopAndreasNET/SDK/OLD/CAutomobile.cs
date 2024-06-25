using GTASDK;
using CoopAndreasNET.SDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public partial class CAutomobile : CVehicle
    {

        public CAutomobile(IntPtr Address) : base(Address) { }

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        public delegate IntPtr _Construtor(IntPtr _t, int m, byte c, bool setupSuspensionLines);
        public CAutomobile(int modelIndex, byte createdBy, bool setupSuspensionLines) : base(IntPtr.Zero)
        {
            IntPtr p = New(0x0A18, 0);
            BaseAddress = Memory.CallFunction<_Construtor>(0x6B0A90)(p, modelIndex, createdBy, setupSuspensionLines).ToInt32();
        }
    }
}
