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

namespace Launcher
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            FormBorderStyle = FormBorderStyle.FixedSingle;
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            languageCombo.SelectedIndex = 0;
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
            
            LaunchResult result = launcher.LaunchAndInject("gta_sa.exe", nicknameInput.Text, ip, port, false, 0, launcher.LibrariesToInject);

            if(result != LaunchResult.Success)
                MessageBox.Show(result.ToString());
        }
    }
}