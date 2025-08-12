using System;
using System.Security.Cryptography;
using System.Text;

namespace Launcher.Core
{
    public class ComputerID
    {
        public ComputerID() { }

        public int Length { get; private set; } = 8;

        public string GetUniqueSystemId()
        {
            string cpuId = WMIProvider.Get("Win32_Processor", "ProcessorId");
            string diskSerial = WMIProvider.Get("Win32_DiskDrive", "SerialNumber");
            string combined = new StringBuilder()
                .Append(cpuId)
                .Append(diskSerial)
                .Append(Environment.MachineName)
                .Append(Environment.UserName)
                .ToString();

            using (MD5 md5 = MD5.Create())
            {
                byte[] hash = md5.ComputeHash(Encoding.UTF8.GetBytes(combined));
                return BitConverter.ToString(hash, 0, Length / 2).Replace("-", "");
            }
        }
    }
}
