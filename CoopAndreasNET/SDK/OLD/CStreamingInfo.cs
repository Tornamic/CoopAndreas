using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public enum StreamingLoadState 
    {
        NotLoaded,
        Loaded,
        Requested,
        Channeled,
        Finishing
    }
    public enum StreamingFlags
    {
        GameRequest = 0x1,
        MissionRequest = 0x2,
        PriorityRequest = 0x8,
        NotVisible = 0x10
    };

    
    public class CStreamingInfo
    {
        public int BaseAddress;

        public CStreamingInfo(IntPtr Address) => BaseAddress = Address.ToInt32();

        public CStreamingInfo Next => new CStreamingInfo((IntPtr)Memory.ReadInt32(BaseAddress + 0x0));
        public CStreamingInfo Previous => new CStreamingInfo((IntPtr)Memory.ReadInt32(BaseAddress + 0x4));

        public StreamingLoadState LoadState
        {
            get => (StreamingLoadState)(Memory.ReadByte(BaseAddress + 0x8));
            set => Memory.WriteByte(BaseAddress + 0x8, Convert.ToByte(value));
        }

        public StreamingFlags Flags
        {
            get => (StreamingFlags)(Memory.ReadByte(BaseAddress + 0x9));
            set => Memory.WriteByte(BaseAddress + 0x9, Convert.ToByte(value));
        }

        public short ModelIndex
        {
            get => Memory.ReadInt16(BaseAddress + 0xB);
            set => Memory.WriteInt16(BaseAddress + 0xB, value);
        }

        public int CDPosition
        {
            get => Memory.ReadInt32(BaseAddress + 0xD);
            set => Memory.WriteInt32(BaseAddress + 0xD, value);
        }

        public int CDSize
        {
            get => Memory.ReadInt32(BaseAddress + 0x11);
            set => Memory.WriteInt32(BaseAddress + 0x11, value);
        }
    }

}
