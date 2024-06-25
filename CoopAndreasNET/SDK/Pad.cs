using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public class Pad
    {
        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        private delegate uint CPlayerPed__GetPadFromPlayer(uint ptr);
        

        public static void InitPads(PlayerPed player)
        {
            player.Pad = Memory.CallFunction<CPlayerPed__GetPadFromPlayer>(0x609560)(player.Pointer);
        }
        public static void SetPadState(PlayerPed player, ControllerState state)
        {
            Memory.WriteInt16((int)(player.Pad + 0x0), state.LeftStickX);       //short LeftStickX; // move/steer left (-128?)/right (+128)
            Memory.WriteInt16((int)(player.Pad + 0x2), state.LeftStickY);       //short LeftStickY; // move back(+128)/forwards(-128?)
            Memory.WriteInt16((int)(player.Pad + 0xE), state.RightShoulder1);   //short RightShoulder1; // target / hand brake
            Memory.WriteInt16((int)(player.Pad + 0x1E), state.ButtonSquare);    //short ButtonSquare; // jump / reverse      Break/Reverse / Jump/Climb
            Memory.WriteInt16((int)(player.Pad + 0x20), state.ButtonTriangle);  //short ButtonTriangle; // get in/out        Exit vehicle / Enter veihcle
            Memory.WriteInt16((int)(player.Pad + 0x22), state.ButtonCross);     //short ButtonCross; // sprint / accelerate  Accelerate / Sprint/Swim
            Memory.WriteInt16((int)(player.Pad + 0x24), state.ButtonCircle);    //short ButtonCircle; // fire                Fire weapon
            Memory.WriteInt16((int)(player.Pad + 0x2C), state.PedWalk);         //short m_bPedWalk; // walk
        }
        public static ControllerState GetPadState(PlayerPed player)
        {
            if (player.Pad == 0) InitPads(player);
            return new ControllerState()
            {
                LeftStickX = Memory.ReadInt16((int)(player.Pad + 0x0)),       //short LeftStickX; // move/steer left (-128?)/right (+128)
                LeftStickY = Memory.ReadInt16((int)(player.Pad + 0x2)),       //short LeftStickY; // move back(+128)/forwards(-128?)
                RightShoulder1 = Memory.ReadInt16((int)(player.Pad + 0xE)),   //short RightShoulder1; // target / hand brake
                ButtonSquare = Memory.ReadInt16((int)(player.Pad + 0x1E)),    //short ButtonSquare; // jump / reverse      Break/Reverse / Jump/Climb
                ButtonTriangle = Memory.ReadInt16((int)(player.Pad + 0x20)),  //short ButtonTriangle; // get in/out        Exit vehicle / Enter veihcle
                ButtonCross = Memory.ReadInt16((int)(player.Pad + 0x22)),     //short ButtonCross; // sprint / accelerate  Accelerate / Sprint/Swim
                ButtonCircle = Memory.ReadInt16((int)(player.Pad + 0x24)),    //short ButtonCircle; // fire                Fire weapon
                PedWalk = Memory.ReadInt16((int)(player.Pad + 0x2C))         //short m_bPedWalk; // walk
            };

        }
        
    }
    public struct ControllerState
    {
        public short LeftStickX; // move/steer left (-128?)/right (+128)
        public short LeftStickY; // move back(+128)/forwards(-128?)
        public short RightShoulder1; // target / hand brake
        public short ButtonSquare; // jump / reverse      Break/Reverse / Jump/Climb
        public short ButtonTriangle; // get in/out        Exit vehicle / Enter veihcle
        public short ButtonCross; // sprint / accelerate  Accelerate / Sprint/Swim
        public short ButtonCircle; // fire                Fire weapon
        public short PedWalk; // walk

        public bool IsEquals(ControllerState state)
        {
            return ((LeftStickX == state.LeftStickX) && 
                (LeftStickY == state.LeftStickY) &&
                (RightShoulder1 == state.RightShoulder1) &&
                (ButtonSquare == state.ButtonSquare) &&
                (ButtonTriangle == state.ButtonTriangle) &&
                (ButtonCross == state.ButtonCross) &&
                (ButtonCircle == state.ButtonCircle) &&
                (PedWalk == state.PedWalk));
        }

        public override string ToString()
        {
            return $"{LeftStickX}, {LeftStickY}, {RightShoulder1}, {ButtonSquare}, {ButtonTriangle}, {ButtonCross}, {ButtonCircle}, {PedWalk}";
        }
    }
}
