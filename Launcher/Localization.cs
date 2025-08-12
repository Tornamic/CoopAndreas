using System.Collections.Generic;
using System.Windows.Forms;

namespace Launcher
{
    public class Localization
    {
        public Dictionary<string, Dictionary<Control, string>> Dictionary = new Dictionary<string, Dictionary<Control, string>>();
        public readonly string[] List = { "en", "ru", "pt", "ua" };
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
            en.Add(Program.MainForm.lb_startserver, "Start a new CoopAndreas server\n(not implemented, start manually)");
            en.Add(Program.MainForm.b_startserver, "Start Server");
            en.Add(Program.MainForm.connectPage, "Connect");
            en.Add(Program.MainForm.serverPage, "Server");
            en.Add(Program.MainForm.configPage, "Config");
            en.Add(Program.MainForm.lb_important,
@"IMPORTANT: You are playing the alpha/beta version 
of CoopAndreas, this version needs a beta test serial key.
Don't worry, the mod as well as the serial key are completely
free. Go to our Discord server, and type in the command below 
into any channel");
            en.Add(Program.MainForm.lb_serialkey, "Serial key:");
            en.Add(Program.MainForm.b_copy, "Copy");
            en.Add(Program.MainForm.link_discord, "Discord Server");
            en.Add(Program.MainForm.lb_gtapath, "GTA SA Path:");

            ru.Add(Program.MainForm.b_connect, "Подключиться");
            ru.Add(Program.MainForm.lb_connect, "Подключиться к серверу CoopAndreas");
            ru.Add(Program.MainForm.lb_nickname, "Никнейм:");
            ru.Add(Program.MainForm.lb_ipport, "IP:Порт:");
            ru.Add(Program.MainForm.lb_port, "Порт:");
            ru.Add(Program.MainForm.lb_language, "Язык:");
            ru.Add(Program.MainForm.lb_maxplayers, "Макс. Игроков:");
            ru.Add(Program.MainForm.lb_startserver, "Запустить новый сервер CoopAndreas\n(не реализовано, запускай вручную)");
            ru.Add(Program.MainForm.b_startserver, "Запустить Сервер");
            ru.Add(Program.MainForm.connectPage, "Подключиться");
            ru.Add(Program.MainForm.serverPage, "Сервер");
            ru.Add(Program.MainForm.configPage, "Конфиг");
            ru.Add(Program.MainForm.lb_important,
@"ВАЖНО: Ты играешь в alpha/beta версию CoopAndreas, 
эта версия нуждается в серийном ключе бета-теста. 
Не беспокойтесь, мод, как и серийный ключ полностью 
бесплатны. Зайдите на наш дискорд сервер, и введите в 
любой канал команду ниже");
            ru.Add(Program.MainForm.lb_serialkey, "Серийный ключ:");
            ru.Add(Program.MainForm.b_copy, "Копировать");
            ru.Add(Program.MainForm.link_discord, "Discord Сервер");
            ru.Add(Program.MainForm.lb_gtapath, "Путь к GTA SA:");

            pt.Add(Program.MainForm.b_connect, "Conectar");
            pt.Add(Program.MainForm.lb_connect, "Conectar a um servidor CoopAndreas"); 
            pt.Add(Program.MainForm.lb_nickname, "Nickname:");
            pt.Add(Program.MainForm.lb_ipport, "IP:PORTA:");
            pt.Add(Program.MainForm.lb_port, "Porta:");
            pt.Add(Program.MainForm.lb_language, "Linguagem:");
            pt.Add(Program.MainForm.lb_maxplayers, "Max Jogadores:");
            pt.Add(Program.MainForm.lb_startserver, "Iniciar um novo servidor CoopAndreas\n(não implementado, iniciar manualmente)");
            pt.Add(Program.MainForm.b_startserver, "Iniciar Servidor");
            pt.Add(Program.MainForm.connectPage, "Conectar");
            pt.Add(Program.MainForm.serverPage, "Servidor");
            pt.Add(Program.MainForm.configPage, "Config");
            pt.Add(Program.MainForm.lb_important,
@"IMPORTANTE: Você está jogando a versão alpha/beta do 
CoopAndreas. Esta versão requer uma chave de série para 
o beta teste. Não se preocupe, tanto o mod quanto a chave 
de série são completamente gratuitos. Entre no nosso 
servidor do Discord e digite o comando abaixo em
qualquer canalе");
            pt.Add(Program.MainForm.lb_serialkey, "Chave serial:");
            pt.Add(Program.MainForm.b_copy, "Cópia");
            pt.Add(Program.MainForm.link_discord, "Discord Servidor");
            pt.Add(Program.MainForm.lb_gtapath, "Caminho do GTA SA:");

            ua.Add(Program.MainForm.b_connect, "Підключитися");
            ua.Add(Program.MainForm.lb_connect, "Підключитися до сервера CoopAndreas");
            ua.Add(Program.MainForm.lb_nickname, "Нікнейм:");
            ua.Add(Program.MainForm.lb_ipport, "IP:Порт:");
            ua.Add(Program.MainForm.lb_port, "Порт:");
            ua.Add(Program.MainForm.lb_language, "Мова:");
            ua.Add(Program.MainForm.lb_maxplayers, "Макс. Гравців:");
            ua.Add(Program.MainForm.lb_startserver, "Запустити новий сервер CoopAndreas\n(не реалізовано, запускай вручну)");
            ua.Add(Program.MainForm.b_startserver, "Запустити Сервер");
            ua.Add(Program.MainForm.connectPage, "Підключитися");
            ua.Add(Program.MainForm.serverPage, "Сервер");
            ua.Add(Program.MainForm.configPage, "Конфіг");
            ua.Add(Program.MainForm.lb_important,
@"ВАЖЛИВО: Ти граєш в alpha/beta версію CoopAndreas,
ця версія потребує серійного ключа бета-тесту.
Не турбуйтесь, мод, як і серійний ключ повністю
безкоштовні. Зайдіть на наш дискорд сервер і введіть в
будь-який канал команду нижче");
            ua.Add(Program.MainForm.lb_serialkey, "Серійний ключ");
            ua.Add(Program.MainForm.b_copy, "Копіювати");
            ua.Add(Program.MainForm.link_discord, "Discord Сервер");
            ua.Add(Program.MainForm.lb_gtapath, "Шлях до GTA SA:");

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
