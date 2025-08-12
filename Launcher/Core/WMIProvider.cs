using System.Management;

namespace Launcher
{
    public class WMIProvider
    {
        public static string Get(string component, string key)
        {
            try
            {
                ManagementObjectSearcher searcher = new ManagementObjectSearcher($"SELECT {key} FROM {component}");
                foreach (ManagementObject obj in searcher.Get())
                {
                    return obj[key]?.ToString()?.Trim() ?? "Unknown";
                }
            }
            catch
            {
                // return "Unknown" in case of any failure
            }

            return "Unknown";
        }
    }
}
