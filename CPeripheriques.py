## MAC : B4:6D:83:FD:AB:60
import numpy as np
import socket
import time

class CPeripheriques():
    def __init__(self,ip):
        self.addrIP=ip
        self.sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connexion(self):
        print("Connexion : ",self.addrIP)
        self.sock.connect((self.addrIP, 80))

    def deconnexion(self):
        print("Deconnexion : ",self.addrIP)
        self.sock.close()

    def SwitchLED(self,etat,position):
        buf = "LED:{:d}:{:d}\r\n".format(etat,position)
        self.sock.send(buf.encode())
 ##       print(buf)
        time.sleep(1)

    def PiloteMoteur(self,sens,duree):
        buf = "Moteur:{:d}:{:.3f}\r\n".format(int(sens),duree)
        self.sock.send(buf.encode())
        duree+=3
        if duree>0:
            time.sleep(duree)
        else:
            time.sleep(-duree)

MonPerif = CPeripheriques("192.168.0.102")
MonPerif.connexion()
##MonPerif.SwitchLED(1,0)
##MonPerif.SwitchLED(1,1)
##time.sleep(5)
##MonPerif.SwitchLED(0,0)
##MonPerif.SwitchLED(0,1)

MonPerif.PiloteMoteur(1,1.246) ## vert rouge et jaune noir : 0 descente 1 montée
time.sleep(1)
######MonPerif.PiloteMoteur(1,2)
######time.sleep(2)
MonPerif.deconnexion()
