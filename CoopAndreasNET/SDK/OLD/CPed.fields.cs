using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using GTASDK;
namespace CoopAndreasNET.SDK
{
    #region Enums
    public enum PedType : uint
    {
        PLAYER1,
        PLAYER2,
        PLAYER_NETWORK,
        PLAYER_UNUSED,
        CIVMALE,
        CIVFEMALE,
        COP,
        GANG1,
        GANG2,
        GANG3,
        GANG4,
        GANG5,
        GANG6,
        GANG7,
        GANG8,
        GANG9,
        GANG10,
        DEALER,
        MEDIC,
        FIREMAN,
        CRIMINAL,
        BUM,
        PROSTITUTE,
        SPECIAL,
        MISSION1,
        MISSION2,
        MISSION3,
        MISSION4,
        MISSION5,
        MISSION6,
        MISSION7,
        MISSION8
    }
    #endregion
    public partial class CPed : CPhysical
    {
        public CPed(IntPtr address): base(address){

        }
        // type: CCollPoly name: polyColliding offset: 0x120 size: 0x28



    }
}
