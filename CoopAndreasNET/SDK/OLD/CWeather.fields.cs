using GTASDK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CoopAndreasNET.SDK
{
    public enum Weather : short
    {
        EXTRASUNNY_LA = 0,
        SUNNY_LA = 1,
        EXTRASUNNY_SMOG_LA = 2,
        SUNNY_SMOG_LA = 3,
        CLOUDY_LA = 4,
        SUNNY_SF = 5,
        EXTRASUNNY_SF = 6,
        CLOUDY_SF = 7,
        RAINY_SF = 8,
        FOGGY_SF = 9,
        SUNNY_VEGAS = 10,
        EXTRASUNNY_VEGAS = 11,
        CLOUDY_VEGAS = 12,
        EXTRASUNNY_COUNTRYSIDE = 13,
        SUNNY_COUNTRYSIDE = 14,
        CLOUDY_COUNTRYSIDE = 15,
        RAINY_COUNTRYSIDE = 16,
        EXTRASUNNY_DESERT = 17,
        SUNNY_DESERT = 18,
        SANDSTORM_DESERT = 19,
        UNDERWATER = 20,
        EXTRACOLOURS_1 = 21,
        EXTRACOLOURS_2 = 22
    }


    public partial class CWeather
    {
        public static Weather OldWeather
        {
            get => (Weather)Memory.ReadInt16(0xC81320);
            set => Memory.WriteInt16(0xC81320, (short)value);
        }
        public static Weather NewWeather
        {
            get => (Weather)Memory.ReadInt16(0xC8131C);
            set => Memory.WriteInt16(0xC8131C, (short)value);
        }
        public static Weather ForcedWeather
        {
            get => (Weather)Memory.ReadInt16(0xC81318);
            set => Memory.WriteInt16(0xC81318, (short)value);
        }
        public static bool LightningFlash
        {
            get => Memory.Read4bBool(0xC812CC);
            set => Memory.Write4bBool(0xC812CC, value);
        }
        public static bool LightningBurst
        {
            get => Memory.Read4bBool(0xC812CD);
            set => Memory.Write4bBool(0xC812CD, value);
        }

        /* To be done:
            int &CWeather::LightningStart = *(int*)0x9B5F84;
            int &CWeather::SoundHandle = *(int*)0x699EDC;
            int &CWeather::StreamAfterRainTimer = *(int*)0x699EE0; 
        */
    }
}
