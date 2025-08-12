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
            "CoopAndreasSA.dll"
        };

        public LaunchResult LaunchAndInject(string gamePath, string playerNickName, string serverIpAddress, ushort serverPort, string id, string serial, params string[] librariesToInject)
        {
            try
            {
                // Sometimes when you wish to inject and have dead gta process it will write inject error when in reality the issue is dead gta process.
                Process[] processes = Process.GetProcessesByName("gta_sa");
                for (int i = 0; i < processes.Length; i++)
                {
                    processes[i].Kill();
                }

                if (!File.Exists(gamePath))
                    return LaunchResult.GameNotFound;

                Process process = new Process();

                process.StartInfo = new ProcessStartInfo()
                {
                    FileName = gamePath,
                    Arguments = $"-name {playerNickName} -ip {serverIpAddress} -port {serverPort} -id {id} -serial {serial}",
                    UseShellExecute = true
                };

                if (!process.Start())
                    return LaunchResult.LaunchFailed;

                DllInjector dllInjector = new DllInjector(process);

                foreach (string library in librariesToInject)
                {
                    if (dllInjector.Inject(Path.Combine(Path.GetDirectoryName(gamePath), library)) != DllInjectionResult.Success)
                        return LaunchResult.InjectionFailed;
                }

                return LaunchResult.Success;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error during launch and inject: {ex.Message}");
                return LaunchResult.LaunchFailed;
            }
        }
    }
}
