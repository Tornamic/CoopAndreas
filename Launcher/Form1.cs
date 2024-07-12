using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Launcher
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            FormBorderStyle = FormBorderStyle.FixedSingle;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            languageCombo.SelectedIndex = 0;
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            string processName = "gta_sa";

            //Both dlls need to be in Launcher folder
            string enetPath = AppDomain.CurrentDomain.BaseDirectory + "enet.dll";
            string coopandreasPath = AppDomain.CurrentDomain.BaseDirectory + "CoopAndreasSA.dll";

            DllInjector injector = DllInjector.GetInstance;

            DllInjectionResult result1 = injector.Inject(processName, enetPath);
            if (result1 != DllInjectionResult.Success)
            {
                MessageBox.Show("Fail injection first DLL: " + result1.ToString());
                return;
            }

            // Injetar segunda DLL
            DllInjectionResult result2 = injector.Inject(processName, coopandreasPath);
            if (result2 != DllInjectionResult.Success)
            {
                MessageBox.Show("Fail injecting second DLL: " + result2.ToString());
                return;
            }

            MessageBox.Show("Both DLL injected successfully!");
        }
    }

    public enum DllInjectionResult
    {
        DllNotFound,
        GameProcessNotFound,
        InjectionFailed,
        Success
    }

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

        static DllInjector _instance;

        public static DllInjector GetInstance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = new DllInjector();
                }
                return _instance;
            }
        }

        DllInjector() { }
        
        public DllInjectionResult Inject(string sProcName, string sDllPath)
        {
            if (!File.Exists(sDllPath))
            {
                return DllInjectionResult.DllNotFound;
            }

            uint _procId = 0;

            Process[] _procs = Process.GetProcesses();
            for (int i = 0; i < _procs.Length; i++)
            {
                if (_procs[i].ProcessName == sProcName)
                {
                    _procId = (uint)_procs[i].Id;
                    break;
                }
            }

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

        bool bInject(uint pToBeInjected, string sDllPath)
        {
            IntPtr hndProc = OpenProcess((0x2 | 0x8 | 0x10 | 0x20 | 0x400), 1, pToBeInjected);

            if (hndProc == INTPTR_ZERO)
            {
                return false;
            }

            IntPtr lpLLAddress = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

            if (lpLLAddress == INTPTR_ZERO)
            {
                return false;
            }

            IntPtr lpAddress = VirtualAllocEx(hndProc, (IntPtr)null, (IntPtr)sDllPath.Length, (0x1000 | 0x2000), 0X40);

            if (lpAddress == INTPTR_ZERO)
            {
                return false;
            }

            byte[] bytes = Encoding.ASCII.GetBytes(sDllPath);

            if (WriteProcessMemory(hndProc, lpAddress, bytes, (uint)bytes.Length, 0) == 0)
            {
                return false;
            }

            if (CreateRemoteThread(hndProc, (IntPtr)null, INTPTR_ZERO, lpLLAddress, lpAddress, 0, (IntPtr)null) == INTPTR_ZERO)
            {
                return false;
            }

            CloseHandle(hndProc);

            return true;
        }
    }

}