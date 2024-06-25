using GTASDK;
using System;
using System.Runtime.InteropServices;

namespace CoopAndreasNET.SDK
{
    public class Ped
    {
        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void CPed__SetMoveState(uint ptr, byte moveState);

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void CPed__ApplyMoveSpeed(uint ptr);

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate void CPed__SetMoveAnim(uint ptr);

        protected uint gtaid = 0;
        protected uint ptr = 0;

        public uint ID
        {
            get { return gtaid; }
        }
        public uint Pointer
        {
            get { return ptr; }
        }

        public float Heading
        {
            get { return Memory.ReadFloat((int)(ptr + 0x558)) * 57.2957f; }
            set
            {
                Memory.WriteFloat((int)(ptr + 0x558), value / 57.2957f);
                Memory.WriteFloat((int)(ptr + 0x55C), value / 57.2957f);
            }
        }
        public CVector Velocity
        {
            get { return Memory.ReadVector((int)(ptr + 0x14 + 0x30)); }
            set { Memory.WriteVector((int)(ptr + 0x14 + 0x30), value); }
        }
        public CVector Position
        {
            get
            {
                uint matptr = Memory.ReadUInt32((int)(ptr + 0x14));
                return Memory.ReadVector((int)(matptr + 0x30));
            }
            set
            {
                uint matptr = Memory.ReadUInt32((int)(ptr + 0x14));
                Memory.WriteVector((int)(matptr + 0x30), value);
            }
        }
        public float Health
        {
            get { return Memory.ReadFloat((int)(ptr + 0x540)); }
            set { Memory.WriteFloat((int)(ptr + 0x540), value); }
        }
        public byte MoveState
        {
            get { return Memory.ReadByte((int)(ptr + 0x534)); }
            set { Memory.CallFunction<CPed__SetMoveState>(0x5DEC00)(ptr, value); }
        }
        public bool OnGround
        {
            get { return Memory.ReadByte((int)(ptr + 0x46C)) == 3; }
        }
        public bool IsDucked
        {
            get { return Memory.ReadByte((int)(ptr + 0x46F)) == 132; }
        }

        // Constructors
        public Ped() { }
        public Ped(uint id, uint ptr)
        {
            gtaid = id;
            this.ptr = ptr;
        }
        public Ped(PedType pedType, int modelID, float x, float y, float z, float angleZ = 0.0f)
        {
            Streaming.RequestModel(modelID, StreamingFlags.GameRequest);
            Streaming.LoadAllRequestedModels(false);
            Scripting.opcode_create_actor((int)pedType, modelID, x, y, z, ref gtaid);
            Scripting.opcode_get_actor_ptr(gtaid, ref ptr);
            Heading = angleZ;
        }
        public Ped(PedType pedType, int modelID, CVector pos, float angleZ = 0.0f)
        {
            Streaming.RequestModel(modelID, StreamingFlags.GameRequest);
            Streaming.LoadAllRequestedModels(false);
            Scripting.opcode_create_actor((int)pedType, modelID, pos.X, pos.Y, pos.Z, ref gtaid);
            Scripting.opcode_get_actor_ptr(gtaid, ref ptr);
            Heading = angleZ;
        }
        public virtual void Destroy()
        {
            Scripting.ScriptCommand((ushort)Command.destroy_actor, "i", __arglist(gtaid));
        }
        public void ApplyMoveSpeed()
        {
            Memory.CallFunction<CPed__ApplyMoveSpeed>(0x542DD0)(ptr);
        }
        public void Duck(bool duck)
        {
            if(duck)
                Scripting.ScriptCommand(0x04EB, "ii", __arglist(gtaid, 1)); // 04EB: AS_actor 65@ crouch 1
            else
                Scripting.ScriptCommand(0x0792, "i", __arglist(gtaid)); // 0792: disembark_instantly_actor $PLAYER_ACTOR

        }
        public void SetMoveAnim()
        {
            Memory.CallFunction<CPed__SetMoveAnim>(0x5E4A00)(ptr);
        }
    }
    public enum PedType : int
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
    public enum MoveState : byte
    {
        NONE = 0,
        STILL,
        TURN_L,
        TURN_R,
        WALK,
        JOG,
        RUN,
        SPRINT,
        DUCK
    }

}
