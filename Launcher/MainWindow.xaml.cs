using System;
using System.Data.Common;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using Launcher.Core;
using Launcher.Core.Enums;
using Localization = Launcher.Core.Localization;

namespace Launcher
{
    public partial class MainWindow : Window
    {
        public static MainWindow Instance { get; private set; }

        ConfigProvider config;
        Localization localization;


        public MainWindow()
        {
            InitializeComponent();
            Instance = this;
        }

        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {
            string configDirectory = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "CoopAndreas");
            string configFilePath = Path.Combine(configDirectory, "launcher.ini");

            config = new ConfigProvider(configFilePath);
            config.Load();

            localization = new Localization();
            localization.ChangeGlobalLanguage(config.Language);

            cbLanguage.SelectedIndex = Array.IndexOf(localization.List, config.Language);
            tbNickname.Text = config.NickName;
            tbIpPort.Text = config.IpPort;
            tbSerialKey.Text = config.SerialKey;
            killGtaProcessesCheckBox.IsChecked = config.KillProcessesBeforeStart;

            var computerId = new ComputerID();

            await Task.Run(() =>
            {
                string id = computerId.GetUniqueSystemId();
                Dispatcher.Invoke(() =>
                {
                    tbGen.Text = $"/gen {id}";
                });
            });
        }

        static bool IsDXInstalled()
        {
            return File.Exists(Path.Combine(Environment.SystemDirectory, "d3dx9_43.dll"));
        }

        /*
        private void languageCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            localization.ChangeGlobalLanguage(localization.List[languageCombo.SelectedIndex]);
            config.Language = localization.List[languageCombo.SelectedIndex];
            config.Save();
        }
        */

        private void CopyButtonGen_Click(object sender, RoutedEventArgs e)
        {
            Clipboard.SetText(tbGen.Text);
        }

        private void TbSerialKey_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            config.SerialKey = tbSerialKey.Text;
            config.Save();
        }

        private void TbIpPort_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            config.IpPort = tbIpPort.Text;
            config.Save();
        }

        private void TbNickname_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            config.NickName = tbNickname.Text;
            config.Save();
        }

        private void DiscordServer_Clicked(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start("https://discord.gg/Z3ugSgFJMU");
            }
            catch
            {
                MessageBox.Show("Failed to open Discord link. Please copy the link and open it in your browser: https://discord.gg/Z3ugSgFJMU");
            }
        }

        private async void ConnectToServer_Click(object sender, RoutedEventArgs e)
        {
            if (!Validator.IsValidNickName(tbNickname.Text))
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

            if (!Validator.ParseIpPort(tbIpPort.Text, ref ip, ref port))
            {
                MessageBox.Show("Bad ip:port");
                return;
            }

            Core.Launcher launcher = new Core.Launcher();

            LaunchResult result = await launcher.LaunchAndInjectAsync("gta_sa.exe", tbNickname.Text, ip, port, tbGen.Text.Replace("/gen", "").Trim(), tbSerialKey.Text, config.KillProcessesBeforeStart, launcher.LibrariesToInject);

            if (result != LaunchResult.Success)
                MessageBox.Show(result.ToString());
        }

        private void CbLanguage_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            localization.ChangeGlobalLanguage(localization.List[cbLanguage.SelectedIndex]);
            config.Language = localization.List[cbLanguage.SelectedIndex];
            config.Save();
        }

        private void KillGtaProcessesCheckBox_Clicked(object sender, RoutedEventArgs e)
        {
            config.KillProcessesBeforeStart = !config.KillProcessesBeforeStart;
            killGtaProcessesCheckBox.IsChecked = config.KillProcessesBeforeStart;
            config.Save();
        }
    }
}