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
using Launcher.Enums;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using System.Threading.Tasks;

namespace Launcher
{
    public partial class MainForm : Form
    {
        ConfigProvider config;
        Localization localization;
        public MainForm()
        {
            InitializeComponent();
            FormBorderStyle = FormBorderStyle.FixedSingle;
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            string configDirectory = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "CoopAndreas");
            string configFilePath = Path.Combine(configDirectory, "launcher.ini");

            config = new ConfigProvider(configFilePath);
            config.Load();

            localization = new Localization();
            localization.ChangeGlobalLanguage(config.Language);

            languageCombo.SelectedIndex = Array.IndexOf(localization.List, config.Language);
            nicknameInput.Text = config.NickName;
            ipportInput.Text = config.IpPort;
            tb_serialKey.Text = config.SerialKey;

            var computerId = new ComputerID();

            Task.Factory.StartNew(() =>
            {
                string id = computerId.GetUniqueSystemId();
                this.Invoke((MethodInvoker)delegate
                {
                    tb_command.Text = $"/gen {id}";
                });
            });

        }

        static bool IsDXInstalled()
        {
            return File.Exists(Path.Combine(Environment.SystemDirectory, "d3dx9_43.dll"));
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            if (!Validator.IsValidNickName(nicknameInput.Text))
            {
                MessageBox.Show("Bad nickname");
                return;
            }

            if (!IsDXInstalled())
            {
                MessageBox.Show("DirectX is not installed. Please install DirectX to run this application.");
                Process.Start("https://www.microsoft.com/en-us/download/details.aspx?id=35");
            }

            string ip = "";
            ushort port = 0;

            if(!Validator.ParseIpPort(ipportInput.Text, ref ip, ref port))
            {
                MessageBox.Show("Bad ip:port");
                return;
            }

            Launcher launcher = new Launcher();
            
            LaunchResult result = launcher.LaunchAndInject("gta_sa.exe", nicknameInput.Text, ip, port, tb_command.Text.Replace("/gen", "").Trim(), tb_serialKey.Text, launcher.LibrariesToInject);

            if(result != LaunchResult.Success)
                MessageBox.Show(result.ToString());
        }

        private void languageCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            localization.ChangeGlobalLanguage(localization.List[languageCombo.SelectedIndex]);
            config.Language = localization.List[languageCombo.SelectedIndex];
            config.Save();
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("https://discord.gg/Z3ugSgFJMU");
        }

        private void nicknameInput_TextChanged(object sender, EventArgs e)
        {
            config.NickName = nicknameInput.Text;
            config.Save();
        }

        private void ipportInput_TextChanged(object sender, EventArgs e)
        {
            config.IpPort = ipportInput.Text;
            config.Save();
        }

        private void tb_serialKey_TextChanged(object sender, EventArgs e)
        {
            config.SerialKey = tb_serialKey.Text;
            config.Save();
        }

        private void b_copy_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(tb_command.Text);
        }
    }
}