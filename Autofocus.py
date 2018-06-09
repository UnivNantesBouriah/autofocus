import numpy as np
import CCamera
import CPeripheriques

DUREE_DEP=0.5

class CAutofocus():
    def __init__(self):
        self.baryGauche=np.zeros((2,2))
        self.baryDroit=np.zeros((2,2))
        
    def chercheBary(self,camera):    
        image = camera.captureImage()
        bary=camera.barycentre(camera.segmentation(image))
        return bary

    def allumeLEDGauche(self,perif):        
        perif.SwitchLED(1,0) ##1 pour allumer ; 0 pour led gauche

    def eteintLEDGauche(self,perif):
        perif.SwitchLED(0,0) ##0 pour eteindre ; 0 pour led gauche        

    def allumeLEDDroite(self,perif):
        perif.SwitchLED(1,1) ##1 pour allumer ; 1 pour led droite

    def eteintLEDDroite(self,perif):
        perif.SwitchLED(0,1) ##0 pour eteindre ; 1 pour led droite

    def piloteMoteur(self,perif,sens,duree):
        perif.PiloteMoteur(sens,duree)

    def acquisitionBary(self,camera,perif):
        ## Calcul barycentre tache pointeur gauche
        self.allumeLEDGauche(perif)
        baryG=self.chercheBary(camera)
##        print("baryG :",baryG)
        self.eteintLEDGauche(perif)
        ## Calcul barycentre tache pointeur droit
        self.allumeLEDDroite(perif)
        baryD=self.chercheBary(camera)
##        print("baryD :",baryD)
        self.eteintLEDDroite(perif)
        return(baryG,baryD)

    def depart(self,perif):
        perif.PiloteMoteur(0,2.*DUREE_DEP)

    def calculDroite(self,bary):
        a=0
        b=0
        try:
            ## pente a:
            ## a=(y0-y1)/(x0-x1)
            a=(bary[0][1]-bary[1][1])/(bary[0][0]-bary[1][0])
 ##           print("a: ",a)
        except ZeroDivisionError:
            print("valeurs incoherentes, recommencer!")

        try:
            ## ordonnée à l'origine b:
            ## b=y0-a*x0
            b=bary[0][1]-a*bary[0][0]
 ##           print("b: ",b)
        except ZeroDivisionError:
            print("valeurs incoherentes, recommencer!")          

        return(a,b)

    def calibreDepDuree(self):
        ##calibration sur le déplacement en y
        coef=(self.coefDepDuree(self.baryGauche[0][1],self.baryGauche[1][1])+
              self.coefDepDuree(self.baryDroit[0][1],self.baryDroit[1][1]))/2
        return coef
    
    def coefDepDuree(self,valeur0,valeur1):
##        print("x1: {:.3f} x2: {:.3f}".format(x1,x2))
        return DUREE_DEP/(valeur0-valeur1)
    
    def calculDeplacement(self):
        ## calcul des droites passants par les barycentres des taches des pointeurs
        ag,bg=self.calculDroite(self.baryGauche)
        print("ag: {:.3f} bg: {:.3f}".format(ag,bg))
        ad,bd=self.calculDroite(self.baryDroit)
        print("ad: {:.3f} bd: {:.3f}".format(ad,bd))
        ## recherche du point (x,y) de croisement des 2 droites 
        try:
            x=(bd-bg)/(ag-ad)
            y=ag*x+bg
            print("x: {:.3f} y: {:.3f}".format(x,y))
            ## calibartion distance/duree
            coef=self.calibreDepDuree()
            print(coef)
            ## calcul duree : moyenne des écarts entre le point 1 et l'intersection
            ##                  multiplié par le rapport entre la durée du déplacement
            ##                  en z et la distance du déplacement en y
            duree=((self.baryGauche[1][1]-y)+(self.baryDroit[1][1]-y))*coef/2.
            return duree
        except:
            return 0
     

if __name__ == "__main__":
    resX = 20
    resY = 20
#################
##Initialisation
    MonAutofocus = CAutofocus()
    MaCamera = CCamera.CCamera("192.168.0.103",resX,resY)
    MaCamera.connexion()
    MonPerif = CPeripheriques.CPeripheriques("192.168.0.101")
    MonPerif.connexion()
    MonAutofocus.eteintLEDGauche(MonPerif)
    MonAutofocus.eteintLEDDroite(MonPerif)
    duree=DUREE_DEP
    MonAutofocus.depart(MonPerif) ## On redescend en dessous du point focal
#################
    
#################
##
    MonAutofocus.baryGauche[0],MonAutofocus.baryDroit[0]=MonAutofocus.acquisitionBary(MaCamera,MonPerif)
    print("BaryG0 = ",MonAutofocus.baryGauche[0],"BaryD0 = ",MonAutofocus.baryDroit[0])
    MonAutofocus.piloteMoteur(MonPerif,1,duree)      ## Montée de DUREE pour faire nouvelle acquisition
    MonAutofocus.baryGauche[1],MonAutofocus.baryDroit[1]=MonAutofocus.acquisitionBary(MaCamera,MonPerif)
    print("BaryG1 = ",MonAutofocus.baryGauche[1],"BaryD1 = ",MonAutofocus.baryDroit[1])
##    MonAutofocus.baryDroit[0]=[270.023,157.236]
##    MonAutofocus.baryGauche[0]=[214.187,100.950]
##    MonAutofocus.baryDroit[1]=[393.72,143.566]
##    MonAutofocus.baryGauche[1]=[379.761,242.786]
    
    ## Calcul position et temps deplacement
    duree=MonAutofocus.calculDeplacement()
    print("duree: {:.3f}".format(duree))
#################
    if duree!=0:
##        ## Aller au point focus
        MonAutofocus.piloteMoteur(MonPerif,1,duree)
##        ## verifier que c'est le bon point
        MonAutofocus.allumeLEDGauche(MonPerif)
        MonAutofocus.allumeLEDDroite(MonPerif)
##        MaCamera.captureImage()
##        MonAutofocus.eteintLEDGauche(MonPerif)
##        MonAutofocus.eteintLEDDroite(MonPerif)
        ## si non, on refait une itération TODO

    MaCamera.deconnexion()
    MonPerif.deconnexion()


##Connexion :  192.168.0.103
##Connexion :  192.168.0.101
##BaryG0 =  [377. 216.] BaryD0 =  [356. 215.]
##BaryG1 =  [374. 211.] BaryD1 =  [359. 210.]
##ag: 1.667 bg: -412.333
##ad: -1.667 bd: 808.333
##x: 366.200 y: 198.000
##0.1
##duree: 1.250
##Deconnexion :  192.168.0.103
##Deconnexion :  192.168.0.101
