using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CoopAndreasNET.SDK
{
    public class PlayerPed : Ped
    {
        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void CPlayerPed__SetRealMoveAnim(uint ptr);

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void CPlayerPed__SetPlayerMoveBlendRatio(uint ptr, CVector arg);

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate uint CPlayerPed__ProcessControl(uint ptr);

        public PlayerPed(uint id, uint ptr) : base(id, ptr) { }
        public static PlayerPed Player
        {
            get
            {
                uint ptrNew = 0;
                Scripting.opcode_get_actor_ptr(1, ref ptrNew);
                return new PlayerPed(1, ptrNew);
            }
        }
        //public float MoveBlendRatio
        //{
        //    get 
        //    {
        //        uint playerDataPtr = Memory.ReadUInt32((int)(ptr + 0x480));
        //        return Memory.ReadFloat((int)(playerDataPtr + 0x14)); 
        //    }
        //    set 
        //    {
        //        //uint playerDataPtr = Memory.ReadUInt32((int)(ptr + 0x480));
        //        //Memory.WriteFloat((int)(playerDataPtr + 0x14), value); 
        //        Memory.CallFunction<CPlayerPed__SetPlayerMoveBlendRatio>(0x60C520)
        //    }
        //}
        public PlayerPed(uint playerid, CVector pos)
        {
            uint dwPlayerID = playerid;
            Streaming.RequestModel(0, StreamingFlags.GameRequest);
            Streaming.LoadAllRequestedModels(false);

            Scripting.opcode_create_player(ref dwPlayerID, pos.X, pos.Y, pos.Z, ref gtaid);
            Scripting.opcode_create_actor_from_player(ref dwPlayerID, ref gtaid);
            Scripting.opcode_get_actor_ptr(gtaid, ref ptr);

            SDK.Pad.InitPads(this);
        }

        public uint Pad = 0;
        
        public void ProcessControl()
        {
            Memory.CallFunction<CPlayerPed__ProcessControl>(0x60EA90)(ptr);
        }
        
        public void SetRealMoveAnim()
        {
            Memory.CallFunction<CPlayerPed__SetRealMoveAnim>(0x60A9C0)(ptr);
        }
    }
}
