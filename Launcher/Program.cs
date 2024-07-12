using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Launcher
{
    internal static class Program
    {
        public static MainForm MainForm;
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            MainForm = new MainForm();
            Application.Run(MainForm);
        }
    }
}
