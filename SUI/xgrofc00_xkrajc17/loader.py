from os.path import dirname

import numpy as np


class Loader:

    def __init__(self):
        self.weightsAVG = 0
        self.weights = [[0 for a in range(8)] for a in range(8)]
        self.weightsCount = [[0 for a in range(8)] for a in range(8)]
        self.handle = np.loadtxt(dirname(__file__) + '/weights')

    def load_trainer_weights(self):
        count = 0
        rows = 0
        testing = 0
        for row in self.handle:
            if row[0] == -2:
                testing += 1
                if rows != 0:
                    for i in range(8):
                        counter = 0
                        for column in self.handle[rows-8+i]:
                            if column != -1:
                                self.weightsCount[i][counter] += 1
                                self.weights[i][counter] += column
                            counter += 1
                count += 1
            rows += 1

        index1 = 0
        for row in self.weights:
            index2 = 0
            for column in row:
                if self.weightsCount[index1][index2] != 0:
                    self.weights[index1][index2] = column/self.weightsCount[index1][index2]
                index2 += 1
            index1 += 1

        for row in self.weights:
            for column in row:
                self.weightsAVG += column
        self.weightsAVG = self.weightsAVG/64

        return self.weights, self.weightsAVG
