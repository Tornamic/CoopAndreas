using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Launcher.Core.Enums;

namespace Launcher.Core
{
    public sealed class DllInjector
    {
        private static readonly IntPtr NULL_HANDLE = IntPtr.Zero;

        [DllImport("kernel32.dll", CharSet = CharSet.Ansi)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("kernel32.dll", CharSet = CharSet.Ansi)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

        private delegate IntPtr OpenProcessDelegate(uint dwDesiredAccess, bool bInheritHandle, uint dwProcessId);
        private delegate bool CloseHandleDelegate(IntPtr hObject);
        private delegate IntPtr VirtualAllocExDelegate(IntPtr hProcess, IntPtr lpAddress, IntPtr dwSize, uint flAllocationType, uint flProtect);
        private delegate bool WriteProcessMemoryDelegate(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int nSize, out int lpNumberOfBytesWritten);
        private delegate IntPtr CreateRemoteThreadDelegate(IntPtr hProcess, IntPtr lpThreadAttributes, IntPtr dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, out uint lpThreadId);
        private delegate uint WaitForSingleObjectDelegate(IntPtr hHandle, uint dwMilliseconds);

        // Access rights constants - obfuscated
        private const uint PROCESS_ACCESS_RIGHTS = 0x43A; // PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ

        private const uint MEM_COMMIT_RESERVE = 0x3000; // MEM_COMMIT | MEM_RESERVE
        private const uint PAGE_READWRITE = 0x04;
        private const uint WAIT_TIMEOUT_MS = 10000; // Increased timeout

        public Process Process { get; private set; }

        public DllInjector(Process process)
        {
            Process = process ?? throw new ArgumentNullException(nameof(process));
        }

        public async Task<DllInjectionResult> InjectAsync(string dllPath)
        {
            if (string.IsNullOrEmpty(dllPath) || !File.Exists(dllPath))
            {
                return DllInjectionResult.DllNotFound;
            }

            if (Process?.HasExited != false)
            {
                return DllInjectionResult.GameProcessNotFound;
            }

            try
            {
                return await Task.Run(() => PerformInjection(dllPath));
            }
            catch (Exception)
            {
                return DllInjectionResult.InjectionFailed;
            }
        }

        private DllInjectionResult PerformInjection(string dllPath)
        {
            // Get full path to avoid relative path issues
            string fullDllPath = Path.GetFullPath(dllPath);

            // Wait for the target process to be in a ready state
            if (!WaitForProcessReady())
            {
                return DllInjectionResult.GameProcessNotFound;
            }

            // Dynamically load kernel32 functions to avoid static imports
            var kernel32 = GetModuleHandle("kernel32.dll");
            if (kernel32 == NULL_HANDLE)
                return DllInjectionResult.InjectionFailed;

            // Get function addresses
            var openProcessAddr = GetProcAddress(kernel32, "OpenProcess");
            var closeHandleAddr = GetProcAddress(kernel32, "CloseHandle");
            var virtualAllocExAddr = GetProcAddress(kernel32, "VirtualAllocEx");
            var writeProcessMemoryAddr = GetProcAddress(kernel32, "WriteProcessMemory");
            var createRemoteThreadAddr = GetProcAddress(kernel32, "CreateRemoteThread");
            var waitForSingleObjectAddr = GetProcAddress(kernel32, "WaitForSingleObject");
            var loadLibraryAddr = GetProcAddress(kernel32, "LoadLibraryA");

            if (openProcessAddr == NULL_HANDLE || loadLibraryAddr == NULL_HANDLE)
                return DllInjectionResult.InjectionFailed;

            // Create delegates
            var openProcess = Marshal.GetDelegateForFunctionPointer<OpenProcessDelegate>(openProcessAddr);
            var closeHandle = Marshal.GetDelegateForFunctionPointer<CloseHandleDelegate>(closeHandleAddr);
            var virtualAllocEx = Marshal.GetDelegateForFunctionPointer<VirtualAllocExDelegate>(virtualAllocExAddr);
            var writeProcessMemory = Marshal.GetDelegateForFunctionPointer<WriteProcessMemoryDelegate>(writeProcessMemoryAddr);
            var createRemoteThread = Marshal.GetDelegateForFunctionPointer<CreateRemoteThreadDelegate>(createRemoteThreadAddr);
            var waitForSingleObject = Marshal.GetDelegateForFunctionPointer<WaitForSingleObjectDelegate>(waitForSingleObjectAddr);

            // Open target process
            IntPtr processHandle = openProcess(PROCESS_ACCESS_RIGHTS, false, (uint)Process.Id);
            if (processHandle == NULL_HANDLE)
                return DllInjectionResult.InjectionFailed;

            try
            {
                // Convert DLL path to bytes
                byte[] dllPathBytes = Encoding.UTF8.GetBytes(fullDllPath + '\0');

                // Allocate memory in target process
                IntPtr allocatedMemory = virtualAllocEx(processHandle, NULL_HANDLE,
                    (IntPtr)dllPathBytes.Length, MEM_COMMIT_RESERVE, PAGE_READWRITE);

                if (allocatedMemory == NULL_HANDLE)
                    return DllInjectionResult.InjectionFailed;

                // Write DLL path to allocated memory
                if (!writeProcessMemory(processHandle, allocatedMemory, dllPathBytes,
                    dllPathBytes.Length, out _))
                    return DllInjectionResult.InjectionFailed;

                // Create remote thread to load the DLL
                IntPtr remoteThread = createRemoteThread(processHandle, NULL_HANDLE, NULL_HANDLE,
                    loadLibraryAddr, allocatedMemory, 0, out _);

                if (remoteThread == NULL_HANDLE)
                    return DllInjectionResult.InjectionFailed;

                // Wait for thread completion
                uint waitResult = waitForSingleObject(remoteThread, WAIT_TIMEOUT_MS);

                closeHandle(remoteThread);

                return waitResult == 0 ? DllInjectionResult.Success : DllInjectionResult.InjectionFailed;
            }
            finally
            {
                closeHandle(processHandle);
            }
        }

        private bool WaitForProcessReady()
        {
            const int maxAttempts = 100;
            const int delayMs = 200;

            for (int attempt = 0; attempt < maxAttempts; attempt++)
            {
                try
                {
                    if (Process?.HasExited == true)
                        return false;

                    Process.Refresh();

                    // Check if vorbisFile.dll is loaded (GTA SA dependency)
                    foreach (ProcessModule module in Process.Modules)
                    {
                        if (string.Equals(module.ModuleName, "vorbisFile.dll",
                            StringComparison.OrdinalIgnoreCase))
                        {
                            return true;
                        }
                    }
                }
                catch (Exception)
                {
                    // Process might not be accessible yet
                }

                Thread.Sleep(delayMs);
            }

            return false;
        }
    }
}
