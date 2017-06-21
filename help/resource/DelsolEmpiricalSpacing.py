from math import *


def spacing(r,hw):
    _tht = 1.
    
    phi_0 = atan(_tht/r);	#Elevation angle from the heliostats in the first row to the receiver
    tan_phi_0 = _tht/r;
    
    #H2 = Htemp->getHeight()/2.;			
    
    #hw = Htemp->getWidth();
    #hw = 0.05
    
    #The radial separation formula is the same for both round and rectangular heliostats
    
    rsep = 1.1442399/tan_phi_0-1.093519+3.0683558*phi_0-1.1255617*pow(phi_0,2);
    
    #Calculate azimuthal separation
    is_round = False
    if(is_round):
        #Round
        asep = 1.609666+0.29654848*phi_0+0.019137019/(phi_0-0.012341664);
        asep *= 1./(1.-rsep*hw/(_tht*2. * r));	#D1275 is always used.
        rsep *= .5;	#Correct for the half-size convention used
        
    else:
        #Rectangular
        asep = (1.7490871+0.63964099*phi_0+0.028726279/(phi_0-0.049023315));
        asep *= 1./(1.-rsep*hw/(_tht*2. * r));	#D1275 is always used.
        rsep *= .5;	#Correct for the half-size convention used
    
    return [rsep,asep]



import pylab as P

R = P.arange(0.5,10,.05)

P.figure(figsize=(7,5))
#for hw in [0.01, 0.02, 0.05, 0.1, 0.2]:
for hw in [0.2, 0.1, 0.05, 0.02, 0.01]:
    dA = []
    dR = []
    for r in R:
        dr, da = spacing(r,hw)
        dA.append(da)
        dR.append(dr)

    P.plot(R,dA,label="Az. spacing ($W_h$/$H_{tower}$=" + str(hw) + ")")
P.subplots_adjust(left=0.1,right=0.92,bottom=0.12,top=0.95)
P.plot(R,dR,"k-",label="Rad. spacing")
P.legend(loc="upper left",fontsize='medium')
P.xlabel("Radial position ($ H_{tower}$)",fontsize='x-large')
P.ylabel("Spacing factor",fontsize='x-large')
P.savefig("DelsolEmpiricalSpacing.png",dpi=75)
P.show()