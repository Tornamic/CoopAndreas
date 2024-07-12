using System.Collections.Generic;
using System.Windows.Forms;

namespace Launcher
{
    public class Localization
    {
        public Dictionary<string, Dictionary<Control, string>> Dictionary = new Dictionary<string, Dictionary<Control, string>>();
        public Localization()
        {
            var en = new Dictionary<Control, string>();
            var ru = new Dictionary<Control, string>();
            var pt = new Dictionary<Control, string>(); // for sr_milton
            var ua = new Dictionary<Control, string>();

            en.Add(Program.MainForm.b_connect, "Connect");
            en.Add(Program.MainForm.lb_connect, "Connect to a CoopAndreas server");
            en.Add(Program.MainForm.lb_nickname, "Nickname:");
            en.Add(Program.MainForm.lb_ipport, "IP:PORT:");
            en.Add(Program.MainForm.lb_port, "Port:");
            en.Add(Program.MainForm.lb_language, "Language:");
            en.Add(Program.MainForm.lb_maxplayers, "Max Players:");
            en.Add(Program.MainForm.lb_startserver, "Start a new CoopAndreas server");
            en.Add(Program.MainForm.b_startserver, "Start Server");
            en.Add(Program.MainForm.connectPage, "Connect");
            en.Add(Program.MainForm.serverPage, "Server");
            en.Add(Program.MainForm.configPage, "Config");

            ru.Add(Program.MainForm.b_connect, "Подключиться");
            ru.Add(Program.MainForm.lb_connect, "Подключиться к серверу CoopAndreas");
            ru.Add(Program.MainForm.lb_nickname, "Никнейм:");
            ru.Add(Program.MainForm.lb_ipport, "IP:Порт:");
            ru.Add(Program.MainForm.lb_port, "Порт:");
            ru.Add(Program.MainForm.lb_language, "Язык:");
            ru.Add(Program.MainForm.lb_maxplayers, "Макс. Игроков:");
            ru.Add(Program.MainForm.lb_startserver, "Запустить новый сервер CoopAndreas");
            ru.Add(Program.MainForm.b_startserver, "Запустить Сервер");
            ru.Add(Program.MainForm.connectPage, "Подключиться");
            ru.Add(Program.MainForm.serverPage, "Сервер");
            ru.Add(Program.MainForm.configPage, "Конфиг");

            pt.Add(Program.MainForm.b_connect, "Conectar");
            pt.Add(Program.MainForm.lb_connect, "Conectar a um servidor CoopAndreas"); 
            pt.Add(Program.MainForm.lb_nickname, "Nickname:");
            pt.Add(Program.MainForm.lb_ipport, "IP:PORTA:");
            pt.Add(Program.MainForm.lb_port, "Porta:");
            pt.Add(Program.MainForm.lb_language, "Linguagem:");
            pt.Add(Program.MainForm.lb_maxplayers, "Max Jogadores:");
            pt.Add(Program.MainForm.lb_startserver, "Iniciar um novo servidor CoopAndreas");
            pt.Add(Program.MainForm.b_startserver, "Iniciar Servidor");
            pt.Add(Program.MainForm.connectPage, "Conectar");
            pt.Add(Program.MainForm.serverPage, "Servidor");
            pt.Add(Program.MainForm.configPage, "Config");

            ua.Add(Program.MainForm.b_connect, "Підключитися");
            ua.Add(Program.MainForm.lb_connect, "Підключитися до сервера CoopAndreas");
            ua.Add(Program.MainForm.lb_nickname, "Нікнейм:");
            ua.Add(Program.MainForm.lb_ipport, "IP:Порт:");
            ua.Add(Program.MainForm.lb_port, "Порт:");
            ua.Add(Program.MainForm.lb_language, "Мова:");
            ua.Add(Program.MainForm.lb_maxplayers, "Макс. Гравців:");
            ua.Add(Program.MainForm.lb_startserver, "Запустити новий сервер CoopAndreas");
            ua.Add(Program.MainForm.b_startserver, "Запустити Сервер");
            ua.Add(Program.MainForm.connectPage, "Підключитися");
            ua.Add(Program.MainForm.serverPage, "Сервер");
            ua.Add(Program.MainForm.configPage, "Конфіг");

            Dictionary.Add("en", en);
            Dictionary.Add("ru", ru);
            Dictionary.Add("pt", pt);
            Dictionary.Add("ua", ua);
        }
        public void ChangeGlobalLanguage(string name)
        {
            if (!Dictionary.ContainsKey(name))
                return;

            foreach (var control in Dictionary[name])
            {
                control.Key.Text = control.Value;
            }
        }
    }
}
