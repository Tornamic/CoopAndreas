using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public class World
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void CWorld__Add(uint ptr);
        public static void Add(uint ptr)
        {
            Memory.CallFunction<CWorld__Add>(0x563220)(ptr);
        }
        /*
        public static float DistanceBetweenPlayers
        {
            get
            {
                return Memory.ReadFloat(0x96AB24);
            }
        }

        public static float MaxDistanceBetweenPlayers
        {
            get
            {
                return Memory.ReadFloat(0x8A5E4C);
            }
            set
            {
                Memory.WriteFloat(0x8A5E4C, value);
            }
        }
        */
    }
}
