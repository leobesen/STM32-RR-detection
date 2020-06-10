import matplotlib.pyplot as plt 

class DataPlt:
    def __init__(self):
        plt.ion()
        plt.figure(figsize=(20,5))
        self.plotCounter = 0
        self.plotCounterMax = 1
        self.numPoints_FSR = 250
        self.numPoints_ACC = 500
        self.numPoints_PPG = 3000
        self.numPlots = 1
        self.var = {}
        self.var_label = {}
        self.var_style = {}

    def makeFig(self):   
        for i in range(self.numPlots):
            plt.subplot(self.numPlots,1,i+1)
            plt.plot(self.var[str(i)], self.var_style[str(i)], label=self.var_label[str(i)])                                  
            plt.grid(True)                             
            plt.legend(loc='upper left')