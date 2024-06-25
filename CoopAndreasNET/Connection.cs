using Riptide;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CoopAndreasNET
{
    public class Connection
    {
        public static Client client;
        public Connection() 
        {
            client = new Client(); 
        }
        [MessageHandler((ushort)Packets.Chat)]
        private static void ChatHandler(Message incomingPacket)
        {
            Console.WriteLine(incomingPacket.GetString().Trim());
        }
        public void Connect(string hostname)
        {
            client.Connect(hostname);
            client.ConnectionFailed += ConnectionFailed;
            client.Connected += Connected;
            client.Disconnected += Disconnected;
        }

        private void Disconnected(object sender, DisconnectedEventArgs e)
        {
            Console.WriteLine($"Disconnected from server. Reason: {e.Reason.ToString()}"); 
        }

        public static void Send(Message message)
        {
            client?.Send(message);
        }
        private void Connected(object sender, EventArgs e)
        {
            Console.WriteLine("Connected to the server");
        }

        public void Disconnect()
        {
            client?.Disconnect();
            client = null;
        }
        private void ConnectionFailed(object sender, ConnectionFailedEventArgs e)
        {
            Console.WriteLine($"Connection to server failed. Reason {e.Reason.ToString()}");
        }


        

    }
    public enum Packets : ushort
    {
        Chat,
        SyncOnFoot,
        Disconnected
    }
}
