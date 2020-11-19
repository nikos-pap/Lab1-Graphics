import math

def print_points(name, point_list):
	print(name,'= [')
	for i in range(0,len(point_list),3):
		print('\t\t{}, {}, {},'.format(point_list[i], point_list[i+1], point_list[i+2]))
	print(']')

radius = 20
sectorCount = 36
stackCount =  18

sectorStep = 2 * math.pi / sectorCount;
stackStep = math.pi / stackCount;
stackAngle = math.pi / 2 #+ stackStep
l = []

for i in range(sectorCount+1):
	stackAngle = (math.pi / 2) - stackStep* i;
	#stackAngle -= stackStep
	xy = radius * math.cos(stackAngle)
	z = radius * math.sin(stackAngle)

	for j in range(sectorCount+1):
		sectorAngle = j * sectorStep
		x = xy * math.cos(sectorAngle);
		y = xy * math.sin(sectorAngle);
		l.extend([x,y,z])
print_points('sphere_points',l)
print(len(l))

indices = []
for i in range(stackCount):
    k1 = i * (sectorCount + 1);
    k2 = k1 + sectorCount + 1;

    for j in range(sectorCount):
        if(i != 0):
            indices.append(k1);
            indices.append(k2);
            indices.append(k1 + 1);

        if(i != (stackCount-1)):
            indices.append(k1 + 1);
            indices.append(k2);
            indices.append(k2 + 1);

        k1 += 1
        k2 += 1

print_points('sphere_indices',indices)
print(len(indices))