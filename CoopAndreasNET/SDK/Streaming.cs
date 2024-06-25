using GTASDK.ViceCity;
using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public class Streaming
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate bool _HasModelLoaded(int modelID);
        public static bool HasModelLoaded(int modelID)
        {
            return Memory.CallFunction<_HasModelLoaded>(0x4044C0)(modelID);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void CStreaming__RequestModel(int id, int f);
        public static void RequestModel(int modelIndex, StreamingFlags flags)
        {
            Memory.CallFunction<CStreaming__RequestModel>(0x4087E0)(modelIndex, (int)flags);
        }

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void CStreaming__LoadAllRequestedModels(bool b);
        public static void LoadAllRequestedModels(bool onlyQuickRequests)
        {
            Memory.CallFunction<CStreaming__LoadAllRequestedModels>(0x40EA10)(onlyQuickRequests);
        }

    }
    public enum StreamingFlags
    {
        GameRequest = 0x1,
        MissionRequest = 0x2,
        PriorityRequest = 0x8,
        NotVisible = 0x10
    };
}
