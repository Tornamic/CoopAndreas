using Riptide;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server
{
    public class RemotePlayer
    {
        public static List<RemotePlayer> All = new List<RemotePlayer>();
        public ushort NetID { get; set; }

        
    }
}
