using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using GTASDK;

namespace CoopAndreasNET.SDK
{
    public class CMatrix
    {
        public int BaseAddress;
        public CVector right
        {
            get => Memory.ReadVector(BaseAddress);
            set => Memory.WriteVector(BaseAddress, value);
        }

        public CVector at
        {
            get => Memory.ReadVector(BaseAddress + 0x10);
            set => Memory.WriteVector(BaseAddress + 0x10, value);
        }

        public CVector up
        {
            get => Memory.ReadVector(BaseAddress + 0x20);
            set => Memory.WriteVector(BaseAddress + 0x20, value);
        }




        public CVector Posn
        {
            get => Memory.ReadVector(BaseAddress + 0x30);
            set => Memory.WriteVector(BaseAddress + 0x30, value);
        }

        //public IntPtr m_pAttachMatrix;

        public bool DeleteOnDetach
        {
            get => Memory.Read4bBool(BaseAddress + 0x44);
            set => Memory.Write4bBool(BaseAddress + 0x44, value);
        }





    }

    [StructLayout(LayoutKind.Sequential)]
    public class _CMatrix
    {
        public CVector right;
        public uint flags;
        public CVector up;
        public uint _pad1;
        public CVector at;
        private uint _pad2;
        public CVector pos;
        private uint _pad3;
        public IntPtr RwMatrix_AttachMatrix;
        [MarshalAs(UnmanagedType.Bool)] public bool OwnsAttachedMatrix;
    }
}
