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

        private async void MainForm_Load(object sender, EventArgs e)
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

            await Task.Run(() =>
            {
                string id = computerId.GetUniqueSystemId();
                tb_command.Text = $"/gen {id}";
            });
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            if (!Validator.IsValidNickName(nicknameInput.Text))
            {
                MessageBox.Show("Bad nickname");
                return;
            }

            string ip = "";
            ushort port = 0;

            if(!Validator.ParseIpPort(ipportInput.Text, ref ip, ref port))
            {
                MessageBox.Show("Bad ip:port");
                return;
            }

            Launcher launcher = new Launcher();
            
            LaunchResult result = launcher.LaunchAndInject("gta_sa.exe", nicknameInput.Text, ip, port, tb_serialKey.Text, launcher.LibrariesToInject);

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
    }
}