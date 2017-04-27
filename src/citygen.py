import random
import math

f = open('cityscape','w')

numbuildings = 100
scenemaxheight = 100

model = 'cube2.obj'

blockx = 2
blockz = 7

for x in range(0,numbuildings):
	for z in range(0,numbuildings):
		print (x % (blockx+1))
		if (((x % (blockx+1)) and (z % (blockz+1)))):
			x0 = z0 = numbuildings/2
			dist = (math.sqrt((x-x0)**2 + (z-z0)**2))/x0
			dist = dist**.1
			dist *= x0
			dist = x0 - dist
			dist /= x0
			maxheight = scenemaxheight * dist

			height = int((random.random()**3)*maxheight) + 1
			for y in range(0,height):
				f.write("o %s\n" % (model))
				f.write("c %f %f %f\n" % (random.random(),random.random(),random.random()))
				f.write("p %f %f %f\n" % (x,y,z))
				f.write("t 0\nt 1\n")

f.write("l\nlc 1 1 1\nlp 1 1 1\nli 1\nlt 0\nlt 1")