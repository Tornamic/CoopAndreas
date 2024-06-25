using GTASDK;
using Riptide;
using GTASDK.SanAndreas;
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Message = Riptide.Message;
using Riptide.Transports;
using System.Drawing;
using CoopAndreasNET.Sync;
using CoopAndreasNET.SDK;
using System.IO;
using System.Diagnostics;
using Scripting = CoopAndreasNET.SDK.Scripting;
using static System.Windows.Forms.AxHost;
namespace CoopAndreasNET
{
    public class CoopAndreasNET : SAPlugin
    {
        public delegate void OnGameInit();
        public event OnGameInit onInit;
        public delegate void OnGameExit();
        public event OnGameExit onExit;
        public static Connection connection;

        Random rand;
        public CoopAndreasNET(string[] cmdLine)
        {
            Memory.Hook((IntPtr)0x57D860, new Memory.VoidDelegate(__OnGameExit));
            rand = new Random();
            AllocConsole();
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            SAPlugin.GameTicking += SAPlugin_GameTicking;
            
            CoopAndreasNET_onInit();
        }
        private void __OnGameExit()
        {
            connection.Disconnect();
            onExit();
        }
        private async void CoopAndreasNET_onInit()
        {
            await Task.Run(() =>
            {
                while (Memory.ReadUInt32(0xC8D4C0) != 9)
                {
                    Thread.Sleep(10);
                }
                Console.WriteLine("Inited");
                Patch.PatchPlayerPed();
                connection = new Connection();
                connection.Connect(File.ReadAllText("coopandreasip.txt"));
                Pad.InitPads(PlayerPed.Player);
                
                Thread.Sleep(2500);
                while (Connection.client != null)
                {
                    Thread.Sleep(62);
                    Connection.client?.Update();
                    var data = OnFootSyncData.Collect();
                    OnFoot.Send(data);
                }
            });
        }

        private void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            Exception ex = (Exception)e.ExceptionObject;
            Logger.Log(ex.ToString());
            Thread.Sleep(500); 

        }


        private void SAPlugin_GameTicking()
        {
            //Memory.WriteVector2D(0xBAA248, new CVector2D(Ped.Player.Position.X, Ped.Player.Position.Y)); // radar moving
            for (int i = 0; i < 4; i++)
            {
                if (RemotePlayer.All[i] != null)
                {
                    //short[] keys = RemotePlayer.All[i].OnFootSyncData.keys;
                    //ControllerState state = new ControllerState()
                    //{
                    //    LeftStickX = keys[0],       // лево/право
                    //    LeftStickY = keys[1],       // прямо/назад
                    //    RightShoulder1 = keys[2],   // прицел/ручник
                    //    ButtonSquare = keys[3],     // прыжок/ехать назад
                    //    ButtonTriangle = keys[4],   // зайти/выйти в/из транспорт/а
                    //    ButtonCross = keys[5],      // спринт/плавание
                    //    ButtonCircle = keys[6],     // выстрел/удар
                    //    PedWalk = keys[7]           // ходьба
                    //};
                    //Console.WriteLine(string.Join(", ", keys));
                    //Console.WriteLine(state.ToString());
                    //Pad.SetPadState(RemotePlayer.All[i].ped, state);
                    RemotePlayer.All[i].ped.Velocity = RemotePlayer.All[i].OnFootSyncData.velocity;
                    if (RemotePlayer.All[i].ped.OnGround)
                    {
                        RemotePlayer.All[i].ped.ApplyMoveSpeed();
                    }
                }
            }
        }
    }
}
