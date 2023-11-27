from atomicdata import *
from util import *

# FD bedrock (solidish granite)

compositions = { 
  'SiO2':  { 'Si' : 1, 'O': 2 },
  'Al2O3': { 'Al' : 2, 'O': 3 },
  'K2O':   { 'K'  : 2, 'O': 1 },
  'Na2O':  { 'Na' : 2, 'O': 1 },
  'CaO' :  { 'Ca' : 1, 'O': 1 },
  'FeO':   { 'Fe' : 1, 'O': 1 },
  'Fe2O3': { 'Fe' : 2, 'O': 3 },
  'MgO':   { 'Mg' : 1, 'O': 1 },
  'TiO2':  { 'Ti' : 1, 'O': 2 },
  'P2O5':  { 'P'  : 2, 'O': 5 },
  'MnO':   { 'Mn' : 1, 'O': 1 },
  'water': { 'H'  : 2, 'O': 1 },
}

matrix_density = 2.81 # doc-637 measurement of NOvA granite
water_density = 1.00

# A guess
void_volume_fraction = 0.01

rock_volume_fraction = 1 - void_volume_fraction

# Assume saturation
water_volume_fraction = void_volume_fraction

air_volume_fraction = void_volume_fraction - water_volume_fraction

water_mass_fraction = water_volume_fraction * water_density /(
water_volume_fraction*water_density + rock_volume_fraction*matrix_density)

# Mass of air in the voids is neglected
rock_mass_fraction = 1 - water_mass_fraction

density = (matrix_density * rock_volume_fraction
         + water_density  * water_volume_fraction)

print('<D value="{0:.2f}" unit="g/cm3"/>'.format(density))

fractions = {
  'water': water_mass_fraction,

  # From Wikipedia "Granite": Mineralogy: Chemical composition on 2017-11-27.
  # This is the worldwide average for dry granite.

  'SiO2':  0.7204 * rock_mass_fraction,
  'Al2O3': 0.1442 * rock_mass_fraction,
  'K2O':   0.0412 * rock_mass_fraction,
  'Na2O':  0.0369 * rock_mass_fraction,
  'CaO' :  0.0182 * rock_mass_fraction,
  'FeO':   0.0168 * rock_mass_fraction,
  'Fe2O3': 0.0122 * rock_mass_fraction,
  # Omit to avoid needing magnesium, which isn't in the path_length_list
  # file, and doesn't seem worth adding at ~0.4% with an unremarkable
  # properties: near-average Z/A and neutron capture cross sections.
  #'MgO':   0.0071,

  'TiO2':  0.0030 * rock_mass_fraction,
  'P2O5':  0.0012 * rock_mass_fraction,
  'MnO':   0.0005 * rock_mass_fraction,
}


def tot_weight(compos):
  weight = 0
  for element, number in compos.items():
     weight += number * atomic_weight[element]
  return weight

def tot_fractions():
  tions = 0
  for material, fraction in fractions.items():
     tions += fraction
  return tions

def elem_frac(compos, elem):
  if elem in compos:
    return compos[elem] * atomic_weight[elem] / tot_weight(compos)
  else:
    return 0

def elem_frac_total(elem):
  frac = 0
  for material, compos in compositions.items():
    frac += elem_frac(compos, elem) * fractions[material]
  return frac / tot_fractions()

def fdbedrock_composition():
  return make_composition("fdbedrock", fractions, compositions)
