using GTASDK;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public class Scripting
    {
        public short OpCode;
        public Command this[ScriptCommands opcode]
        {
            get { return new Command((short)opcode); }
        }

        public class Command
        {
            public short OpCode;
            public Command(short opcode)
            {
                OpCode = opcode;
            }

            public void Call(params object[] args)
            {
                byte[] buffer = new byte[256];
                BinaryWriter buffWriter = new BinaryWriter(new MemoryStream(buffer));
                buffWriter.Write(OpCode);
                foreach (var arg in args)
                {
                    if (arg.GetType() == typeof(int))
                    {
                        buffWriter.Write((byte)0x1);
                        buffWriter.Write((int)arg);
                    }
                    if (arg.GetType() == typeof(short))
                    {
                        buffWriter.Write((byte)0x5);
                        buffWriter.Write((short)arg);
                    }
                    if (arg.GetType() == typeof(float))
                    {
                        buffWriter.Write((byte)0x6);
                        buffWriter.Write((float)arg);
                    }
                    if (arg.GetType() == typeof(bool) || arg.GetType() == typeof(byte))
                    {
                        buffWriter.Write((byte)0x4);
                        buffWriter.Write((byte)arg);
                    }
                }
                IntPtr scriptptr = Memory.Allocate(0xE0);

                IntPtr scbufptr = Memory.Allocate((uint)buffer.Length);
                Marshal.Copy(buffer, 0, scbufptr, buffer.Length);

                CRunningScript script = new CRunningScript(scriptptr);

                script.Name = "gtasdk";
                script.IsMission = false;
                script.UseMissionCleanup = false;
                script.NotFlag = Convert.ToBoolean((0x1B6 >> 15) & 1);
                script.IP = scbufptr.ToInt32();
                script.ProcessOneCommand();

            }

        }
    }

    public enum ScriptCommands : short
    {
        
    }

}