using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Launcher.Core.Enums;

namespace Launcher.Core
{
    public class Launcher
    {
        public string[] LibrariesToInject =
        {
            "CoopAndreasSA.dll"
        };

        public async Task<LaunchResult> LaunchAndInjectAsync(string gamePath, string playerNickName, string serverIpAddress, ushort serverPort, string id, string serial, bool killProcesses, params string[] librariesToInject)
        {
            try
            {
                // Sometimes when you wish to inject and have dead gta process it will write inject error when in reality the issue is dead gta process.
                if (killProcesses)
                {
                    Process[] processes = Process.GetProcessesByName("gta_sa");
                    foreach (Process proc in processes)
                    {
                        try
                        {
                            proc.Kill();
                            proc.WaitForExit(5000);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine($"Warning: Could not kill process {proc.Id}: {ex.Message}");
                        }
                        finally
                        {
                            proc?.Dispose();
                        }
                    }

                    await Task.Delay(500);
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
                    string libraryPath = Path.Combine(Path.GetDirectoryName(gamePath), library);
                    DllInjectionResult result = await dllInjector.InjectAsync(libraryPath);

                    if (result != DllInjectionResult.Success)
                    {
                        Console.WriteLine($"Failed to inject {library}: {result}");
                        return LaunchResult.InjectionFailed;
                    }
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
