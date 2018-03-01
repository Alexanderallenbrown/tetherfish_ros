#!/usr/bin/env python

import roslib; roslib.load_manifest('tetherfish_ros')
import rospy
from fish_target.msg import TargetMsg
import tf
import serial
import sys
import datetime
import rospkg

class ArduinoTarget():


   def __init__(self):
      self.port = rospy.get_param('~port','/dev/ttyACM0')
      self.baud = rospy.get_param('~baud',115200)
      self.ser = serial.Serial(self.port, self.baud,timeout=1) #this initializes the serial object
      self.pub= rospy.Publisher("tetherfish/robotinfo",TetherFishMsg)
      self.num_shots = 0
      self.last_shots = 0
      #main loop runs on a timer
      rospy.Timer(rospy.Duration(.005),self.loop,oneshot=False) #timer callback (math) allows filter to run at constant time
      #subscribers (inputs)
      #construct the file name for our text output file
      rospack = rospkg.RosPack()
      # get the file path for rospy_tutorials
      self.package_path=rospack.get_path('tetherfish')

   def loop(self,event):
      line = self.ser.readline()
      #print line
      linestrip = line.strip('\r\n')
      linesplit = line.split()
      if len(linesplit)>=6:
        #print shotslast, arduinonumshots,shotslast==(arduinonumshots+1)
        
        try:

            msg = TetherFishMsg()
            msg.header.stamp = rospy.Time.now()
            msg.robotpos = float(linesplit[0])
            msg.robotangle = float(linesplit[1])
            msg.arduino_time = float(linesplit[2])
            msg.tailfreq= float(linesplit[3])
            msg.tailamp = float(linesplit[4])
            msg.tailangle = float(linesplit[5])
            self.pub.publish(msg)
            

        except:
            print "OOPS! BAD LINE"
            #ef.write("problem  with serial line"+"\r\n")

      
#main function
def main(args):
  rospy.init_node('tetherfish_node', anonymous=True)
  encreader = ArduinoTarget()
  
  try:
    rospy.spin()
  except KeyboardInterrupt:
    print "Shutting down"

if __name__ == '__main__':
    main(sys.argv) 