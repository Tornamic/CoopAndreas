using Launcher.Properties;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Launcher
{
    public class ConfigProvider
    {
        public ConfigProvider(string filePath) 
        {
            _filePath = filePath;
        }

        public string NickName { get; set; } = string.Empty;
        public string IpPort { get; set; } = string.Empty;
        public string SerialKey { get; set; } = string.Empty;
        public string Language { get; set; } = "en";
        public string GamePath { get; set; } = string.Empty;

        private string _filePath;

        public void Save()
        {
            string directory = Path.GetDirectoryName(_filePath);
            if (!Directory.Exists(directory))
            {
                Directory.CreateDirectory(directory);
            }

            using (StreamWriter writer = new StreamWriter(_filePath))
            {
                foreach (var prop in this.GetType().GetProperties())
                {
                    string value = prop.GetValue(this, null)?.ToString();
                    writer.WriteLine($"{prop.Name}={value}");
                }
            }
        }

        public void Load()
        {
            if (!File.Exists(_filePath))
            {
                Save();
                return;
            }

            foreach (var line in File.ReadLines(_filePath))
            {
                var parts = line.Split('=');
                if (parts.Length == 2)
                {
                    var prop = this.GetType().GetProperty(parts[0].Trim());
                    if (prop != null && prop.CanWrite)
                    {
                        prop.SetValue(this, Convert.ChangeType(parts[1].Trim(), prop.PropertyType), null);
                    }
                }
            }
        }
    }
}
