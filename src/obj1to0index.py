with open('sphere_20480.obj','r') as fin:
	with open('sphere','w') as fout:
		for line in fin:
			line = line.replace('\n','')
			args = line.split(" ")
			if args[0] == 'f':
				args[1] = int(args[1]) - 1
				args[2] = int(args[2]) - 1
				args[3] = int(args[3]) - 1
				fout.write("f %d %d %d\n" % (args[1],args[2],args[3]))
			else:
				fout.write("%s\n" % (line))