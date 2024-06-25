using CoopAndreasNET.SDK;
using GTASDK;
using Riptide;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Message = Riptide.Message;

namespace CoopAndreasNET.Sync
{
    public class OnFoot
    {

        public static void Send(OnFootSyncData onFootSyncData)
        {
            Message message = Message.Create(MessageSendMode.Unreliable, Packets.SyncOnFoot);

            // вектор позиции
            message.AddFloat(onFootSyncData.position.X);
            message.AddFloat(onFootSyncData.position.Y);
            message.AddFloat(onFootSyncData.position.Z);

            // вектор скорости
            message.AddFloat(onFootSyncData.velocity.X);
            message.AddFloat(onFootSyncData.velocity.Y);
            message.AddFloat(onFootSyncData.velocity.Z);

            // угол поворота
            message.AddFloat(onFootSyncData.heading);

            // состояние движения
            message.AddByte(onFootSyncData.moveState);

            // клавиши игрока
            message.AddShorts(onFootSyncData.keys);

            // *
            message.AddFloat(onFootSyncData.moveBlendRatio);

            Connection.Send(message);
        }

        [MessageHandler((ushort)Packets.SyncOnFoot)]
        private static void OnFootHandler(Message incomingPacket)
        {
            // чтение айди игрока из пакета
            ushort id = (ushort)(incomingPacket.GetUShort() - 1);

            // если игрока не сущействует
            if (RemotePlayer.All[id] == null)
            {
                // спавним его
                new RemotePlayer(id);
                Console.WriteLine($"Created player id {id}");
            }

            // чтение и запись позиции игрока в вектор
            CVector position = new CVector
                (
                    incomingPacket.GetFloat(),
                    incomingPacket.GetFloat(),
                    incomingPacket.GetFloat()
                );

            // чтение и запись скорости игрока в вектор
            CVector velocity = new CVector
                (
                    incomingPacket.GetFloat(),
                    incomingPacket.GetFloat(),
                    incomingPacket.GetFloat()
                );

            // чтение поворота игрока из пакета
            float heading = incomingPacket.GetFloat();

            // чтение состояния движения из пакета 
            byte moveState = incomingPacket.GetByte();

            // чтение клавиш из пакета
            short[] keys = incomingPacket.GetShorts();

            // чтение * из пакета
            float moveBlendRatio = incomingPacket.GetFloat();

            // применение позиции
            RemotePlayer.All[id].ped.Position = position;
 
            // применение скорости
            RemotePlayer.All[id].ped.Velocity = velocity;

            // применение угла поворота
            RemotePlayer.All[id].ped.Heading = heading;

            // запись клавишей в ControllerState для дальнейшего применения 
            ControllerState state = new ControllerState()
            {
                LeftStickX = keys[0],       // лево/право
                LeftStickY = keys[1],       // прямо/назад
                RightShoulder1 = keys[2],   // прицел/ручник
                ButtonSquare = keys[3],     // прыжок/ехать назад
                ButtonTriangle = keys[4],   // зайти/выйти в/из транспорт/а
                ButtonCross = keys[5],      // спринт/плавание
                ButtonCircle = keys[6],     // выстрел/удар
                PedWalk = keys[7]           // ходьба
            };

            // применение нажатых клавиш
            for (int i = 0; i < keys.Length; i++)
            {
                if (RemotePlayer.All[id].OnFootSyncData.keys[i] != keys[i])
                {
                    Pad.SetPadState(RemotePlayer.All[id].ped, state);
                    RemotePlayer.All[id].ped.ProcessControl();
                    break;
                }

            }

            // применение состояния движения 
            //RemotePlayer.All[id].ped.MoveState = moveState;
            //RemotePlayer.All[id].ped.SetMoveAnim();
            //RemotePlayer.All[id].ped.SetRealMoveAnim();

            // применение *
            //RemotePlayer.All[id].ped.MoveBlendRatio = moveBlendRatio;

            // запись пришедшей структуры как последнюю пришедшую
            RemotePlayer.All[id].OnFootSyncData = new OnFootSyncData()
            {
                position = position,
                velocity = velocity,
                heading = heading,
                moveState = moveState,
                keys = keys,
                moveBlendRatio = moveBlendRatio
            };
        }
    }
    public struct OnFootSyncData
    {
        public CVector position; // позиция игрока
        public CVector velocity; // скорость игрока
        public float heading; // угол поворота игрока
        public byte moveState; // состояние движения
        public short[] keys; // клавиши
        public float moveBlendRatio; // *
        public static OnFootSyncData Collect() // функция для чтения данных для отправки
        {
            OnFootSyncData onFootSyncData = new OnFootSyncData();

            PlayerPed player = PlayerPed.Player;

            onFootSyncData.position = player.Position;
            onFootSyncData.velocity = player.Velocity;
            onFootSyncData.heading = player.Heading;
            onFootSyncData.moveState = player.MoveState;
            ControllerState state = Pad.GetPadState(player);
            short[] keys = new short[]
            {
                state.LeftStickX,      // лево/право
                state.LeftStickY,      // прямо/назад
                state.RightShoulder1,  // прицел/ручник
                state.ButtonSquare,    // прыжок/ехать назад
                state.ButtonTriangle,  // зайти/выйти в/из транспорт/а
                state.ButtonCross,     // спринт/плавание
                state.ButtonCircle,    // выстрел/удар
                state.PedWalk          // ходьба
            };
            //onFootSyncData.moveBlendRatio = player.MoveBlendRatio;

            onFootSyncData.keys = keys;
            return onFootSyncData;
        }
    }
}
