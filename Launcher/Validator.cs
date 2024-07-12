using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Launcher
{
    public static class Validator
    {
        public static bool IsValidNickName(string value)
        {
            return Regex.IsMatch(value, @"[a-zA-Z0-9_\[\]]{3,24}");
        }

        public static bool ParseIpPort(string value, ref string ip, ref ushort port)
        {
            Regex regex = new Regex(@"\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?):\d{1,5}\b");
            Match match = regex.Match(value);

            if (match.Success)
            {
                string[] parts = match.Value.Split(':');
                ip = parts[0];
                port = ushort.Parse(parts[1]);
            }

            return match.Success;
        }
    }
}
