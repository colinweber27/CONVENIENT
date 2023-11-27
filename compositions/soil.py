from atomicdata import *
from util import *

compositions = { 
  # Very rough, not specific to Ash River site
  'sand_silica':  { 'Si' : 1, 'O': 2 },
  'silt_silica': { 'Al' : 2, 'O': 3 },
  'silt1': { 'K': 1, 'Al': 1, 'Si': 3, 'O': 8 },
  'silt2': { 'Na': 1, 'Al': 1, 'Si': 3, 'O': 8 },
  'silt3': { 'Ca': 1, 'Al': 2, 'Si': 2, 'O': 8 },
  'clay': { 'Al': 2, 'Si': 2, 'O': 9, 'H': 4 },

  'water': { 'H'  : 2, 'O': 1 },
}

matrix_density = 2.7 # Typical from Wikipedia
water_density = 1.00

# Rough mean from Wikipedia table
void_volume_fraction = 0.40

rock_volume_fraction = 1 - void_volume_fraction

# Lower parts saturated, upper parts not.  Then there's snow on top
# for half the year.  Reasonable guesses lead to full saturation
# being a sensible approximation for the soil plus snow cover (the
# snow geometry is wrong, of course, but at least particles that would
# traverse both snow and soil will be treated better like this).
water_volume_fraction = void_volume_fraction

air_volume_fraction = void_volume_fraction - water_volume_fraction

water_mass_fraction = water_volume_fraction * water_density /(
water_volume_fraction*water_density + rock_volume_fraction*matrix_density)

# Mass of air in the voids is neglected
rock_mass_fraction = 1 - water_mass_fraction

density = (matrix_density * rock_volume_fraction
         + water_density  * water_volume_fraction)

print('<D value="{0:.2f}" unit="g/cm3"/>'.format(density))

clay_fraction = 1/3.
sand_fraction = 1/3.
silt_fraction = 1/3.

fractions = {
  'water': water_mass_fraction,

  'sand_silica':  sand_fraction * rock_mass_fraction,

  'silt_silica':  0.5 * silt_fraction * rock_mass_fraction,
  'silt1': 0.5/3 * silt_fraction * rock_mass_fraction,
  'silt2': 0.5/3 * silt_fraction * rock_mass_fraction,
  'silt3': 0.5/3 * silt_fraction * rock_mass_fraction,

  'clay': clay_fraction * rock_mass_fraction,
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

def soil_composition():
  return make_composition("soil", fractions, compositions)
