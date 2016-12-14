using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Life
{
    public class Pool
    {
        /// <summary>
        /// Pole buněk, aktuální a další generace
        /// </summary>
        public bool[,] pool;
        public bool[,] next;

        /// <summary>
        /// Velikost, Generace buněk
        /// </summary>
        public int Size = 0;
        public int Generation = 0;

        private Task processTask;
        public Pool() { }
        public bool this[int x, int y]
        {
            get { return this.pool[x, y]; }
            set { this.pool[x, y] = value; }
        }

        /// <summary>
        /// Nastavi velikost GoL pole
        /// </summary>
        /// <param name="size">Velikost pole (size x size)</param>
        public void SetPool(int size)
        {
            this.Size = size;
            pool = new bool[size, size];
            next = new bool[size, size];
        }

        /// <summary>
        /// Počítá živé sousedy buňky
        /// </summary>
        /// <param name="world">Vstupní pool buněk </param>
        /// <param name="size">Velikost poolu</param>
        /// <param name="x">X buňky</param>
        /// <param name="y">Y buňky</param>
        /// <returns></returns>
        private static int CountLive(bool[,] world, int size, int x, int y)
        {
            //Region (offset) ve kterém hledáme
            int[,] region = new int[,] { { -1, 0 }, { -1, 1 },
                { 0, 1 }, { 1, 1 }, { 1,0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } };
            int result = 0;

            for (int i = 0; i <= 7; i++)
            {
                int region_count = i;

                int regionx = x + region[region_count, 0];
                int regiony = y + region[region_count, 1];

                bool range = regionx < 0 || regionx >= size
                                   | regiony < 0 || regiony >= size;
                if (range == false)
                {
                    int result2 = world[x + region[region_count, 0],
                        y + region[region_count, 1]] ? 1 : 0;

                    result = result + result2;
                }
            }
            return result;

        }

        /// <summary>
        /// Spustit generaci GoL
        /// </summary>
        public void Begin()
        {
            //Pouze pokud je předchozí task dokončen 
            if (this.processTask == null)
            {
                this.processTask = this.NextGeneration();
            }
        }

        /// <summary>
        /// Přepnout buňku mezi živá/mrtvá
        /// </summary>
        /// <param name="x">x buňky v poli</param>
        /// <param name="y">y buňky v poli</param>
        /// <returns></returns>
        public bool SwitchCell(int x, int y)
        {
            bool currentValue = this.pool[x, y];
            return this.pool[x, y] = !currentValue;
        }

        /// <summary>
        /// Updatuje pole (Přesouvá výsledky z next pole do aktuálního pole) po dokončení všech threadu
        /// </summary>
        public void Update()
        {
            if (this.processTask != null && this.processTask.IsCompleted)
            {
                var flip = this.next;
                this.next = this.pool;
                this.pool = flip;
                Generation++;

                this.processTask = this.NextGeneration();
            }
        }

        /// <summary>
        /// Generovat další generaci herního pole - zde jsou uplatněny GoL pravidla
        /// </summary>
        /// <returns></returns>
        private Task NextGeneration()
        {
            return Task.Factory.StartNew(() =>
            {
                Parallel.For(0, Size, x =>
                {
                    Parallel.For(0, Size, y =>
                    {
                        // Spočitat živé sousedy
                        int n = CountLive(pool, Size, x, y);

                //Pravidlo 1 - Ziva bunka s mene, nez dvema zivymi sousedy umira.
                //Pravidlo 2 - Ziva bunka s vice, nez tremi zivymi sousedy umira. 
                //Pravidlo 3 - Ziva bunka s dvoumi nebo tremi sousedy preziva beze zmeny do dalsi generace.	
                        next[x, y] = false;
                        if (pool[x, y] && (n == 2 || 
                                        n == 3))
                            next[x, y] = true;

                //Pravidlo 4 - Mrtva bunka, s presne tremi zivymi sousedy, opet oziva.
                        else if (!pool[x, y] && 
                                  n == 3)
                            next[x, y] = true;
                    });
                });
            });
        }
    }

}
