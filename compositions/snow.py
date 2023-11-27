from atomicdata import *
from util import *

# Snow is just water, but to make it easy to print out its Z/A, etc.
# in a consistent way, go through the mechanism of generating it.

# *Is* snow just water? Yes, typically at the 99.99%+ level:
# https://link.springer.com/referenceworkentry/10.1007/978-90-481-2642-2_59

compositions = { 
  'water': { 'H' : 2, 'O': 1 },
}

fractions = {
  'water': 1,
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

def snow_composition():
  return make_composition("snow", fractions, compositions)
