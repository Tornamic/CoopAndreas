using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Server
{
    public class Program
    {
        static void Main(string[] args)
        {
            Connection connection = new Connection();
            connection.Start(7777, 4);
            Console.ReadKey();
        }
    }
    public struct CVector
    {
        public float X;
        public float Y;
        public float Z;
        public CVector(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }
}
