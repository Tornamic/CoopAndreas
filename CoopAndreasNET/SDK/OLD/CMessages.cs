using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public static class CMessages
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _AddBigMessage([MarshalAs(UnmanagedType.LPWStr)]string s, int t, short st);
        public static void AddBigMessage(string Message, int Time, short Style)
        {
            Memory.CallFunction<_AddBigMessage>(0x584050)(Message, Time, Style);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _AddBigMessageQ([MarshalAs(UnmanagedType.LPWStr)]string s, int t, short st);
        public static void AddBigMessageQ(string Message, int Time, short Style)
        {
            Memory.CallFunction<_AddBigMessage>(0x583F40)(Message, Time, Style);
        }

        //void CMessages::AddBigMessageWithNumber(ushort *pString, uint time, ushort style, int number, int number2, int number3, int number4, int number5, int number6)	0x583350	


        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _AddMessage([MarshalAs(UnmanagedType.LPWStr)]string s, int t, short f);
        public static void AddMessage(string Message, int Time, short Flag)
        {
            Memory.CallFunction<_AddMessage>(0x584410)(Message, Time, Flag);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _AddMessageJumpQ([MarshalAs(UnmanagedType.LPWStr)]string s, int t, short f);
        public static void AddMessageJumpQ(string Message, int Time, short Flag)
        {
            Memory.CallFunction<_AddMessageJumpQ>(0x584300)(Message, Time, Flag);
        }

        //void CMessages::AddMessageJumpQWithNumber(ushort *pString, uint time, ushort flag, int number, int number2, int number3, int number4, int number5, int number6)	0x583440	

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _AddMessageJumpQWithString([MarshalAs(UnmanagedType.LPWStr)]string s, int t, short f, [MarshalAs(UnmanagedType.LPWStr)]string s2);
        public static void AddMessageJumpQWithString(string Message1, int Time, short Flag, string Message2)
        {
            Memory.CallFunction<_AddMessageJumpQWithString>(0x583220)(Message1, Time, Flag, Message2);
        }

        public static void ClearAllMessagesDisplayedByGame()
        {
            Memory.CallFunction<Memory.VoidDelegate>(0x582C70)();
        }

        public static void ClearMessages()
        {
            Memory.CallFunction<Memory.VoidDelegate>(0x5841E0)();
        }

        public static void ClearSmallMessagesOnly()
        {
            Memory.CallFunction<Memory.VoidDelegate>(0x584130)();
        }

        public static void Display()
        {
            Memory.CallFunction<Memory.VoidDelegate>(0x584550)();
        }

        public static void Init()
        {
            Memory.CallFunction<Memory.VoidDelegate>(0x5849C0)();
        }

        public static void Process()
        {
            Memory.CallFunction<Memory.VoidDelegate>(0x584650)();
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _InsertStringInString([MarshalAs(UnmanagedType.LPWStr)]string s1, [MarshalAs(UnmanagedType.LPWStr)]string s2);
        public static void InsertStringInString(string String1, string String2)
        {
            Memory.CallFunction<_InsertStringInString>(0x583AF0)(String1, String2);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _WideStringCompare([MarshalAs(UnmanagedType.LPWStr)]string s1, [MarshalAs(UnmanagedType.LPWStr)]string s2, short len);
        public static void WideStringCompare(string String1, string String2, short lengthToCompare)
        {
            Memory.CallFunction<_WideStringCompare>(0x583AF0)(String1, String2, lengthToCompare);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void _InsertPlayerControlKeysInString([MarshalAs(UnmanagedType.LPWStr)]string s);
        public static void InsertPlayerControlerKeysInString(string String)
        {
            Memory.CallFunction<_InsertPlayerControlKeysInString>(0x5836B0)(String);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate short _GetWideStringLength([MarshalAs(UnmanagedType.LPWStr)] string String);
        public static short GetWideStringLength(string String)
        {
            return Memory.CallFunction<_GetWideStringLength>(0x5849A0)(String);
        }
    }
}
