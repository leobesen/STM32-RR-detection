import matplotlib.pyplot as plt 

class DataPlt:
    def __init__(self):
        plt.ion()
        plt.figure(figsize=(20,5))
        self.plotCounter = 0
        self.plotCounterMax = 1
        self.numPoints_FSR_plot = 100    # 5 min = 300 s - 10 x 300 = 3000
        self.numPoints_ACC_plot = 200    # 20 x 300 = 6000
        self.numPoints_PPG_plot = 1000   # 100 x 300 = 30000
        self.numPoints_FSR_save = 3000    # 5 min = 300 s - 10 x 300 = 3000
        self.numPoints_ACC_save = 6000    # 20 x 300 = 6000
        self.numPoints_PPG_save = 30000   # 100 x 300 = 30000
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