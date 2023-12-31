from math import *

# gperiodic -D | cut -d, -f2,1 | sed s/,/\ / | awk '{print $2, $1}' | \
#                sed -e s/^/\'/ -e "s/ /\':\'/" -e s/$/\',/
atomic_names = {
'H':'Hydrogen',
'He':'Helium',
'Li':'Lithium',
'Be':'Beryllium',
'B':'Boron',
'C':'Carbon',
'N':'Nitrogen',
'O':'Oxygen',
'F':'Fluorine',
'Ne':'Neon',
'Na':'Sodium',
'Mg':'Magnesium',
'Al':'Aluminum',
'Si':'Silicon',
'P':'Phosphorus',
'S':'Sulfur',
'Cl':'Chlorine',
'Ar':'Argon',
'K':'Potassium',
'Ca':'Calcium',
'Sc':'Scandium',
'Ti':'Titanium',
'V':'Vanadium',
'Cr':'Chromium',
'Mn':'Manganese',
'Fe':'Iron',
'Co':'Cobalt',
'Ni':'Nickel',
'Cu':'Copper',
'Zn':'Zinc',
'Ga':'Gallium',
'Ge':'Germanium',
'As':'Arsenic',
'Se':'Selenium',
'Br':'Bromine',
'Kr':'Krypton',
'Rb':'Rubidium',
'Sr':'Strontium',
'Y':'Yttrium',
'Zr':'Zirconium',
'Nb':'Niobium',
'Mo':'Molybdenum',
'Tc':'Technetium',
'Ru':'Ruthenium',
'Rh':'Rhodium',
'Pd':'Palladium',
'Ag':'Silver',
'Cd':'Cadmium',
'In':'Indium',
'Sn':'Tin',
'Sb':'Antimony',
'Te':'Tellurium',
'I':'Iodine',
'Xe':'Xenon',
'Cs':'Cesium',
'Ba':'Barium',
'La':'Lanthanum',
'Ce':'Cerium',
'Pr':'Praseodymium',
'Nd':'Neodymium',
'Pm':'Promethium',
'Sm':'Samarium',
'Eu':'Europium',
'Gd':'Gadolinium',
'Tb':'Terbium',
'Dy':'Dysprosium',
'Ho':'Holmium',
'Er':'Erbium',
'Tm':'Thulium',
'Yb':'Ytterbium',
'Lu':'Lutetium',
'Hf':'Hafnium',
'Ta':'Tantalum',
'W':'Tungsten',
'Re':'Rhenium',
'Os':'Osmium',
'Ir':'Iridium',
'Pt':'Platinum',
'Au':'Gold',
'Hg':'Mercury',
'Tl':'Thallium',
'Pb':'Lead',
'Bi':'Bismuth',
'Po':'Polonium',
'At':'Astatine',
'Rn':'Radon',
'Fr':'Francium',
'Ra':'Radium',
'Ac':'Actinium',
'Th':'Thorium',
'Pa':'Protactinium',
'U':'Uranium',
'Np':'Neptunium',
'Pu':'Plutonium',
'Am':'Americium',
'Cm':'Curium',
'Bk':'Berkelium',
'Cf':'Californium',
'Es':'Einsteinium',
'Fm':'Fermium',
'Md':'Mendelevium',
'No':'Nobelium',
'Lr':'Lawrencium'
}


# gperiodic -D | cut -d, -f2,4 | sed -e s/^/\'/ -e "s/,/\':/" -e s/$/,/
atomic_weight = { 
'H':1.00794,
'He':4.002602,
'Li':6.941,
'Be':9.01218,
'B':10.811,
'C':12.011,
'N':14.00674,
'O':15.9994,
'F':18.998403,
'Ne':20.1797,
'Na':22.989768,
'Mg':24.305,
'Al':26.981539,
'Si':28.0855,
'P':30.973762,
'S':32.066,
'Cl':35.4527,
'Ar':39.948,
'K':39.0983,
'Ca':40.078,
'Sc':44.95591,
'Ti':47.867, # Wikipedia for another digit
'V':50.9415,
'Cr':51.9961,
'Mn':54.93805,
'Fe':55.847,
'Co':58.9332,
'Ni':58.6934,
'Cu':63.546,
'Zn':65.39,
'Ga':69.723,
'Ge':72.61,
'As':74.92159,
'Se':78.96,
'Br':79.904,
'Kr':83.8,
'Rb':85.4678,
'Sr':87.62,
'Y':88.90585,
'Zr':91.224,
'Nb':92.90638,
'Mo':95.94,
'Tc':97.9072,
'Ru':101.07,
'Rh':102.9055,
'Pd':106.42,
'Ag':107.8682,
'Cd':112.411,
'In':114.818,
'Sn':118.71,
'Sb':121.760,
'Te':127.6,
'I':126.90447,
'Xe':131.29,
'Cs':132.90543,
'Ba':137.327,
'La':138.9055,
'Ce':140.115,
'Pr':140.90765,
'Nd':144.24,
'Pm':144.9127,
'Sm':150.36,
'Eu':151.965,
'Gd':157.25,
'Tb':158.92534,
'Dy':162.50,
'Ho':164.93032,
'Er':167.26,
'Tm':168.93421,
'Yb':173.04,
'Lu':174.967,
'Hf':178.49,
'Ta':180.9479,
'W':183.84,
'Re':186.207,
'Os':190.23,
'Ir':192.22,
'Pt':195.08,
'Au':196.96654,
'Hg':200.59,
'Tl':204.3833,
'Pb':207.2,
'Bi':208.98037,
'Po':208.9824,
'At':209.9871,
'Rn':222.0176,
'Fr':223.0197,
'Ra':226.0254,
'Ac':227.0278,
'Th':232.0381,
'Pa':231.03588,
'U':238.0289,
'Np':237.048,
'Pu':244.0642,
'Am':243.0614,
'Cm':247.0703,
'Bk':247.0703,
'Cf':251.0796,
'Es':252.083,
'Fm':257.0951,
'Md':258.1,
'No':259.1009,
'Lr':262.11
}

# gperiodic -D | cut -d, -f2,3 | sed -e s/^/\'/ -e "s/,/\':/" -e s/$/,/
atomic_number = {
'H':  1, 'He': 2, 'Li': 3, 'Be': 4, 'B' : 5, 'C' : 6, 'N' : 7, 'O' : 8,
'F' : 9, 'Ne':10, 'Na':11, 'Mg':12, 'Al':13, 'Si':14, 'P' :15, 'S' :16,
'Cl':17, 'Ar':18, 'K' :19, 'Ca':20, 'Sc':21, 'Ti':22, 'V' :23, 'Cr':24,
'Mn':25, 'Fe':26, 'Co':27, 'Ni':28, 'Cu':29, 'Zn':30, 'Ga':31, 'Ge':32,
'As':33, 'Se':34, 'Br':35, 'Kr':36, 'Rb':37, 'Sr':38, 'Y' :39, 'Zr':40,
'Nb':41, 'Mo':42, 'Tc':43, 'Ru':44, 'Rh':45, 'Pd':46, 'Ag':47, 'Cd':48,
'In':49, 'Sn':50, 'Sb':51, 'Te':52, 'I' :53, 'Xe':54, 'Cs':55, 'Ba':56,
'La':57, 'Ce':58, 'Pr':59, 'Nd':60, 'Pm':61, 'Sm':62, 'Eu':63, 'Gd':64,
'Tb':65, 'Dy':66, 'Ho':67, 'Er':68, 'Tm':69, 'Yb':70, 'Lu':71, 'Hf':72,
'Ta':73, 'W' :74, 'Re':75, 'Os':76, 'Ir':77, 'Pt':78, 'Au':79, 'Hg':80,
'Tl':81, 'Pb':82, 'Bi':83, 'Po':84, 'At':85, 'Rn':86, 'Fr':87, 'Ra':88,
'Ac':89, 'Th':90, 'Pa':91, 'U' :92, 'Np':93, 'Pu':94, 'Am':95, 'Cm':96,
'Bk':97, 'Cf':98, 'Es':99, 'Fm':100,'Md':101,'No':102,'Lr':103,
}

cl35molefrac = 0.7576
cl35massamu = 34.96885268
cl37massamu = 36.96590259
cl35massfrac = cl35molefrac*cl35massamu/ \
              (cl35molefrac*cl35massamu + (1-cl35molefrac)*cl37massamu)
cl37massfrac = 1 - cl35massfrac

# PDG table 32.2
lrad = {
'H':  5.31,
'He': 4.79,
'Li': 4.74,
'Be': 4.71,
}
for elem, z in atomic_number.items():
  if z > 4:
    lrad[elem] = log(181.15*pow(z,-1./3.))

lradprime = {
'H':  6.144,
'He': 5.621,
'Li': 5.805,
'Be': 5.924,
}
for elem, z in atomic_number.items():
  if z > 4:
    lradprime[elem] = log(1194.*pow(z,-2./3.))

# PDG eq 32.26
inverseradlength = {
  'H': 0,
}

def fz3226(z):
  a = z/137.036
  return a*a*(1/(1+a*a) + 0.20206 - 0.0369*a*a
   + 0.0083*pow(a, 4) - 0.002*pow(a, 6))

for elem, z in atomic_number.items():
  inverseradlength[elem] = 1/716.408/atomic_weight[elem] * (
    z*z*(lrad[elem] - fz3226(z)) + z*lradprime[elem])


# Groom tables: LBNL-44742.  MeV cm^2/g
# Liquid when there is a choice
stoppingpower = {
'H':  4.034,
'He': 1.936,
'Li': 1.639,
'Be': 1.595,
'B':  1.623,
'C':  1.753, # graphite
'N':  1.825,
'N':  1.813,
'O':  1.801,
'O':  1.788,
'F':  1.676,
'F':  1.634,
'Ne': 1.724,
'Ne': 1.695,
'Na': 1.639,
'Mg': 1.674,
'Al': 1.615,
'Si': 1.664,
'P':  1.613,
'S':  1.652,
'Cl': 1.608,
'Ar': 1.508,
'K':  1.623,
'Ca': 1.655,
'Sc': 1.522,
'Ti': 1.477,
'V':  1.436,
'Cr': 1.456,
'Mn': 1.428,
'Fe': 1.451,
'Co': 1.419,
'Ni': 1.468,
'Cu': 1.403,
'Zn': 1.411,
'Ga': 1.379,
'Ge': 1.370,
'As': 1.370,
'Se': 1.343,
'Br': 1.385,
'Kr': 1.357,
'Rb': 1.356,
'Sr': 1.353,
'Zr': 1.349,
'Nb': 1.343,
'Mo': 1.330,
'Pd': 1.289,
'Ag': 1.299,
'Cd': 1.277,
'In': 1.278,
'Sn': 1.263,
'Sb': 1.259,
'I':  1.263,
'Xe': 1.255,
'Cs': 1.254,
'Ba': 1.231,
'Ce': 1.234,
'Dy': 1.175,
'Ta': 1.149,
'W':  1.145,
'Pt': 1.128,
'Au': 1.134,
'Hg': 1.130,
'Pb': 1.122,
'Bi': 1.128,
'Th': 1.098,
'U':  1.081,
'Pu': 1.071,
}
