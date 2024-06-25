using System;
using System.IO;

namespace CoopAndreasNET
{
    public static class Logger
    {
        public static void Log(string input)
        {
            string message = $"\n[{DateTime.Now}] [LOG] {input}";
            File.AppendAllText("coopandreas.log", message);
            Console.WriteLine(message);
        }
        public static void Warning(string input)
        {
            string message = $"\n[{DateTime.Now}] [WARNING] {input}";
            File.AppendAllText("coopandreas.log", message);
            Console.WriteLine(message);
        }
        public static void Error(string input)
        {
            string message = $"\n[{DateTime.Now}] [ERROR] {input}";
            File.AppendAllText("coopandreas.log", message);
            Console.WriteLine(message);
        }
    }
}
