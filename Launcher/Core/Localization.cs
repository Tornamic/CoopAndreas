using System.Collections.Generic;
using System.Windows.Controls;
using Xceed.Wpf.Toolkit;

namespace Launcher.Core
{
    public class Localization
    {
        public Dictionary<string, Dictionary<Controls, string>> Dictionary = new Dictionary<string, Dictionary<Controls, string>>();
        public readonly string[] List = { "en", "ru", "pt", "ua" };

        public class Controls
        {
            public TextBlock lb;
            public WatermarkTextBox tb;
        }

        public Localization()
        {
            var en = new Dictionary<Controls, string>();
            var ru = new Dictionary<Controls, string>();
            var pt = new Dictionary<Controls, string>(); // for sr_milton
            var ua = new Dictionary<Controls, string>();

            en.Add(new Controls() { lb = MainWindow.Instance.lbConnect }, "Connect");
            en.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Connect to a CoopAndreas server");
            en.Add(new Controls() { tb = MainWindow.Instance.tbNickname }, "Enter your nickname...");
            en.Add(new Controls() { tb = MainWindow.Instance.tbIpPort }, "Enter IP:Port of server (example 127.0.0.1:6767)...");
            en.Add(new Controls() { lb = MainWindow.Instance.lbLanguage }, "Language:");
            en.Add(new Controls() { lb = MainWindow.Instance.lbKillProcess }, "Kill GTA-SA Processes before start:");

            // en.Add(Program.MainForm.lb_maxplayers, "Max Players:");
            // en.Add(new Controls() { Program.MainForm.lb_startserver, "Start a new CoopAndreas server\n(not implemented, start manually)");
            // en.Add(Program.MainForm.b_startserver, "Start Server");
            en.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Connect to Server");
            en.Add(new Controls() { lb = MainWindow.Instance.lbServer }, "Server");
            en.Add(new Controls() { lb = MainWindow.Instance.lbConfig }, "Config");
            en.Add(new Controls() { lb = MainWindow.Instance.lbImportant },
@"IMPORTANT: You are playing the alpha/beta version 
of CoopAndreas, this version needs a beta test serial key.
Don't worry, the mod as well as the serial key are completely
free. Go to our Discord server, and type in the command below 
into any channel");
            en.Add(new Controls() { tb = MainWindow.Instance.tbSerialKey }, "Enter your serial key...");
            en.Add(new Controls() { lb = MainWindow.Instance.lbCopyGen }, "Copy");
            en.Add(new Controls() { lb = MainWindow.Instance.lbDiscordServer }, "Discord Server");
            // Russian (ru)
            ru.Add(new Controls() { lb = MainWindow.Instance.lbConnect }, "Подключиться");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Подключиться к серверу CoopAndreas");
            ru.Add(new Controls() { tb = MainWindow.Instance.tbNickname }, "Введите ваш никнейм...");
            ru.Add(new Controls() { tb = MainWindow.Instance.tbIpPort }, "Введите IP:Порт сервера (пример 127.0.0.1:6767)...");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbLanguage }, "Язык:");
            // ru.Add(Program.MainForm.lb_maxplayers, "Макс. игроков:");
            // ru.Add(new Controls() { Program.MainForm.lb_startserver, "Запустить новый сервер CoopAndreas\n(не реализовано, запускайте вручную)");
            // ru.Add(Program.MainForm.b_startserver, "Запустить сервер");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Подключиться к серверу");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbServer }, "Сервер");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbConfig }, "Конфигурация");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbImportant },
            @"ВАЖНО: Вы играете в альфа/бета версию
CoopAndreas, эта версия требует бета-ключ.
Не волнуйтесь, мод и ключ полностью
бесплатные. Зайдите на наш Discord сервер
и введите команду ниже в любой канал");
            ru.Add(new Controls() { tb = MainWindow.Instance.tbSerialKey }, "Введите ваш серийный ключ...");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbCopyGen }, "Копировать");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbDiscordServer }, "Discord Сервер");
            ru.Add(new Controls() { lb = MainWindow.Instance.lbKillProcess }, "Убить процессы GTA-SA перед запуском:");

            // Ukrainian (ua)
            ua.Add(new Controls() { lb = MainWindow.Instance.lbConnect }, "Підключитися");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Підключитися до сервера CoopAndreas");
            ua.Add(new Controls() { tb = MainWindow.Instance.tbNickname }, "Введіть ваш нікнейм...");
            ua.Add(new Controls() { tb = MainWindow.Instance.tbIpPort }, "Введіть IP:Порт сервера (приклад 127.0.0.1:6767)...");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbLanguage }, "Мова:");
            // ua.Add(Program.MainForm.lb_maxplayers, "Макс. гравців:");
            // ua.Add(new Controls() { Program.MainForm.lb_startserver, "Запустити новий сервер CoopAndreas\n(не реалізовано, запускайте вручну)");
            // ua.Add(Program.MainForm.b_startserver, "Запустити сервер");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Підключитися до сервера");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbServer }, "Сервер");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbConfig }, "Конфігурація");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbImportant },
            @"ВАЖЛИВО: Ви граєте в альфа/бета версію
CoopAndreas, ця версія потребує бета-ключ.
Не хвилюйтеся, мод і ключ повністю
безкоштовні. Зайдіть на наш Discord сервер
і введіть команду нижче в будь-який канал");
            ua.Add(new Controls() { tb = MainWindow.Instance.tbSerialKey }, "Введіть ваш серійний ключ...");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbCopyGen }, "Копіювати");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbDiscordServer }, "Discord Сервер");
            ua.Add(new Controls() { lb = MainWindow.Instance.lbKillProcess }, "Вбити процеси GTA-SA перед запуском:");

            // Portuguese (pt)
            pt.Add(new Controls() { lb = MainWindow.Instance.lbConnect }, "Conectar");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Conectar a um servidor CoopAndreas");
            pt.Add(new Controls() { tb = MainWindow.Instance.tbNickname }, "Digite seu apelido...");
            pt.Add(new Controls() { tb = MainWindow.Instance.tbIpPort }, "Digite o IP:Porta do servidor (exemplo 127.0.0.1:6767)...");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbLanguage }, "Idioma:");
            // pr.Add(Program.MainForm.lb_maxplayers, "Máx. Jogadores:");
            // pr.Add(new Controls() { Program.MainForm.lb_startserver, "Iniciar um novo servidor CoopAndreas\n(não implementado, inicie manualmente)");
            // pr.Add(Program.MainForm.b_startserver, "Iniciar Servidor");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbConnectToServer }, "Conectar ao Servidor");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbServer }, "Servidor");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbConfig }, "Configuração");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbImportant },
            @"IMPORTANTE: Você está jogando a versão alpha/beta
do CoopAndreas, esta versão precisa de uma chave serial beta.
Não se preocupe, o mod e a chave serial são completamente
gratuitos. Vá ao nosso servidor Discord e digite o comando
abaixo em qualquer canal");
            pt.Add(new Controls() { tb = MainWindow.Instance.tbSerialKey }, "Digite sua chave serial...");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbCopyGen }, "Copiar");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbDiscordServer }, "Servidor Discord");
            pt.Add(new Controls() { lb = MainWindow.Instance.lbKillProcess }, "Finalizar processos do GTA-SA antes de iniciar:");

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
                if (control.Key.lb != null)
                {
                    control.Key.lb.Text = control.Value;
                    continue;
                }
                if (control.Key.tb != null)
                {
                    control.Key.tb.Watermark = control.Value;
                    continue;
                }
            }
        }
    }
}