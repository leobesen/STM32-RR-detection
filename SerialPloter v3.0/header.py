'''
    Libraries
'''
import time
import serial
from serial import SerialException 
import numpy as np  
import os


'''
    Classes
'''
class Serial:
    
    def __init__(self):
        self.COM = '/dev/ttyACM0'        # COM port connected to the device
        self.BaudRate = 9600           # Baud Rate of the communication
        self.portAvailable = False
    
    def initSerial(self):
        try:
            os.system("sudo chmod 666 " + self.COM)          # Change permission to access COM port
            self.portAvailable = True
        except OSError:
            self.portAvailable = False
            print("Cannot change permission of port {0}".format(self.COM))
        try:
            if(self.portAvailable):
                self.stmSerial = serial.Serial(self.COM,  self.BaudRate)    # Creating serial object
        except SerialException:
            self.portAvailable = False
            print("Cannot open port {0}".format(self.COM))

    def readSerial(self):
        while (self.stmSerial.inWaiting()==0):
            pass 
        self.dataRead = self.stmSerial.read_until().decode()
        #self.dataRead = self.stmSerial.readline().decode()
        self.dataArray = self.dataRead.split(",")
        # print(self.dataArray)
        return self.dataArray

class PPG:
    def __init__(self,enable):
        self.enable = enable
        self.IR_LED = []
        self.RED_LED = []

class PPG_DC_Remove:
    def __init__(self,enable):
        self.enable = enable
        self.IR_DC_Removed = []
        self.IR_baseLineWander = []
        self.RED_DC_Removed = []
        self.RED_baseLineWander = []
        self.RED_bw = 0
        self.IR_bw = 0

class PPG_Impulse_Remove:
    def __init__(self,enable):
        self.enable = enable
        self.N = 8
        self.IR_Imp_Removed = []
        self.RED_Imp_Removed = []
        self.IR_temp = []
        self.RED_temp = []

class PPG_Peak_Finder:
    def __init__(self):
        self.diff_pos_neg_peak = []

class PPG_EEMD:
    def __init__(self,enable):
        self.enable = enable
        self.cnt_emd = 0
        self.imfs_number = 4
        self.IR_EEMD = []
        self.IR_HIST = []
        self.ACC_Z_HIST = []

class ACC:
    def __init__(self,enable):
        self.enable = enable
        self.X = []
        self.Y = []
        self.Z = []

class ACC_Impulse_Remove:
    def __init__(self,enable):
        self.enable = enable
        self.N = 16
        self.X_Imp_Removed = []
        self.Y_Imp_Removed = []
        self.Z_Imp_Removed = []
        self.X_temp = []
        self.Y_temp = []
        self.Z_temp = []

class FSR:
    def __init__(self,enable):
        self.enable = enable
        self.fsr = []

class Counters:
    def __init__(self):
        self.cnt = 0
        self.cnt_peak = 0

