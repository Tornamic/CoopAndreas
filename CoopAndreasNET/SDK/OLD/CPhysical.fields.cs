using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using GTASDK;

namespace CoopAndreasNET.SDK
{

    public class CPhysical : CEntity
    {
        public const float DefaultGravity = 0.008f;

        public CPhysical(IntPtr address) :base(address)
        {

        }
    }

}
