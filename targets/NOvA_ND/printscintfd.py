from air import air_composition
from scint import scint_composition_early, scint_composition_late
from util import (elemlist, zoveramaterial, print_composition)

compositions = {
  'air'        : air_composition(),
  'scint early': scint_composition_early(),
  'scint late' : scint_composition_late(),
}
fractions = {
  # Mix in 300kg of air for the otherwise-unmodeled air bubbles. 
  # Distinct from dissolved gasses!
  'air'        :     300,
  'scint early':  760497,
  'scint late' : 7935040,
}

def tot_weight(compos):
  weight = 0
  for element, number in compos.items():
     weight += number
  return weight

def tot_fractions(fs):
  tions = 0
  for material, fraction in fs.items():
     tions += fraction
  return tions

def elem_frac(compos, elem):
  if elem in compos:
    return compos[elem] / tot_weight(compos)
  else:
    return 0

def elem_frac_total(elem, comps, fs):
  frac = 0
  for material, compos in comps.items():
    frac += elem_frac(compos, elem) * fs[material]
  return frac / tot_fractions(fs)

def scintfd_composition():

  the_composition = { }

  for elem in elemlist:
    the_composition[elem] = elem_frac_total(elem, compositions, fractions)
  return the_composition

print_composition(scintfd_composition())
