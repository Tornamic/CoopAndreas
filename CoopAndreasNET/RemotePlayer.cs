using CoopAndreasNET.SDK;
using Riptide;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.Sync
{
    public class RemotePlayer
    {
        public static RemotePlayer[] All = { null, null, null, null };
        public PlayerPed ped;
        //public Ped ped;
        public OnFootSyncData OnFootSyncData;
        public ushort NetID { get; set; }

        public RemotePlayer(int id)
        {
            if (All[id] != null) All[id].Destroy();

            ped = new PlayerPed(1, PlayerPed.Player.Position);
            //ped = new Ped(PedType.BUM, 0, Ped.Player.Position);

            All[id] = this;

            Scripting.ScriptCommand((ushort)Command.set_actor_immunities, "iiiiii", __arglist(ped.ID, 1, 1, 1, 1, 1));
            Scripting.ScriptCommand(0x07BC, "ii", __arglist(ped.ID, 65545)); //07BC: set_actor -1 decision_maker_to 236@
            Scripting.ScriptCommand(0x0350, "ii", __arglist(ped.ID, 1)); //0350: set_actor 2@ maintain_position_when_attacked 1
            Scripting.ScriptCommand(0x039E, "ii", __arglist(ped.ID, 1)); //039E: set_actor 65@ locked 1 while_in_car
            Scripting.ScriptCommand(0x08C6, "ii", __arglist(ped.ID, 1)); //08C6: set_actor 65@ stay_on_bike 1
            Scripting.ScriptCommand(0x087E, "ii", __arglist(ped.ID, 0)); //087E: set_actor 35@ weapon_droppable 0
            ped.Health = 0x7F800000;
            OnFootSyncData.keys = new short[8];
        }

        public void Destroy()
        {
            All[NetID] = null;
            ped.Destroy();
        }
        [MessageHandler((ushort)Packets.Disconnected)]
        private static void DisconnectedHandler(Message incomingPacket)
        {
            ushort id = (ushort)(incomingPacket.GetUShort() - 1);
            if (All[id] != null) All[id].Destroy();
            Console.WriteLine($"Destroyed player id {id}");
        }
    }
}