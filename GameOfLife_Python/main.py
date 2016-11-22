# Python 2.7 - Windows !

# Pro vypocet casu vypracovani jedne generace
import time as timer
# Pro generovani nahodneho startovaci pole
import random as randomize
# Pro praci s thready, threadpoolem.
import multiprocessing
import threadpool
# Pro parsing argumentu pri spusteni.
import argparse
import sys 
# Pro cisteni konzole pri vypisu do CLI
import os

 # Nahodne generovane pocatecni pole
Random = [[1 if randomize.random() < 0.3 
					else 0 for x in range(20)] 
					for y in range(20)]

 # Glider test pravidel
Glider = [
	[0,1,0,0,0,0,0,0,0,0,0],
	[0,0,1,0,0,0,0,0,0,0,0],
	[1,1,1,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0]]
	
 # Blinkers test pravidel
Blinkers = [
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,1,0,0,0,0,0],
	[0,0,0,0,1,1,1,0,0,0,0],
	[0,0,0,0,0,1,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0]]
		
class life(object):
    """
    size :=  velikost startovni matice (size x size) - je ctverec
    threadCount  :=  pocet vytvorenych vlaken
	pattern :=  odkazuje na startovaci matici 
    """
    def __init__(self, size=10, threadCount=2, pattern=Random):
		# Priradime potrebne tributy.
        self.arraySize = size
		# Vytvorime threadpool o pozadovane velikosti.
        self.threadCount = threadCount
        self.pool = threadpool.ThreadPool(threadCount)
		# Stav bunek v poli - 1 v poli oznacuje zivou bunku, 0 naopak mrtvou.
        self.alive, self.dead = 1, 0
		
		
        # Priradime startovaci matici / pole.
        self.GameField = pattern	
		# Definuji pomocne ktere budeme vyuzivat pro ulozeni poctu zivych sousedu kazde bunky v hernim poli.
        self.NeighbourCountField = [[0 for x in range(self.arraySize)] for y in range(self.arraySize)]
  						
	"""	
	Princip implementace game of life pravidel v teto pripade zajisten tremi funkcemi:
	1. countAll := Spousti funkci countAlive na kazdou bunku v urcitem rozmezi radku pole.
	2. countAlive := Ulozi do naseho pomocneho pole (Na identicke misto) pocet zivych sousedu kazde bunky.
	3. checkRules := Prochazi pomocne pole a podle poctu sousedu prepina stav bunek - Zde dochazi k uplatneni GoL pravidel.								
	"""
	"""	
	Spousti funkci countAlive na kazdou bunku v urcitem rozmezi radku pole (Od first do last) - pro rozdeleni prace na poli do threadu.
	Zapisuje vysledky countAlive na patricne misto v pomocne matici NeighbourCountField.
	"""
    def countAll(self, first=0, last=0):
        for x in range(first, self.arraySize if last == 0 else last):
            for y in range(self.arraySize):
                self.NeighbourCountField[y][x] = self.countAlive(self.GameField, x, y)	
	"""	
	Pocet zivych sousedu na jednu urcitou bunku. 
	Funkce vraci pro urcitou bunku na pozici (x,y) pocet zivych sousedu.
	"""
    def countAlive(self, grid, x, y):
	neighbours = 0		
	for outside in range(x - 1, x + 2):
		for inside in range(y - 1, y + 2):
			if outside >= 0 and outside < len(grid[y]) and inside >= 0 and inside < len(grid):
				if grid[inside][outside] == self.alive:
					neighbours += 1			
	return neighbours - 1 if grid[y][x] == self.alive else neighbours

		
	"""	
	Prepinac mezi zivou a mrtvou bunkou v urcitem rozmezi radku pole (Od first do last).
	Prochazi pomocne pole a podle poctu sousedu prepina stav bunek - Zde dochazi k uplatneni GoL pravidel.
	"""
    def checkRules(self, first=0, last=0):
        for x in range(first, self.arraySize if last == 0 else last):
            for y in range(self.arraySize):
				# Pravidlo 1 - Ziva bunka s mene, nez dvema zivymi sousedy umira.
				# Pravidlo 2 - Ziva bunka s vice, nez tremi zivymi sousedy umira. 
				# Pravidlo 3 - Ziva bunka s dvoumi nebo tremi sousedy preziva beze zmeny do dalsi generace.	
                if self.GameField[y][x] == self.alive:		
                    if self.NeighbourCountField[y][x] < 2 or self.NeighbourCountField[y][x] > 3:
                        self.GameField[y][x] = self.dead			
				# Pravidlo 4 - Mrtva bunka, s presne tremi zivymi sousedy, opet oziva.
                elif self.NeighbourCountField[y][x] == 3:
                    self.GameField[y][x] = self.alive				
			
	"""
	Generuje dalsi generaci matice hry.
	Dochazi k rozdeleni herni matice podle poctu threadu. Kazdemu threadu je pak prirazeno urcite rozmezi radku ke zpracovani.
	"""
    def next(self):
		# Pole atributu rozmezi pro kazdy thread.
		# Podle poctu jsou threadum rozdeleny rozmezi radku herni matice do atributu first --> last.
		thread_range = [([], {
		'first': self.arraySize/self.threadCount * x, 
		'last': self.arraySize/self.threadCount * x + self.arraySize/self.threadCount
		}) for x in range(self.threadCount)]
		
	    # Do thread poolu jsou zaslany requesty pro vypocteni sousedu do pomocne matice. A ceka se na dokonceni..
		[self.pool.putRequest(req) 
		for req in threadpool.makeRequests(self.countAll, thread_range)]
		self.pool.wait()
		
		# Do thread poolu jsou zaslany requesty pro uplatneni GoL pravidel (checkRules funkce). A ceka se na dokonceni..
		[self.pool.putRequest(req) 
		for req in threadpool.makeRequests(self.checkRules, thread_range)]
		
		self.pool.wait()
		
    """
    Jednoduche vykresleni matice aktualni generace do konzole pomoci dvou cyklu pro x a y.
    """
    def draw(self):
		clear = lambda: os.system('cls')
		clear()
		for x in range(self.arraySize):
			for y in range(self.arraySize) :
				if self.GameField[y][x] == self.alive:
					print('X'),
				print ' ',
			print "\n"              				
					
    """
    Spustit simulaci.
    """
    def play(self):
	generation = 0
	while True:
		start = timer.time()
		generation += 1
		self.next()
		stop = timer.time()
		self.draw()
		
		print "Size: " + str(self.arraySize) + "x" + str(self.arraySize) 
		print "Generation: " + str(generation)
		print "Generated in: " + str((stop - start) * 100) + "ms"
		print "Threads: " + str(self.threadCount)			
		
		#Pozastaveni pro lepsi zobrazeni v konzoli...
		timer.sleep(0.3)
		
#Pocatecni cteni argumentu a spusteni GoL
parser = argparse.ArgumentParser(description='Game Of Life - reh0063')
parser.add_argument("-s", "--size", dest="size", help="Size of the default array (10 is default)", default=10, metavar="SIZE")
parser.add_argument("-t", "--threads", dest="threads", help="Number of threads to use", default=2, metavar="THREADS")
parser.add_argument("-p", "--pattern", dest="pattern",  help="Set the startup pattern (Random is default)", 
								default="random", metavar="[random | blinkers | glider]")
args = parser.parse_args()

if __name__ == '__main__':
	if args.pattern == 'blinkers':
		gol = life(size=10, threadCount = int(args.threads), pattern=Blinkers)		
	elif args.pattern == 'glider':
		gol = life(size=10, threadCount = int(args.threads), pattern=Glider)	
	else:
		gol = life(size= int(args.size), threadCount = int(args.threads), pattern=Random)
	gol.play()
