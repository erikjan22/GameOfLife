# ReadPopulationGrowth.py
# This code has been designed to read the output of the Game Of Life program with two species.
# Please place this file in the same directory as the output (PopulationGrowth.txt) which will be read, and run it from there.

import matplotlib.pyplot as plt

Times = []
SpeciesA = []
SpeciesB = []

text_file = open("PopulationGrowth.txt", "r")

firstline = text_file.readline()
temp = firstline.strip().split(",")
nrRows = temp[0]
nrCols = temp[1]

lines = text_file.readlines()

for line in lines:  # taking each line
    temp = line.strip().split(",")
    Times += [int(temp[0])]
    SpeciesA += [int(temp[1])]
    SpeciesB += [int(temp[2])]      # Remove this line if you only have one species.

text_file.close()


plt.plot(Times, SpeciesA, '-r', label='Species A')
plt.plot(Times, SpeciesB, '-b', label='Species B')
plt.xlabel('Time')
plt.ylabel('Number of individuals')
plt.legend()
plt.title('Species growth over time for ' + str(nrRows) + ' x' + str(nrCols) + ' grid')
plt.show()