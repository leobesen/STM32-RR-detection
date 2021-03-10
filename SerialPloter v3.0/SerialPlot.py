'''
    Modules
'''
from header import *
from DataPlot import *
from DSP_Functions import Functions
from drawnow import drawnow
import pandas as pd 

# classes
ppg_plot = PPG(True)
ppg_save = PPG(True)
acc_1_plot = ACC(True)
acc_2_plot = ACC(True)
acc_3_plot = ACC(True)
acc_4_plot = ACC(True)
acc_1_save = ACC(True)
acc_2_save = ACC(True)
acc_3_save = ACC(True)
acc_4_save = ACC(True)
fsr_plot = FSR(True)
fsr_save = FSR(True)
counter = Counters()
plot = DataPlt()
func = Functions()
# Open serial port  
s = Serial()
s.initSerial()

ppg_n_points = 0

save_data = True
file_number = 0
acc_scale = 4.0


# Test port
if(s.portAvailable):
    # (CTRL + C) to end program
    try:
        while True:
            #read data
            data = s.readSerial()
            #data = np.array(data)
            # Force sensitive resistor (FSR)
            try:
                if(int(data[0])==28):
                    if(int(data[1])==1):
                        # print("FSR/r/n");
                        if(fsr_plot.enable):
                            for i in range(3,13,1):
                                if(len(fsr_plot.fsr) >= plot.numPoints_FSR_plot):
                                    fsr_plot.fsr.pop(0)
                                fsr_plot.fsr.append(int(data[i])) # 2 - 11
                        if(fsr_save.enable):
                            for i in range(3,13,1):
                                if(len(fsr_save.fsr) >= plot.numPoints_FSR_save):
                                    fsr_save.fsr.pop(0)
                                fsr_save.fsr.append(int(data[i])) # 2 - 11

                    if(int(data[1])==2):
                        if(acc_1_plot.enable):
                            for i in range(3,18,3):
                                if(len(acc_1_plot.X) >= plot.numPoints_ACC_plot):
                                    acc_1_plot.X.pop(0)
                                    acc_1_plot.Y.pop(0)
                                    acc_1_plot.Z.pop(0)
                                acc_1_plot.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_1_plot.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_1_plot.Z.append(int(data[i+2]))#*acc_scale/65535.0)
                        if(acc_1_save.enable):
                            for i in range(3,18,3):
                                counter.acc_samples+=1
                                if(len(acc_1_save.X) >= plot.numPoints_ACC_save):
                                    acc_1_save.X.pop(0)
                                    acc_1_save.Y.pop(0)
                                    acc_1_save.Z.pop(0)
                                acc_1_save.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_1_save.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_1_save.Z.append(int(data[i+2]))#*acc_scale/65535.0)
                        if(acc_2_plot.enable):
                            for i in range(18,33,3):
                                if(len(acc_2_plot.X) >= plot.numPoints_ACC_plot):
                                    acc_2_plot.X.pop(0)
                                    acc_2_plot.Y.pop(0)
                                    acc_2_plot.Z.pop(0)
                                acc_2_plot.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_2_plot.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_2_plot.Z.append(int(data[i+2]))#*acc_scale/65535.0) 
                        if(acc_2_save.enable):
                            for i in range(18,33,3):
                                if(len(acc_2_save.X) >= plot.numPoints_ACC_save):
                                    acc_2_save.X.pop(0)
                                    acc_2_save.Y.pop(0)
                                    acc_2_save.Z.pop(0)
                                acc_2_save.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_2_save.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_2_save.Z.append(int(data[i+2]))#*acc_scale/65535.0)
                        if(acc_3_plot.enable):        
                            for i in range(33,48,3):
                                if(len(acc_3_plot.X) >= plot.numPoints_ACC_plot):
                                    acc_3_plot.X.pop(0)
                                    acc_3_plot.Y.pop(0)
                                    acc_3_plot.Z.pop(0)
                                acc_3_plot.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_3_plot.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_3_plot.Z.append(int(data[i+2]))#*acc_scale/65535.0) 
                        if(acc_3_save.enable):        
                            for i in range(33,48,3):
                                if(len(acc_3_save.X) >= plot.numPoints_ACC_save):
                                    acc_3_save.X.pop(0)
                                    acc_3_save.Y.pop(0)
                                    acc_3_save.Z.pop(0)
                                acc_3_save.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_3_save.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_3_save.Z.append(int(data[i+2]))#*acc_scale/65535.0)
                        if(acc_4_plot.enable):
                            for i in range(48,63,3):
                                if(len(acc_4_plot.X) >= plot.numPoints_ACC_plot):
                                    acc_4_plot.X.pop(0)
                                    acc_4_plot.Y.pop(0)
                                    acc_4_plot.Z.pop(0)
                                acc_4_plot.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_4_plot.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_4_plot.Z.append(int(data[i+2]))#*acc_scale/65535.0) 
                        if(acc_4_save.enable):
                            for i in range(48,63,3):
                                if(len(acc_4_save.X) >= plot.numPoints_ACC_save):
                                    acc_4_save.X.pop(0)
                                    acc_4_save.Y.pop(0)
                                    acc_4_save.Z.pop(0)
                                acc_4_save.X.append(int(data[i]))#*acc_scale/65535.0)
                                acc_4_save.Y.append(int(data[i+1]))#*acc_scale/65535.0) 
                                acc_4_save.Z.append(int(data[i+2]))#*acc_scale/65535.0)
                    
                    if(int(data[1])==3):
                        if(ppg_plot.enable):
                            for i in range(3,28,1):
                                if(len(ppg_plot.RED_LED) >= plot.numPoints_PPG_plot):
                                    ppg_plot.RED_LED.pop(0)
                                ppg_plot.RED_LED.append(int(data[i]))
                            for i in range(28,53,1):
                                if(len(ppg_plot.IR_LED) >= plot.numPoints_PPG_plot):
                                    ppg_plot.IR_LED.pop(0)
                                ppg_plot.IR_LED.append(int(data[i]))
                        if(ppg_save.enable):
                            for i in range(3,28,1):
                                if(len(ppg_save.RED_LED) >= plot.numPoints_PPG_save):
                                    ppg_save.RED_LED.pop(0)
                                ppg_save.RED_LED.append(int(data[i]))
                            for i in range(28,53,1):
                                if(len(ppg_save.IR_LED) >= plot.numPoints_PPG_save):
                                    ppg_save.IR_LED.pop(0)
                                ppg_save.IR_LED.append(int(data[i]))

            except:
                print("error getting data")

            
            #Choose data to plot
            
            if(len(acc_1_save.X) < plot.numPoints_ACC_save):
                ''' PPG '''
                # plot.numPlots = 2
                # plot.var["0"] = ppg.IR_LED
                # plot.var["1"] = ppg.RED_LED
                # plot.var_label["0"] = 'ppg.IR_LED'
                # plot.var_label["1"] = 'ppg.RED_LED'
                
                ''' FSR '''
                # plot.numPlots = 1
                # plot.var["0"] = fsr.fsr
                # plot.var_label["0"] = 'fsr.fsr'
                
                # ''' ACC '''
                # plot.numPlots = 1
                # plot.var["0"] = acc_1_plot.Z
                # plot.var["1"] = acc_2_plot.Z
                # plot.var["2"] = acc_3_plot.Z
                # plot.var["3"] = acc_4_plot.Z
                # plot.var_label["0"] = 'acc_1.Y'
                # plot.var_label["1"] = 'acc_2.Y'
                # plot.var_label["2"] = 'acc_3.Y'
                # plot.var_label["3"] = 'acc_4.Y'

                ''' Compose '''
                plot.numPlots = 3
                plot.var["0"] = ppg_plot.RED_LED
                plot.var["1"] = acc_1_plot.Z
                plot.var["2"] = fsr_plot.fsr

                plot.var_label["0"] = 'PPG RED'
                plot.var_label["1"] = 'ACC Z'     
                plot.var_label["2"] = 'FSR'



                plot.var_style["0"] = 'ro-'
                plot.var_style["1"] = 'go-'
                plot.var_style["2"] = 'bo-'
                plot.var_style["3"] = 'go-'
                plot.var_style["4"] = 'ko-'
                plot.var_style["5"] = 'co-'

                #Plot Control

                if plot.plotCounter >= 8:
                    drawnow(plot.makeFig)
                    plt.pause(.0000001)
                    plot.plotCounter = 0
                else:
                    plot.plotCounter+=1

            #Control counters
            counter.cnt+=1
            #print(counter.cnt)
            #print(fsr.fsr[0:5])

            # if (counter.cnt >= 256):
            #     if (counter.cnt%40 == 0):
            #         func.fft(ppg_DCR.IR_DC_Removed[0:256], 256)

            #save data
            if(save_data):
                if(counter.acc_samples == plot.numPoints_ACC_save):
                    if file_number>=1:
                        dict_FSR = {}
                        dict_ACC = {}
                        dict_PPG = {}
                        dict_FSR['FSR'] = fsr_save.fsr
                        dict_ACC['ACC_1.X'] = acc_1_save.X
                        dict_ACC['ACC_1.Y'] = acc_1_save.Y
                        dict_ACC['ACC_1.Z'] = acc_1_save.Z
                        dict_ACC['ACC_2.X'] = acc_2_save.X
                        dict_ACC['ACC_2.Y'] = acc_2_save.Y
                        dict_ACC['ACC_2.Z'] = acc_2_save.Z
                        dict_ACC['ACC_3.X'] = acc_3_save.X
                        dict_ACC['ACC_3.Y'] = acc_3_save.Y
                        dict_ACC['ACC_3.Z'] = acc_3_save.Z
                        dict_ACC['ACC_4.X'] = acc_4_save.X
                        dict_ACC['ACC_4.Y'] = acc_4_save.Y
                        dict_ACC['ACC_4.Z'] = acc_4_save.Z
                        dict_PPG['PPG.IR'] = ppg_save.IR_LED
                        dict_PPG['PPG.RED'] = ppg_save.RED_LED
                        
                        dt = pd.DataFrame(list(dict_FSR['FSR']),columns = ['FSR'])
                        dt.to_csv('data/fsr_data_' + str(file_number) + '.csv',index=False,header=False)
                        
                        dt = pd.DataFrame(list(zip(dict_ACC['ACC_1.X'],dict_ACC['ACC_1.Y'],dict_ACC['ACC_1.Z'])),columns = ['ACC_1_X','ACC_1_Y','ACC_1_Z'])
                        dt.to_csv('data/acc_1_data_' + str(file_number) + '.csv',index=False,header=False)
                        dt = pd.DataFrame(list(zip(dict_ACC['ACC_2.X'],dict_ACC['ACC_2.Y'],dict_ACC['ACC_2.Z'])),columns = ['ACC_2_X','ACC_2_Y','ACC_2_Z'])
                        dt.to_csv('data/acc_2_data_' + str(file_number) + '.csv',index=False,header=False)
                        dt = pd.DataFrame(list(zip(dict_ACC['ACC_3.X'],dict_ACC['ACC_3.Y'],dict_ACC['ACC_3.Z'])),columns = ['ACC_3_X','ACC_3_Y','ACC_3_Z'])
                        dt.to_csv('data/acc_3_data_' + str(file_number) + '.csv',index=False,header=False)
                        dt = pd.DataFrame(list(zip(dict_ACC['ACC_4.X'],dict_ACC['ACC_4.Y'],dict_ACC['ACC_4.Z'])),columns = ['ACC_4_X','ACC_4_Y','ACC_4_Z'])
                        dt.to_csv('data/acc_4_data_' + str(file_number) + '.csv',index=False,header=False)
                        
                        dt = pd.DataFrame(list(zip(dict_PPG['PPG.IR'],dict_PPG['PPG.RED'])),columns = ['PPG_IR','PPG_RED'])
                        dt.to_csv('data/ppg_data_' + str(file_number) + '.csv',index=False,header=False) 
                        print('File ' + str(file_number) + 'ready!')

                    counter.acc_samples = 0
                    file_number+=1

                    
    except KeyboardInterrupt:
        print ("Program closed by Keyboard interrupt") 