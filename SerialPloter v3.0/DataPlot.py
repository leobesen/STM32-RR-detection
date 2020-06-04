import matplotlib.pyplot as plt 

class DataPlt:
    def __init__(self):
        plt.ion()
        plt.figure(figsize=(20,5))
        self.plotCounter = 0
        self.plotCounterMax = 10
        self.numPointToPlot = 256
        self.numPlots = 1
        self.var1 = []
        self.var2 = []
        self.var1_label = ""
        self.var2_label = ""
        self.var_1_style = 'bo-'
        self.var_2_style = 'yo-'

    def makeFig_oneVar(self):
        plt.plot(self.var1, self.var_1_style, label=self.var1_label)
        plt.title(self.var1_label)                              
        plt.grid(True)                               
        plt.ylabel(self.var1_label)                               
        plt.legend(loc='upper left')                      

    def makeFig_twoVar(self):   
        plt.subplot(2,1,1)
        plt.plot(self.var1, self.var_1_style, label=self.var1_label)  
        plt.title(self.var1_label)                                
        plt.grid(True)
        plt.ylabel(self.var1_label)                              
        plt.legend(loc='upper left')

        plt.subplot(2,1,2) 
        plt.plot(self.var2, self.var_2_style, label=self.var2_label)      
        plt.title(self.var2_label)                              
        plt.grid(True) 
        plt.ylabel(self.var2_label)                              
        plt.legend(loc='upper left')