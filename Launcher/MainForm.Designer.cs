namespace Launcher
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.connectPage = new System.Windows.Forms.TabPage();
            this.b_copy = new System.Windows.Forms.Button();
            this.tb_command = new System.Windows.Forms.TextBox();
            this.link_discord = new System.Windows.Forms.LinkLabel();
            this.lb_important = new System.Windows.Forms.Label();
            this.tb_serialKey = new System.Windows.Forms.TextBox();
            this.lb_serialkey = new System.Windows.Forms.Label();
            this.lb_connect = new System.Windows.Forms.Label();
            this.b_connect = new System.Windows.Forms.Button();
            this.ipportInput = new System.Windows.Forms.TextBox();
            this.lb_ipport = new System.Windows.Forms.Label();
            this.nicknameInput = new System.Windows.Forms.TextBox();
            this.lb_nickname = new System.Windows.Forms.Label();
            this.serverPage = new System.Windows.Forms.TabPage();
            this.b_startserver = new System.Windows.Forms.Button();
            this.lb_startserver = new System.Windows.Forms.Label();
            this.maxplayersInput = new System.Windows.Forms.NumericUpDown();
            this.portInput = new System.Windows.Forms.TextBox();
            this.lb_maxplayers = new System.Windows.Forms.Label();
            this.lb_port = new System.Windows.Forms.Label();
            this.configPage = new System.Windows.Forms.TabPage();
            this.languageCombo = new System.Windows.Forms.ComboBox();
            this.lb_language = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.connectPage.SuspendLayout();
            this.serverPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.maxplayersInput)).BeginInit();
            this.configPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.connectPage);
            this.tabControl1.Controls.Add(this.serverPage);
            this.tabControl1.Controls.Add(this.configPage);
            this.tabControl1.Location = new System.Drawing.Point(13, 13);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(319, 337);
            this.tabControl1.TabIndex = 0;
            // 
            // connectPage
            // 
            this.connectPage.BackColor = System.Drawing.Color.White;
            this.connectPage.Controls.Add(this.b_copy);
            this.connectPage.Controls.Add(this.tb_command);
            this.connectPage.Controls.Add(this.link_discord);
            this.connectPage.Controls.Add(this.lb_important);
            this.connectPage.Controls.Add(this.tb_serialKey);
            this.connectPage.Controls.Add(this.lb_serialkey);
            this.connectPage.Controls.Add(this.lb_connect);
            this.connectPage.Controls.Add(this.b_connect);
            this.connectPage.Controls.Add(this.ipportInput);
            this.connectPage.Controls.Add(this.lb_ipport);
            this.connectPage.Controls.Add(this.nicknameInput);
            this.connectPage.Controls.Add(this.lb_nickname);
            this.connectPage.Location = new System.Drawing.Point(4, 22);
            this.connectPage.Name = "connectPage";
            this.connectPage.Padding = new System.Windows.Forms.Padding(3);
            this.connectPage.Size = new System.Drawing.Size(311, 311);
            this.connectPage.TabIndex = 0;
            this.connectPage.Text = "Connect";
            // 
            // b_copy
            // 
            this.b_copy.Location = new System.Drawing.Point(135, 204);
            this.b_copy.Name = "b_copy";
            this.b_copy.Size = new System.Drawing.Size(43, 23);
            this.b_copy.TabIndex = 11;
            this.b_copy.Text = "Copy";
            this.b_copy.UseVisualStyleBackColor = true;
            this.b_copy.Click += new System.EventHandler(this.b_copy_Click);
            // 
            // tb_command
            // 
            this.tb_command.Location = new System.Drawing.Point(29, 204);
            this.tb_command.Name = "tb_command";
            this.tb_command.ReadOnly = true;
            this.tb_command.Size = new System.Drawing.Size(99, 20);
            this.tb_command.TabIndex = 10;
            this.tb_command.Text = "Loading...";
            // 
            // link_discord
            // 
            this.link_discord.AutoSize = true;
            this.link_discord.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.link_discord.Location = new System.Drawing.Point(184, 207);
            this.link_discord.Name = "link_discord";
            this.link_discord.Size = new System.Drawing.Size(97, 16);
            this.link_discord.TabIndex = 9;
            this.link_discord.TabStop = true;
            this.link_discord.Text = "Discord Server";
            this.link_discord.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
            // 
            // lb_important
            // 
            this.lb_important.AutoSize = true;
            this.lb_important.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lb_important.Location = new System.Drawing.Point(6, 112);
            this.lb_important.Name = "lb_important";
            this.lb_important.Size = new System.Drawing.Size(305, 65);
            this.lb_important.TabIndex = 8;
            this.lb_important.Text = resources.GetString("lb_important.Text");
            // 
            // tb_serialKey
            // 
            this.tb_serialKey.Location = new System.Drawing.Point(70, 89);
            this.tb_serialKey.Name = "tb_serialKey";
            this.tb_serialKey.Size = new System.Drawing.Size(180, 20);
            this.tb_serialKey.TabIndex = 7;
            this.tb_serialKey.TextChanged += new System.EventHandler(this.tb_serialKey_TextChanged);
            // 
            // lb_serialkey
            // 
            this.lb_serialkey.AutoSize = true;
            this.lb_serialkey.Location = new System.Drawing.Point(6, 92);
            this.lb_serialkey.Name = "lb_serialkey";
            this.lb_serialkey.Size = new System.Drawing.Size(56, 13);
            this.lb_serialkey.TabIndex = 6;
            this.lb_serialkey.Text = "Serial key:";
            // 
            // lb_connect
            // 
            this.lb_connect.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.lb_connect.AutoSize = true;
            this.lb_connect.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lb_connect.Location = new System.Drawing.Point(67, 3);
            this.lb_connect.Name = "lb_connect";
            this.lb_connect.Size = new System.Drawing.Size(188, 15);
            this.lb_connect.TabIndex = 5;
            this.lb_connect.Text = "Connect to a CoopAndreas server";
            this.lb_connect.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // b_connect
            // 
            this.b_connect.Location = new System.Drawing.Point(29, 235);
            this.b_connect.Name = "b_connect";
            this.b_connect.Size = new System.Drawing.Size(250, 70);
            this.b_connect.TabIndex = 4;
            this.b_connect.Text = "CONNECT";
            this.b_connect.UseVisualStyleBackColor = true;
            this.b_connect.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // ipportInput
            // 
            this.ipportInput.Location = new System.Drawing.Point(70, 63);
            this.ipportInput.Name = "ipportInput";
            this.ipportInput.Size = new System.Drawing.Size(180, 20);
            this.ipportInput.TabIndex = 3;
            this.ipportInput.TextChanged += new System.EventHandler(this.ipportInput_TextChanged);
            // 
            // lb_ipport
            // 
            this.lb_ipport.AutoSize = true;
            this.lb_ipport.Location = new System.Drawing.Point(6, 66);
            this.lb_ipport.Name = "lb_ipport";
            this.lb_ipport.Size = new System.Drawing.Size(53, 13);
            this.lb_ipport.TabIndex = 2;
            this.lb_ipport.Text = "IP:PORT:";
            // 
            // nicknameInput
            // 
            this.nicknameInput.Location = new System.Drawing.Point(70, 36);
            this.nicknameInput.Name = "nicknameInput";
            this.nicknameInput.Size = new System.Drawing.Size(180, 20);
            this.nicknameInput.TabIndex = 1;
            this.nicknameInput.TextChanged += new System.EventHandler(this.nicknameInput_TextChanged);
            // 
            // lb_nickname
            // 
            this.lb_nickname.AutoSize = true;
            this.lb_nickname.Location = new System.Drawing.Point(6, 39);
            this.lb_nickname.Name = "lb_nickname";
            this.lb_nickname.Size = new System.Drawing.Size(58, 13);
            this.lb_nickname.TabIndex = 0;
            this.lb_nickname.Text = "Nickname:";
            // 
            // serverPage
            // 
            this.serverPage.BackColor = System.Drawing.Color.White;
            this.serverPage.Controls.Add(this.label1);
            this.serverPage.Controls.Add(this.b_startserver);
            this.serverPage.Controls.Add(this.lb_startserver);
            this.serverPage.Controls.Add(this.maxplayersInput);
            this.serverPage.Controls.Add(this.portInput);
            this.serverPage.Controls.Add(this.lb_maxplayers);
            this.serverPage.Controls.Add(this.lb_port);
            this.serverPage.Location = new System.Drawing.Point(4, 22);
            this.serverPage.Name = "serverPage";
            this.serverPage.Padding = new System.Windows.Forms.Padding(3);
            this.serverPage.Size = new System.Drawing.Size(311, 311);
            this.serverPage.TabIndex = 1;
            this.serverPage.Text = "Server";
            // 
            // b_startserver
            // 
            this.b_startserver.Location = new System.Drawing.Point(30, 100);
            this.b_startserver.Name = "b_startserver";
            this.b_startserver.Size = new System.Drawing.Size(250, 70);
            this.b_startserver.TabIndex = 7;
            this.b_startserver.Text = "START SERVER";
            this.b_startserver.UseVisualStyleBackColor = true;
            // 
            // lb_startserver
            // 
            this.lb_startserver.AutoSize = true;
            this.lb_startserver.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lb_startserver.Location = new System.Drawing.Point(67, 3);
            this.lb_startserver.Name = "lb_startserver";
            this.lb_startserver.Size = new System.Drawing.Size(181, 15);
            this.lb_startserver.TabIndex = 6;
            this.lb_startserver.Text = "Start a new CoopAndreas server";
            this.lb_startserver.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // maxplayersInput
            // 
            this.maxplayersInput.Location = new System.Drawing.Point(218, 52);
            this.maxplayersInput.Name = "maxplayersInput";
            this.maxplayersInput.Size = new System.Drawing.Size(57, 20);
            this.maxplayersInput.TabIndex = 3;
            this.maxplayersInput.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // portInput
            // 
            this.portInput.Location = new System.Drawing.Point(85, 52);
            this.portInput.Name = "portInput";
            this.portInput.Size = new System.Drawing.Size(53, 20);
            this.portInput.TabIndex = 2;
            this.portInput.Text = "6767";
            // 
            // lb_maxplayers
            // 
            this.lb_maxplayers.AutoSize = true;
            this.lb_maxplayers.Location = new System.Drawing.Point(140, 55);
            this.lb_maxplayers.Name = "lb_maxplayers";
            this.lb_maxplayers.Size = new System.Drawing.Size(67, 13);
            this.lb_maxplayers.TabIndex = 1;
            this.lb_maxplayers.Text = "Max Players:";
            // 
            // lb_port
            // 
            this.lb_port.AutoSize = true;
            this.lb_port.Location = new System.Drawing.Point(50, 55);
            this.lb_port.Name = "lb_port";
            this.lb_port.Size = new System.Drawing.Size(29, 13);
            this.lb_port.TabIndex = 0;
            this.lb_port.Text = "Port:";
            // 
            // configPage
            // 
            this.configPage.BackColor = System.Drawing.Color.White;
            this.configPage.Controls.Add(this.languageCombo);
            this.configPage.Controls.Add(this.lb_language);
            this.configPage.Location = new System.Drawing.Point(4, 22);
            this.configPage.Name = "configPage";
            this.configPage.Padding = new System.Windows.Forms.Padding(3);
            this.configPage.Size = new System.Drawing.Size(311, 311);
            this.configPage.TabIndex = 2;
            this.configPage.Text = "Config";
            // 
            // languageCombo
            // 
            this.languageCombo.FormattingEnabled = true;
            this.languageCombo.Items.AddRange(new object[] {
            "English",
            "Русский",
            "Português",
            "Українська"});
            this.languageCombo.Location = new System.Drawing.Point(101, 11);
            this.languageCombo.Name = "languageCombo";
            this.languageCombo.Size = new System.Drawing.Size(161, 21);
            this.languageCombo.TabIndex = 1;
            this.languageCombo.SelectedIndexChanged += new System.EventHandler(this.languageCombo_SelectedIndexChanged);
            // 
            // lb_language
            // 
            this.lb_language.AutoSize = true;
            this.lb_language.Location = new System.Drawing.Point(37, 14);
            this.lb_language.Name = "lb_language";
            this.lb_language.Size = new System.Drawing.Size(58, 13);
            this.lb_language.TabIndex = 0;
            this.lb_language.Text = "Language:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(26, 41);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(265, 96);
            this.label1.TabIndex = 8;
            this.label1.Text = "!!!\r\nTHIS TAB IS TEMPORARILY \r\nUNAVAILABLE, LAUNCH VIA \r\n`server.exe`";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(344, 362);
            this.Controls.Add(this.tabControl1);
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "CoopAndreas Launcher";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.tabControl1.ResumeLayout(false);
            this.connectPage.ResumeLayout(false);
            this.connectPage.PerformLayout();
            this.serverPage.ResumeLayout(false);
            this.serverPage.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.maxplayersInput)).EndInit();
            this.configPage.ResumeLayout(false);
            this.configPage.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.TabControl tabControl1;
        public System.Windows.Forms.TabPage connectPage;
        public System.Windows.Forms.TabPage serverPage;
        public System.Windows.Forms.TabPage configPage;
        public System.Windows.Forms.Button b_connect;
        public System.Windows.Forms.TextBox ipportInput;
        public System.Windows.Forms.Label lb_ipport;
        public System.Windows.Forms.TextBox nicknameInput;
        public System.Windows.Forms.Label lb_nickname;
        public System.Windows.Forms.Label lb_maxplayers;
        public System.Windows.Forms.Label lb_port;
        public System.Windows.Forms.Label lb_connect;
        public System.Windows.Forms.NumericUpDown maxplayersInput;
        public System.Windows.Forms.TextBox portInput;
        public System.Windows.Forms.Label lb_startserver;
        public System.Windows.Forms.Label lb_language;
        public System.Windows.Forms.Button b_startserver;
        public System.Windows.Forms.ComboBox languageCombo;
        public System.Windows.Forms.Label lb_important;
        public System.Windows.Forms.TextBox tb_serialKey;
        public System.Windows.Forms.Label lb_serialkey;
        public System.Windows.Forms.LinkLabel link_discord;
        public System.Windows.Forms.TextBox tb_command;
        public System.Windows.Forms.Button b_copy;
        private System.Windows.Forms.Label label1;
    }
}

