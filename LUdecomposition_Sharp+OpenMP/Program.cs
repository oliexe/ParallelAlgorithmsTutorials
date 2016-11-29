using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace LUdecomposition
{
    public class Program
    {
        private int s = 0; // Velikost čtvercové matice

        private double[,] A = null; // Vstupní matice
        private double[,] U = null; // Matice U
        private double[,] L = null; // Matice L

        public static void Main(string[] args)
        {
            Program Matrix = new Program();
            Stopwatch sw = new Stopwatch();

            //1: Detailni dekompozice s vykreslením matic a overenim.
            double[,] ServicePoint = new double[10, 9];
           
            //Vstupni matice
            double[,] mat = new double[4, 4] {
                              {5,5,5,6}, 
                              {7,5,1,1}, 
                              {1,2,1,1},
                              {5,1,2,1}};

            Matrix.init(mat, 4);
            sw.Start();
            Matrix.luPar(Matrix.A);
            sw.Stop();
            Console.WriteLine("(Input) A =");
            Matrix.print(Matrix.A);
            Console.WriteLine("L = ");
            Matrix.print(Matrix.L);
            Console.WriteLine("U = ");
            Matrix.print(Matrix.U);
            Console.WriteLine("(Check) L x U =");
            Matrix.print(Matrix.checkPar(Matrix.L, Matrix.U));
            Console.WriteLine("Decomposed in = {0}", sw.Elapsed);
            sw.Reset();
            Console.WriteLine("Press any key for performance tests...");
            Console.ReadLine();

            //2: Test výkonnosti - matice 100x100 až 2000x2000
            Console.Clear();
            Console.WriteLine("Performance test on random generated matrices");
            Console.WriteLine("***********************************************");
            for (int test = 1; test <= 20; test++)
            {
                Matrix.init(Matrix.generator(test * 100), test * 100);
                sw.Start();
                Matrix.luPar(Matrix.A);
                sw.Stop();
                Console.WriteLine("A = {0} x {0} - decomposed in = {1}", Matrix.s, sw.Elapsed);
                sw.Reset();
            }
            Console.ReadLine();
        }

        /// <summary>
        /// Funkce pro generování maticí pro LU testy.
        /// </summary>
        /// <param name="s">Velikost matice pro vygenerování</param>
        public double[,] generator(int s)
        {
            double[,] rand = new double[s, s];
            for (int i = 0; i < s; i++)
            {
                for (int j = 0; j < s; j++)
                {
                    if (i <= j)
                        rand[i, j] = i + 1;
                    else
                        rand[i, j] = j + 1;
                }
            }
            return rand;
        }

        /// <summary>
        /// Funkce pro tisk matice.
        /// </summary>
        /// <param name="arr">Vstupní matice pro tisk</param>
        public void print(double[,] arr)
        {
            for (int i = 0; i < s; i++)
            {
                for (int j = 0; j < s; j++)
                    Console.Write(string.Format(" {0,2} ", arr[i, j]));
                Console.Write(Environment.NewLine);
            }
            Console.Write(Environment.NewLine);
        }

        /// <summary>
        /// Inicializace matice (vytvorit pole o prislusnych velikostech).
        /// </summary>
        /// <param name="values">Hodnoty do vstupní matice A</param>
        /// <param name="s">Velikost vstupní matice</param>
        public void init(double[,] values, int s)
        {
            this.s = s;
            A = new double[s, s];
            U = new double[s, s];
            L = new double[s, s];
            A = values;
        }

        /// <summary>
        /// Paralelní zpracování LU rozkladu. Realizace pomoci Parallel.for
        /// </summary>
        /// <param name="A">Vstupní matice pro rozklad.</param>
        public void luPar(double[,] A)
        {
            for (int outerFor = 0; outerFor < s; outerFor++)
            {
                //L
                Parallel.For(outerFor, s, i =>
               {
                   double sum_l = 0;
                   for (int innerFor = 0; innerFor < outerFor; innerFor++)
                       sum_l = sum_l + L[i, innerFor] * U[innerFor, outerFor];
                   L[i, outerFor] = A[i, outerFor] - sum_l;
               });

                //U
                Parallel.For(outerFor, s, i =>
               {
                   double sum_u = 0;
                   for (int innerFor = 0; innerFor < outerFor; innerFor++)
                       sum_u = sum_u + L[outerFor, innerFor] * U[innerFor, i];
                   U[outerFor, i] = (A[outerFor, i] - sum_u) / L[outerFor, outerFor];
               });
            }
        }

        /// <summary>
        /// Paralelní ověřeni správnosti rozkladu.
        /// Nasobeni dvou matic L a U v Parallel.For
        /// </summary>
        /// <param name="L">Vstupní matice L.</param>
        /// <param name="U">Vstupní matice U.</param>
        public double[,] checkPar(double[,] L, double[,] U)
        {
            double[,] result = new double[s, s];
           Parallel.For(0, s, i =>
              {
                  for (int j = 0; j < s; ++j)
                      for (int k = 0; k < s; ++k)
                          result[i, j] += L[i, k] * U[k, j];
              }
            );
            return result;
        }
    }
}