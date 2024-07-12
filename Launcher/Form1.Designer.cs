namespace Launcher
{
    partial class Form1
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
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.connectPage = new System.Windows.Forms.TabPage();
            this.connectlabelText = new System.Windows.Forms.Label();
            this.connectButton = new System.Windows.Forms.Button();
            this.ipportInput = new System.Windows.Forms.TextBox();
            this.ipportText = new System.Windows.Forms.Label();
            this.nicknameInput = new System.Windows.Forms.TextBox();
            this.nicknameText = new System.Windows.Forms.Label();
            this.serverPage = new System.Windows.Forms.TabPage();
            this.startserverButton = new System.Windows.Forms.Button();
            this.serverlabelText = new System.Windows.Forms.Label();
            this.maxplayersInput = new System.Windows.Forms.NumericUpDown();
            this.portInput = new System.Windows.Forms.TextBox();
            this.maxplayerText = new System.Windows.Forms.Label();
            this.portText = new System.Windows.Forms.Label();
            this.configPage = new System.Windows.Forms.TabPage();
            this.languageCombo = new System.Windows.Forms.ComboBox();
            this.languageText = new System.Windows.Forms.Label();
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
            this.tabControl1.Size = new System.Drawing.Size(245, 225);
            this.tabControl1.TabIndex = 0;
            // 
            // connectPage
            // 
            this.connectPage.BackColor = System.Drawing.Color.White;
            this.connectPage.Controls.Add(this.connectlabelText);
            this.connectPage.Controls.Add(this.connectButton);
            this.connectPage.Controls.Add(this.ipportInput);
            this.connectPage.Controls.Add(this.ipportText);
            this.connectPage.Controls.Add(this.nicknameInput);
            this.connectPage.Controls.Add(this.nicknameText);
            this.connectPage.Location = new System.Drawing.Point(4, 22);
            this.connectPage.Name = "connectPage";
            this.connectPage.Padding = new System.Windows.Forms.Padding(3);
            this.connectPage.Size = new System.Drawing.Size(237, 199);
            this.connectPage.TabIndex = 0;
            this.connectPage.Text = "Connect";
            // 
            // connectlabelText
            // 
            this.connectlabelText.AutoSize = true;
            this.connectlabelText.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.connectlabelText.Location = new System.Drawing.Point(25, 3);
            this.connectlabelText.Name = "connectlabelText";
            this.connectlabelText.Size = new System.Drawing.Size(188, 15);
            this.connectlabelText.TabIndex = 5;
            this.connectlabelText.Text = "Connect to a CoopAndreas server";
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(32, 104);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(173, 66);
            this.connectButton.TabIndex = 4;
            this.connectButton.Text = "CONNECT";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // ipportInput
            // 
            this.ipportInput.Location = new System.Drawing.Point(70, 52);
            this.ipportInput.Name = "ipportInput";
            this.ipportInput.Size = new System.Drawing.Size(161, 20);
            this.ipportInput.TabIndex = 3;
            // 
            // ipportText
            // 
            this.ipportText.AutoSize = true;
            this.ipportText.Location = new System.Drawing.Point(6, 55);
            this.ipportText.Name = "ipportText";
            this.ipportText.Size = new System.Drawing.Size(53, 13);
            this.ipportText.TabIndex = 2;
            this.ipportText.Text = "IP:PORT:";
            // 
            // nicknameInput
            // 
            this.nicknameInput.Location = new System.Drawing.Point(70, 25);
            this.nicknameInput.Name = "nicknameInput";
            this.nicknameInput.Size = new System.Drawing.Size(161, 20);
            this.nicknameInput.TabIndex = 1;
            // 
            // nicknameText
            // 
            this.nicknameText.AutoSize = true;
            this.nicknameText.Location = new System.Drawing.Point(6, 28);
            this.nicknameText.Name = "nicknameText";
            this.nicknameText.Size = new System.Drawing.Size(58, 13);
            this.nicknameText.TabIndex = 0;
            this.nicknameText.Text = "Nickname:";
            // 
            // serverPage
            // 
            this.serverPage.BackColor = System.Drawing.Color.White;
            this.serverPage.Controls.Add(this.startserverButton);
            this.serverPage.Controls.Add(this.serverlabelText);
            this.serverPage.Controls.Add(this.maxplayersInput);
            this.serverPage.Controls.Add(this.portInput);
            this.serverPage.Controls.Add(this.maxplayerText);
            this.serverPage.Controls.Add(this.portText);
            this.serverPage.Location = new System.Drawing.Point(4, 22);
            this.serverPage.Name = "serverPage";
            this.serverPage.Padding = new System.Windows.Forms.Padding(3);
            this.serverPage.Size = new System.Drawing.Size(237, 199);
            this.serverPage.TabIndex = 1;
            this.serverPage.Text = "Server";
            // 
            // startserverButton
            // 
            this.startserverButton.Location = new System.Drawing.Point(31, 100);
            this.startserverButton.Name = "startserverButton";
            this.startserverButton.Size = new System.Drawing.Size(178, 68);
            this.startserverButton.TabIndex = 7;
            this.startserverButton.Text = "START SERVER";
            this.startserverButton.UseVisualStyleBackColor = true;
            // 
            // serverlabelText
            // 
            this.serverlabelText.AutoSize = true;
            this.serverlabelText.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.serverlabelText.Location = new System.Drawing.Point(28, 3);
            this.serverlabelText.Name = "serverlabelText";
            this.serverlabelText.Size = new System.Drawing.Size(181, 15);
            this.serverlabelText.TabIndex = 6;
            this.serverlabelText.Text = "Start a new CoopAndreas server";
            // 
            // maxplayersInput
            // 
            this.maxplayersInput.Location = new System.Drawing.Point(172, 33);
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
            this.portInput.Location = new System.Drawing.Point(43, 33);
            this.portInput.Name = "portInput";
            this.portInput.Size = new System.Drawing.Size(53, 20);
            this.portInput.TabIndex = 2;
            this.portInput.Text = "6767";
            // 
            // maxplayerText
            // 
            this.maxplayerText.AutoSize = true;
            this.maxplayerText.Location = new System.Drawing.Point(102, 36);
            this.maxplayerText.Name = "maxplayerText";
            this.maxplayerText.Size = new System.Drawing.Size(67, 13);
            this.maxplayerText.TabIndex = 1;
            this.maxplayerText.Text = "Max Players:";
            // 
            // portText
            // 
            this.portText.AutoSize = true;
            this.portText.Location = new System.Drawing.Point(8, 36);
            this.portText.Name = "portText";
            this.portText.Size = new System.Drawing.Size(29, 13);
            this.portText.TabIndex = 0;
            this.portText.Text = "Port:";
            // 
            // configPage
            // 
            this.configPage.BackColor = System.Drawing.Color.White;
            this.configPage.Controls.Add(this.languageCombo);
            this.configPage.Controls.Add(this.languageText);
            this.configPage.Location = new System.Drawing.Point(4, 22);
            this.configPage.Name = "configPage";
            this.configPage.Padding = new System.Windows.Forms.Padding(3);
            this.configPage.Size = new System.Drawing.Size(237, 199);
            this.configPage.TabIndex = 2;
            this.configPage.Text = "Config";
            // 
            // languageCombo
            // 
            this.languageCombo.FormattingEnabled = true;
            this.languageCombo.Items.AddRange(new object[] {
            "English",
            "Русский",
            "Português"});
            this.languageCombo.Location = new System.Drawing.Point(70, 11);
            this.languageCombo.Name = "languageCombo";
            this.languageCombo.Size = new System.Drawing.Size(161, 21);
            this.languageCombo.TabIndex = 1;
            // 
            // languageText
            // 
            this.languageText.AutoSize = true;
            this.languageText.Location = new System.Drawing.Point(6, 14);
            this.languageText.Name = "languageText";
            this.languageText.Size = new System.Drawing.Size(58, 13);
            this.languageText.TabIndex = 0;
            this.languageText.Text = "Language:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(270, 250);
            this.Controls.Add(this.tabControl1);
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "CoopAndreas Launcher";
            this.Load += new System.EventHandler(this.Form1_Load);
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

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage connectPage;
        private System.Windows.Forms.TabPage serverPage;
        private System.Windows.Forms.TabPage configPage;
        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.TextBox ipportInput;
        private System.Windows.Forms.Label ipportText;
        private System.Windows.Forms.TextBox nicknameInput;
        private System.Windows.Forms.Label nicknameText;
        private System.Windows.Forms.Label maxplayerText;
        private System.Windows.Forms.Label portText;
        private System.Windows.Forms.Label connectlabelText;
        private System.Windows.Forms.NumericUpDown maxplayersInput;
        private System.Windows.Forms.TextBox portInput;
        private System.Windows.Forms.Label serverlabelText;
        private System.Windows.Forms.Label languageText;
        private System.Windows.Forms.Button startserverButton;
        private System.Windows.Forms.ComboBox languageCombo;
    }
}

