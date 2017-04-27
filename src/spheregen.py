import random
import math

f = open('sphere_20000l','w')

radius = 1.1

numlights = 20000

lt = 20

sizelights = .03

model = 'sphere'

f.write("o %s\n" % (model))
f.write("c 1 1 1\np 0 0 0\nt 0\nt 1\n")

for i in range(0,numlights):
	#x = random.random()
	#y = random.random()
	#z = random.random()
	polar = random.random()*math.pi
	azimuth = random.random()*math.pi*2

	

	f.write("l\n")
	for c in range(0,lt):
		f.write("lc %f %f %f\n" % (random.random(), random.random(),random.random()))
	for p in range(0,lt):
		x = math.sin(polar)*math.cos(azimuth)*radius
		y = math.sin(polar)*math.sin(azimuth)*radius
		z = math.cos(polar)*radius
		f.write('lp %f %f %f\n' % (x, y, z))

		polar += math.pi/lt*2
		azimuth -= math.pi/lt*2
	for inte in range(0,lt):
		f.write("li %f\n" % (sizelights))
	f.write("lt 0\n")
	x = random.random()+.1
	for step in range(0,lt):
		f.write("lt %f\n" % (x))
		x += random.random()*20+.1