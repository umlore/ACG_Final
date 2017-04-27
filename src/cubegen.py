import random

#filename
f = open('bunnies_216_40k', 'w')

#timesteps
t = 7

#light timesteps
lt = 8

#cubesize (how many models for one size)
cubesize = 6

#spacing (spacing between models)
spacing = 1

#number of lights (randomly distributed)
numlights = 100

#max light intensity
mli = 1

#which file to generate grid of?
model = 'bunny40k'

#generate a grid
for i in range(0,cubesize):
	for j in range(0,cubesize):
		for k in range(0,cubesize):
			#declare one object
			f.write("o %s\n" % (model))
			#colors
			for c in range(0,t):
				f.write("c %f %f %f\n" % (random.random(),random.random(),random.random()))
			#positions
			for p in range(0,t):
				#f.write("p %f %f %f\n" % (i*4+random.random(), j*4 + random.random(), k*4 + random.random()))
				f.write("p %f %f %f\n" % (random.random()*cubesize*spacing, random.random()*cubesize*spacing, random.random()*cubesize*spacing))
			#timesteps
			#f.write("t 0\nt 1\nt 2\n")
			x = .5+random.random()*10
			f.write("t 0\n");
			for step in range(0,t):
				f.write("t %f\n" % (x))
				x += .5+random.random()*10

for i in range(0,numlights):
	f.write("l\n")
	for c in range(0,lt):
		f.write("lc %f %f %f\n" % (random.random(), random.random(),random.random()))
	for p in range(0,lt):
		f.write("lp %f %f %f\n" % (random.random()*cubesize*spacing,random.random()*cubesize*spacing, random.random()*cubesize*spacing))
	for inte in range(0,lt):
		f.write("li %f\n" % (random.random()*mli))
		#f.write("li %f\n");
	f.write("lt 0\n")
	x = .5+random.random()*10
	for step in range(0,lt):
		f.write("lt %f\n" % (x))
		x += .5+random.random()*10