using Riptide;
using Riptide.Transports;
using Riptide.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Server
{
    public class Connection
    {
        public static Riptide.Server server;
        private Thread updateThread;
        private bool running = false;
        public Connection() 
        {
            server = new Riptide.Server();
            server.ClientConnected += ClientConnected;
            server.ClientDisconnected += ClientDisconnected;
            server.TimeoutTime = 5000;
        }

        [MessageHandler((ushort)Packets.Chat)]
        private static void ChatHandler(ushort fromClientId, Message incomingPacket)
        {
            string message = $"[{fromClientId}] Says: {incomingPacket.GetString().Trim()}";
            Console.WriteLine(message);

            Message outcomingPacket = Message.Create(MessageSendMode.Reliable, Packets.Chat);
            outcomingPacket.AddString(message);

            server.SendToAll(outcomingPacket);
        }

        public void Start(ushort port, ushort maxConnections)
        {
            server.Start(port, maxConnections);
            
            running = true;
            updateThread = new Thread(() =>
            {
                while (running)
                {
                    server.Update();
                    Thread.Sleep(1);
                }
            });
            updateThread.Start();
            Console.WriteLine($"Server succesfuly running on {port} with max players {maxConnections}");
        }
        public void Stop()
        {
            running = false;
            server.Stop();
        }
        private void ClientDisconnected(object sender, ServerDisconnectedEventArgs e)
        {
            Console.WriteLine($"Player {e.Client.Id} disconnected. Reason: {e.Reason.ToString()}");

            Message message = Message.Create(MessageSendMode.Reliable, Packets.Disconnected);
            message.AddUShort(e.Client.Id);

            server.SendToAll(message);
        }

        private void ClientConnected(object sender, ServerConnectedEventArgs e)
        {
            Console.WriteLine($"Player {e.Client.Id} connected.");
        }
    }
    public enum Packets : ushort
    {
        Chat,
        SyncOnFoot,
        Disconnected
    }
}
