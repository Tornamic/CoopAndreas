using Launcher.Enums;
using System;
using System.Diagnostics;
using System.IO;

namespace Launcher
{
    public class Launcher
    {
        public string[] LibrariesToInject =
        {
            //"enet.dll",
            "CoopAndreasSA.dll"
        };

        public LaunchResult LaunchAndInject(string gamePath, string playerNickName, string serverIpAddress, ushort serverPort, params string[] librariesToInject)
        {
            if(!File.Exists(gamePath)) 
                return LaunchResult.GameNotFound;

            Process process = new Process();

            process.StartInfo = new ProcessStartInfo()
            {
                FileName = gamePath,
                Arguments = $"-name {playerNickName} -ip {serverIpAddress} -port {serverPort}",
                UseShellExecute = true
            };

            if(!process.Start())
                return LaunchResult.LaunchFailed;

            DllInjector dllInjector = new DllInjector(process);

            foreach (string library in librariesToInject)
            {
                if(dllInjector.Inject(AppDomain.CurrentDomain.BaseDirectory + library) != DllInjectionResult.Success)
                    return LaunchResult.InjectionFailed;
            }

            return LaunchResult.Success;
        }
    }
}
