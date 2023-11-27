from atomicdata import *
from util import *

# This defines ND rock as it is relevant for beam events.
# It does not attempt to simulate geological layers which may be
# relevant for cosmics.
#
# There is a fair amount of guesswork and heavy use of Wikipedia here,
# but the composition should be reasonable, and a lot closer to reality
# than "standard rock".  See also nova-doc-25794.

compositions = { 
  'SiO2':  { 'Si' : 1, 'O': 2 }, # Quartz
  'CaCO3': { 'Ca' : 1, 'O': 3, 'C': 1 },

  'kaolinite': { 'Al': 2, 'Si': 2, 'O': 5+4, 'H': 4 },

  # Omitted to avoid adding an element: 'Mg': 1,
  'montmorillonite': { 'Na': 0.33/2, 'Ca': 0.33/2, 'Al': 1,
                       'Si': 4, 'O': 10 + 2 + 2, 'H': 2 + 4 },

  'illite': { 'K': 0.5, 'Al': 0.67 + 2,  'Fe': 0.67,
              'Si': 2, 'O': 10+2+1, 'H': 2+2 }, # Omitted 'Mg': 0.67,

  'water': { 'H'  : 2, 'O': 1 },

  # Just elements, for checking old composition + water
  'oxygen':   { 'O'  : 1 },
  'silicon':  { 'Si' : 1 },
  'calcium':  { 'Ca' : 1 },
  'sodium':   { 'Na' : 1 },
  'iron':     { 'Fe' : 1 },
  'aluminum': { 'Al' : 1 },
  'potassium':{ 'K'  : 1 },
}

# From MINOS-doc-2777

matrix_density = 2.84
water_density = 1.00

# Measurement for "Lower portion (Shalely) of
# Scales formation (Maquoketa)"
void_volume_fraction = 0.22
rock_volume_fraction = 1 - void_volume_fraction

# We have been draining water out of this rock for 20 years
# This is a guess at the remaning water content.
# We had hoped to find better data in the sump logs, but weren't able to.
water_volume_fraction = void_volume_fraction * 0.5

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

  # Here's the old composition, for checking how much difference the water
  # alone makes
  #'oxygen':    0.437,
  #'silicon':   0.257,
  #'sodium':    0.222,
  #'iron':      0.020,
  #'aluminum':  0.049,
  #'potassium': 0.015,

  # Some not-totally-wild guesses using Wikipedia "Shale"
  
  # Clay minerals
  'montmorillonite': 0.3 * rock_mass_fraction,
  'kaolinite':       0.2 * rock_mass_fraction,
  'illite':          0.2 * rock_mass_fraction,

  # Quartz and calcite.  Quartz is mentioned more often.
  'SiO2':  0.2 * rock_mass_fraction,
  'CaCO3': 0.1 * rock_mass_fraction,
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

def ndrock_composition():
  return make_composition("ndrock", fractions, compositions)
