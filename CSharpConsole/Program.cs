using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CSharpConsole
{
    class Program
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct JOYCONSTATE
        {
            public int Status;
            public uint POV;
            public int A;
            public int B;
            public int Y;
            public int X;
            public int Minus;
            public int Plus;
            public int SL;
            public int SR;
            public int JoystickClick;
        };
        [StructLayout(LayoutKind.Sequential)]
        public struct JOYCONS
        {
            public JOYCONSTATE Left;
            public JOYCONSTATE Right;
        };

        [DllImport("JoyConDLL")]
        public static extern int tryConnect();
        [DllImport("JoyConDLL")]
        public static extern JOYCONS pollJoycons();
        [DllImport("JoyConDLL")]
        public static extern int getJoyconsAmount();

        static int ConnRes = -1;
        static JOYCONS jc;

        static void Main(string[] args)
        {
            while (true)
            {
                if (ConnRes != 0)
                {
                    ConnRes = tryConnect();
                    Console.WriteLine("Failed...");
                }
                else
                {
                    jc = pollJoycons();
                    if (jc.Left.Status != 0 && jc.Right.Status != 0)
                    {
                        ConnRes = -1;
                        Console.WriteLine("Connection reset. Status was L - " + jc.Left.Status + " R - " + jc.Right.Status);
                    }
                    else
                    {
                        Console.WriteLine("Devices - " + getJoyconsAmount() + " Status was L - " + jc.Left.Status + " R - " + jc.Right.Status);
                        if (getJoyconsAmount() < 2)
                        {
                            ConnRes = -1;
                            Console.WriteLine("Retrying for 2");
                        }
                        //Console.WriteLine("A: " + jc.Left.A + " B: " +jc.Left.B + " X: " + jc.Left.X + " Y: " + jc.Left.Y);
                        //Console.WriteLine("A: " + jc.Right.A + " B: " +jc.Right.B + " X: " + jc.Right.X + " Y: " + jc.Right.Y);
                    }
                }
            }
        }
    }
}
