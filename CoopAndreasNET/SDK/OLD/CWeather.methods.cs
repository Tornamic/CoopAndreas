using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public partial class CWeather
    {

        // Functions Done...

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void __AddRain();
        public static void AddRain()
        {
            Memory.CallFunction<__AddRain>(0x72A9A0)();
        }

        //[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        //public delegate void __AddSplashesDuringHurricane();
        //public static void AddSplashesDuringHurricane()
        //{
        //    Memory.CallFunction<__AddSplashesDuringHurricane>(0x57D160)();
        //}

        //[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        //public delegate void __AddStreamAfterRain();
        //public static void AddStreamAfterRain()
        //{
        //    Memory.CallFunction<__AddStreamAfterRain>(0x57D340)();
        //}

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void __ForceWeather(short w);
        public static void ForceWeather(Weather Weather)
        {
            Memory.CallFunction<__ForceWeather>(0x72A4E0)((short)Weather);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void __ForceWeatherNow(short w);
        public static void ForceWeatherNow(Weather Weather)
        {
            Memory.CallFunction<__ForceWeatherNow>(0x72A4F0)((short)Weather);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void __Init();
        public static void Init()
        {
            Memory.CallFunction<__Init>(0x72A480)();          
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void __ReleaseWeather();
        public static void ReleaseWeather()
        {
            Memory.CallFunction<__ReleaseWeather>(0x72A510)();
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void __RenderRainStreaks();
        public static void RenderRainStreaks()
        {
            Memory.CallFunction<__RenderRainStreaks>(0x72AF70)();
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void __Update();
        public static void Update()
        {
            Memory.CallFunction<__Update>(0x72B850)();
        }
    }
}
