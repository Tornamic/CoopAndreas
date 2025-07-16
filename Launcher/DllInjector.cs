using Launcher.Enums;
using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace Launcher
{
    public sealed class DllInjector
    {
        static readonly IntPtr INTPTR_ZERO = (IntPtr)0;

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr OpenProcess(uint dwDesiredAccess, int bInheritHandle, uint dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern int CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, IntPtr dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern int WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] buffer, uint size, int lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttribute, IntPtr dwStackSize, IntPtr lpStartAddress,
            IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern UInt32 WaitForSingleObject(IntPtr hHandle, UInt32 dwMilliseconds);

        private const UInt32 WAIT_OBJECT_0 = 0x00000000;
        private const UInt32 WAIT_TIMEOUT = 0x00000102;


        public Process Process { get; private set; }

        public DllInjector(Process process)
        {
            Process = process;
        }

        public DllInjectionResult Inject(string sDllPath)
        {
            if (!File.Exists(sDllPath))
            {
                return DllInjectionResult.DllNotFound;
            }

            uint _procId = (uint)Process.Id;

            if (_procId == 0)
            {
                return DllInjectionResult.GameProcessNotFound;
            }

            if (!bInject(_procId, sDllPath))
            {
                return DllInjectionResult.InjectionFailed;
            }

            return DllInjectionResult.Success;
        }

        private bool bInject(uint pToBeInjected, string sDllPath)
        {
            // Убедимся, что путь абсолютный
            sDllPath = Path.GetFullPath(sDllPath);

            // Открываем процесс
            IntPtr hndProc = OpenProcess(0x2 | 0x8 | 0x10 | 0x20 | 0x400, 1, pToBeInjected);
            if (hndProc == IntPtr.Zero)
                return false;

            // Ждём, пока загрузится vorbisFile.dll
            Process targetProc = Process.GetProcessById((int)pToBeInjected);
            bool vorbisLoaded = false;
            while (!vorbisLoaded)
            {
                try
                {
                    targetProc.Refresh();
                    foreach (ProcessModule module in targetProc.Modules)
                    {
                        if (module.ModuleName.Equals("vorbisFile.dll", StringComparison.OrdinalIgnoreCase))
                        {
                            vorbisLoaded = true;
                            break;
                        }
                    }
                }
                catch
                {
                    // В случае ошибки доступа — лучше выйти
                    CloseHandle(hndProc);
                    return false;
                }

                if (!vorbisLoaded)
                    Thread.Sleep(100);
            }

            // Получаем адрес LoadLibraryA
            IntPtr lpLLAddress = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
            if (lpLLAddress == IntPtr.Zero)
            {
                CloseHandle(hndProc);
                return false;
            }

            // Добавляем null-терминатор, чтобы LoadLibraryA корректно сработал
            byte[] dllBytes = Encoding.ASCII.GetBytes(sDllPath + '\0');

            // Выделяем память под строку с путём
            IntPtr lpAddress = VirtualAllocEx(hndProc, IntPtr.Zero, (IntPtr)dllBytes.Length, 0x1000 | 0x2000, 0x40);
            if (lpAddress == IntPtr.Zero)
            {
                CloseHandle(hndProc);
                return false;
            }

            // Пишем строку в память
            if (WriteProcessMemory(hndProc, lpAddress, dllBytes, (uint)dllBytes.Length, 0) == 0)
            {
                CloseHandle(hndProc);
                return false;
            }

            // Создаём удалённый поток
            IntPtr hThread = CreateRemoteThread(hndProc, IntPtr.Zero, IntPtr.Zero, lpLLAddress, lpAddress, 0, IntPtr.Zero);
            if (hThread == IntPtr.Zero)
            {
                CloseHandle(hndProc);
                return false;
            }

            // Дожидаемся завершения потока (опционально, но полезно)
            WaitForSingleObject(hThread, 5000);

            // Закрываем хендлы
            CloseHandle(hThread);
            CloseHandle(hndProc);

            return true;
        }

    }
}
