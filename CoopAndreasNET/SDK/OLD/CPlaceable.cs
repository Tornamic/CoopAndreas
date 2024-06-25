using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace CoopAndreasNET.SDK
{

    public class CPlaceable : CMatrix
    {
        public CPlaceable(IntPtr address)
        {
            BaseAddress = address.ToInt32();
        }
    }
}
