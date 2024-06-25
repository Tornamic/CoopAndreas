using Riptide;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;

namespace Server.Sync
{
    public class OnFoot
    {
        [MessageHandler((ushort)Packets.SyncOnFoot)]
        private static void OnFootHandler(ushort fromClientId, Message incomingPacket)
        {
            OnFootSyncData onFootSyncData = new OnFootSyncData()
            {
                position = new CVector(incomingPacket.GetFloat(), incomingPacket.GetFloat(), incomingPacket.GetFloat()),
                velocity = new CVector(incomingPacket.GetFloat(), incomingPacket.GetFloat(), incomingPacket.GetFloat()),
                heading = incomingPacket.GetFloat(),
                moveState = incomingPacket.GetByte(),
                keys = incomingPacket.GetShorts(),
                moveBlendRatio = incomingPacket.GetFloat(),
            };


            Message message = Message.Create(MessageSendMode.Unreliable, Packets.SyncOnFoot);

            // player id
            message.AddUShort(fromClientId);

            // position
            message.AddFloat(onFootSyncData.position.X);
            message.AddFloat(onFootSyncData.position.Y);
            message.AddFloat(onFootSyncData.position.Z);

            // velocity
            message.AddFloat(onFootSyncData.velocity.X);
            message.AddFloat(onFootSyncData.velocity.Y);
            message.AddFloat(onFootSyncData.velocity.Z);

            // heading
            message.AddFloat(onFootSyncData.heading);

            // move state
            message.AddByte(onFootSyncData.moveState);

            // keys
            message.AddShorts(onFootSyncData.keys);

            // moveBlendRatio
            message.AddFloat(onFootSyncData.moveBlendRatio);

            Connection.server.SendToAll(message, fromClientId);
        }
    }
    public struct OnFootSyncData
    {
        public CVector position;
        public CVector velocity;
        public float heading;
        public byte moveState;
        public short[] keys;
        public float moveBlendRatio;
    }
}
