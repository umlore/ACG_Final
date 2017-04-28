import random
import math

f = open('cityscape','w')

numbuildings = 100
scenemaxheight = 100

#percent of buildings w/ lights in them
lightintensitymax = 20
lightintensitymin = 3
lightdistribution = .3

#light timesteps
lt = 10

model = 'cube2.obj'

blockx = 2
blockz = 7

#building either is lit or not
lightcenters = []
lightdist = []

for x in range(0,numbuildings):
	for z in range(0,numbuildings):
		if (((x % (blockx+1)) and (z % (blockz+1)))):
			x0 = z0 = numbuildings/2
			dist = (math.sqrt((x-x0)**2 + (z-z0)**2))/x0
			dist = dist**.1
			dist *= x0
			dist = x0 - dist
			dist /= x0
			maxheight = scenemaxheight * dist

			height = int((random.random()**3)*maxheight) + 1

			lightintensity = lightintensitymax * dist

			islit = random.random()

			for y in range(0,height):
				if (islit < lightdistribution):
					lightcenters.append((x+.5,y+.5,z+.5))
					lightdist.append(dist)


				f.write("o %s\n" % (model))
				f.write("c %f %f %f\n" % (random.random(),random.random(),random.random()))
				f.write("p %f %f %f\n" % (x,y,z))
				f.write("t 0\nt 1\n")

i = 0
while i < len(lightcenters):
	f.write("l\n")
	for c in range (0,lt):
		f.write("lc %f %f %f\n" % (random.randint(0,1),random.randint(0,1),random.randint(0,1)))
	for p in range (0,lt):
		f.write("lp %f %f %f\n" % (lightcenters[i][0],lightcenters[i][1],lightcenters[i][2]))
	lightintensity = lightintensitymax * lightdist[i]
	if lightintensity < lightintensitymin:
		lightintensity = lightintensitymin
	print lightintensity
	for inte in range (0,lt):
		lint = random.random() * (lightintensity-lightintensitymin)
		lint += lightintensitymin
		lint *= (random.random()) + .5
		f.write("li %f\n" % (lint))
	f.write("lt 0\n")
	for step in range(0,lt):
		f.write("lt %f\n" % (random.random()*.8))

	i+=1