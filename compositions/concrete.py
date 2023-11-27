from atomicdata import *
from util import *

# Generates the composition of concrete for the ND and FD, meant to include
# both the shotcrete liner and the poured concrete floor.
#
# This is all done using reasonable guesses and Wikipedia.  See also
# nova-doc-25794.
#
# I'll put the densities in by hand in the GDML since I found a number
# for the FD overburden concrete, but not for anything at the ND.

compositions = {
  'cementSiO2':  { 'Si' : 1, 'O': 2 },
  'sandSiO2':  { 'Si' : 1, 'O': 2 },
  'Al2O3': { 'Al' : 2, 'O': 3 },
  'CaO' :  { 'Ca' : 1, 'O': 1 },
  'CaCO3': { 'Ca' : 1, 'O': 3, 'C': 1 },
  'Fe2O3': { 'Fe' : 2, 'O': 3 },
  'SO3':   { 'S'  : 1, 'O': 3 },
  'water': { 'H'  : 2, 'O': 1 },

  # Just elements, for checking old composition + water
  'oxygen':   { 'O'  : 1 },
  'silicon':  { 'Si' : 1 },
  'calcium':  { 'Ca' : 1 },
  'sodium':   { 'Na' : 1 },
  'iron':     { 'Fe' : 1 },
  'aluminum': { 'Al' : 1 },
}

fractions = {
  # Using either mix at 
  # http://www.everything-about-concrete.com/concrete-mixing-ratios.html,
  # I get about 7% water by mass
  'water': 0.07,

  # Here's the old composition, so we can check what happens if all we
  # do is add water.
  #'oxygen':   0.531,
  #'silicon':  0.332,
  #'calcium':  0.061,
  #'sodium':   0.015,
  #'iron':     0.020,
  #'aluminum': 0.041,

  # And here's a probably better (or at least traceable) composition

  # And 13% cement, whose composition I get from
  # https://en.wikipedia.org/wiki/Portland_cement on 2017-11-27
  'CaO' :  0.65 * 0.13,
  'cementSiO2':  0.22 * 0.13,
  'Al2O3': 0.06 * 0.13,
  'Fe2O3': 0.03 * 0.13,
  'SO3':   0.04 * 0.13,

  # And the other 80% "sand" and "stone", which I'll give a very
  # crude composition based on https://en.wikipedia.org/wiki/Sand
  'sandSiO2':  0.7 * 0.8,
  'CaCO3': 0.3 * 0.8,
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

def concrete_composition():
  return make_composition("concrete", fractions, compositions)
