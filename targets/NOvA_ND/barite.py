from atomicdata import *
from util import *

# FD overburden

compositions = { 
  'BaSO4': { 'Ba' : 1, 'S' : 1, 'O': 4 },
  'water': { 'H'  : 2, 'O': 1 },
}

# Of perfect barite crystals, I think.  Maybe higher than reasonable for
# the mean of real rocks?
matrix_density = 4.48

water_density = 1.00

# From doc-6092
void_volume_fraction = 0.269

rock_volume_fraction = 1 - void_volume_fraction

# Assume lower parts saturated, upper parts not, and we want the average
water_volume_fraction = void_volume_fraction * 0.75

air_volume_fraction = void_volume_fraction - water_volume_fraction

water_mass_fraction = water_volume_fraction * water_density /(
water_volume_fraction*water_density + rock_volume_fraction*matrix_density)

# Mass of air in the voids is neglected
rock_mass_fraction = 1 - water_mass_fraction

density = (matrix_density * rock_volume_fraction
         + water_density  * water_volume_fraction)

print('<D value="{0:.2f}" unit="g/cm3"/>'.format(density))


fractions = {
  # Assume 30% void in the rock, filled by water.
  'water': water_mass_fraction,
  'BaSO4': rock_mass_fraction,
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

def barite_composition():
  return make_composition("barite", fractions, compositions)
