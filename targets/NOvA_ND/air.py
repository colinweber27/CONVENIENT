from atomicdata import *
from util import *

compositions = { 
  # Don't include argon (neon, etc.) because it isn't otherwise in the model
  'nitrogen': { 'N': 2 },
  'oxygen':   { 'O': 2 },
  'water':    { 'H': 2, 'O': 1 },
  'carbon dioxide': { 'C': 1, 'O': 2 },
}

fractions = {
  # Take the mole fractions from Wikipedia "Atmospheric chemistry" and convert
  # to number proportional to mass fractions
  'nitrogen': 0.78084 * 2 * 14.007,
  'oxygen':   0.20946 * 2 * 15.999,

  # I think this is the right ballpark for our moderate humidity
  'water':    0.007 * (2 * 1.008 + 15.999),

  'carbon dioxide': 400e-6 * (12.011 + 2 * 15.999),
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

def air_composition():
  return make_composition("air", fractions, compositions)
