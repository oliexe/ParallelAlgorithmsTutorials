using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Life
{
    class Program
    {
        public static Pool _sim = new Pool();

        static void Main(string[] args)
        {
            if (args.Length > 0)
            ReadInput(args[0]);
            else
            ReadInput("sample_input.txt");

            _sim.Begin();

            while (true)
            {
                Console.Clear();
                var watch = System.Diagnostics.Stopwatch.StartNew();
                _sim.Update();
                watch.Stop();
                Print(_sim, watch.Elapsed.Ticks, _sim.Generation);

                Thread.Sleep((int)TimeSpan.FromSeconds(0.3).TotalMilliseconds);
            }
        }

        /// <summary>
        /// Importuje textový soubor do GoL pole
        /// </summary>
        /// <param name="fileName">Vstupní textový soubor</param>
        private static void ReadInput(string fileName)
        {
            String input = File.ReadAllText(fileName);

            int i = 0, j = 0;
            int[,] result = new int[25, 25];
            foreach (var row in input.Split('\n'))
            {
                j = 0;
                foreach (var col in row.Trim().Split(' '))
                {
                    result[i, j] = int.Parse(col.Trim());
                    j++;
                }
                i++;
            }

            int rowLength = result.GetLength(0);
            int colLength = result.GetLength(1);
            _sim.SetPool(result.GetLength(0));

            for (int printx = 0; printx < rowLength; printx++)
            {
                for (int printy = 0; printy < colLength; printy++)
                {
                    if (result[printx, printy] == 1)
                    _sim.SwitchCell(printx, printy);
                }
            }
        }

        /// <summary>
        /// Tisk pole do konzole
        /// </summary>
        /// <param name="sim">Vstupní Simulace</param>
        /// <param name="time">Čas vygenerování</param>
        /// <param name="generation">Generace pole</param>
        private static void Print(Pool sim, double time, int generation)
        {
            for (int printy = 0; printy < sim.Size; printy++)
            {
                for (int printx = 0; printx < sim.Size; printx++) 
                    Console.Write(sim[printx, printy] ? " X " : " - ");
                Console.WriteLine();
            }
            Console.WriteLine("Generation : " + generation);
            Console.WriteLine("Generated in: " + time + " ticks");
        }
    }
}
