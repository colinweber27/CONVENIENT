# Author: unknown. This was copied from $SRT_PUBLIC_CONTEXT/Geometry/gdml/compositions
# Date: No earlier than 11 August 2016. Modified 19 July 2023.
# Purpose: To give the chemical composition of the NOvA detectors
#
# Command: python soup.py

from atomicdata import *
from util import (elemlist, zoveramaterial, print_composition)
from glue  import glue_composition
from pvc   import pvc_composition
from scint import scint_composition_late
from scint import scint_composition_early
from air   import air_composition
from soil  import soil_composition
from snow  import snow_composition
from concrete import concrete_composition
from barite import barite_composition
from fdbermrock import fdbermrock_composition
from fdbedrock import fdbedrock_composition
from oldsoup import (old_pvc, old_glue, old_scint, old_rock, old_fdconcrete, old_barite, old_compositions,
                     old_fractions, old_densities)

# Not chemical formula-like!  Now these are mass fractions
compositions = {
  'glue'  : glue_composition(),
  'pvc'   : pvc_composition(),
  'scint early' : scint_composition_early(),
  'scint late'  : scint_composition_late(),
  'air'         : air_composition()
}

print("Glue:")
gluecomp = print_composition(compositions['glue'])
print("Scintillator, blends #1 and #2:")
scintcomp= print_composition(compositions['scint early'])
print("Scintillator, blends #3-25:")
scintcomp= print_composition(compositions['scint late'])
print("PVC:")
pvccomp  = print_composition(compositions['pvc'])

fractions = {
  # FD MC as of 2016-06-14
  'glue':    98867.292555,
  'pvc'  : 5062151.135481,
  'scint early':  760497,
  'scint late' : 7935040,
  'air' : 596.9,
}

# Without the muon capture, so these are the fiducial fractions.
nd_fractions = {
  # ND MC as of 2016-08-11
  'glue':    1460.144683,
  'pvc'  : 75248.258120,
  'scint early':  0,
  'scint late' : 129871.066409,
  'air' : 5.8, # I think?  This is harder to count properly since there
               # is air all around the detector.  It is about half the fraction
               # as compared to the FD, so it is reasonable.
}

densities = {
  # FD MC as of 2016-06-29
  'glue':        0.98,
  'pvc':         1.482,
  'scint early': 0.8530, # actually the average FD scint+fiber density
  'scint late':  0.8530, # actually the average FD scint+fiber density
  'scint nd'  :  0.8576, # average ND scint+fiber density
  'air':         0.001205,
}

def mean_density(fs, ds):
  volume = 0
  totmass = 0
  for material, mass in fs.items():
    volume  += mass/ds[material]
    totmass += mass
  return totmass/volume

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


def soup_composition():

  the_composition = { }

  for elem in elemlist:
    the_composition[elem] = elem_frac_total(elem, compositions, nd_fractions)
  print("Soup:")
  for elem in elemlist:
    print("const double soup_{0:<2s} = {1:.11f};"
      .format(elem, elem_frac_total(elem, compositions, nd_fractions)))

  return the_composition

soup = soup_composition()

def old_soup_composition():

  the_composition = { }

  for elem in elemlist:
    the_composition[elem] = elem_frac_total(elem, old_compositions, old_fractions)
  print("Old Soup:")
  for elem in elemlist:
    print("const double soup_{0:<2s} = {1:.8f};"
      .format(elem, elem_frac_total(elem, old_compositions, old_fractions)))

  return the_composition

old_soup = old_soup_composition()

# takes a mass fraction composition
def radlength(compos, name, verbose):
  totilength = 0
  for elem in elemlist:
    if elem in compos:
      totilength += compos[elem]*inverseradlength[elem]
  if verbose:
    print("{0:12s} radiation length: {1:.2f} g/cm^2".format(name, 1/totilength)), 
  return 1/totilength

# takes a mass fraction composition
def totalstoppingpower(compos, name, verbose):
  totstop = 0
  for elem in elemlist:
    if elem in compos:
      totstop += compos[elem]*stoppingpower[elem]
  if verbose:
    print("{0:12s} stopping power: {1:.3f} MeV/cm^2/g".format(name, totstop)),
  return totstop

print("")
radlength(old_scint, "Old scint", True)
print("")
radlength(compositions['scint early'], "Early scintillator", True) 
print(" ({0:+0.2f}% larger)".format(100*(radlength(compositions['scint early'], "", False)/radlength(old_scint, "", False)-1)))
radlength(compositions['scint late'], "Late  scintillator", True)
print(" ({0:+0.2f}% larger)".format(100*(radlength(compositions['scint late'], "", False)/radlength(old_scint, "", False)-1)))
print("")
radlength(old_pvc, "Old PVC", True)
print("")
radlength(compositions['pvc'], "PVC", True)
print(" ({0:+0.2f}% larger)".format(100*(radlength(compositions['pvc'], "", False)/radlength(old_pvc, "", False)-1)))
print("")
radlength(old_glue, "Old glue", True)
print("")
radlength(compositions['glue'], "Glue", True)
print(" ({0:+0.2f}% larger)".format(100*(radlength(compositions['glue'], "", False)/radlength(old_glue, "", False)-1)))

# z extent of the average plane = extrusion depth + glue + block gap
plane = 6.61 + 0.0379 + 0.45/32
oldmeandensity = mean_density(old_fractions, old_densities)
newmeandensity = mean_density(    fractions,     densities)
nd_newmeandensity = mean_density( nd_fractions,  densities)
print("")
print("old mean FD density {0:0.3f}g/cm^3, new {1:0.3f}g/cm^3".format(mean_density(old_fractions, old_densities), mean_density(fractions, densities)))
print("new mean ND density {0:0.3f}g/cm^3".format(mean_density(nd_fractions, densities)))

print("")
oldrl = radlength(old_soup, "Old Soup", True)
poldrl = oldrl/mean_density(old_fractions, old_densities)
print("= {0:0.2f} cm = 1/{1:0.3f} planes".format(poldrl, plane/poldrl))
newrl = radlength(soup, "Soup", True)
pnewrl = newrl/mean_density(fractions, densities)
print("= {0:0.2f} cm = 1/{1:0.3f} planes".format(pnewrl, plane/pnewrl)),
print(" ({0:+0.2f}% (by X_0)/{1:+0.2f}% (by absolute distance) larger)".format(100*(newrl/oldrl-1), 100*(pnewrl/poldrl - 1)))

print("")
print("")
totalstoppingpower(old_scint, "Old scint", True)
print("")
totalstoppingpower(compositions['scint early'], "Early scintillator", True)
print(" ({0:+0.2f}% longer)".format(-100*(totalstoppingpower(compositions['scint early'], "", False)/totalstoppingpower(old_scint, "", False)-1)))
totalstoppingpower(compositions['scint late'], "Late  scintillator", True)
print(" ({0:+0.2f}% longer)".format(-100*(totalstoppingpower(compositions['scint late'], "", False)/totalstoppingpower(old_scint, "", False)-1)))
print("")
totalstoppingpower(old_pvc, "Old PVC", True)
print("")
totalstoppingpower(compositions['pvc'], "PVC", True)
print(" ({0:+0.2f}% longer)".format(-100*(totalstoppingpower(compositions['pvc'], "", False)/totalstoppingpower(old_pvc, "", False)-1)))
print("")
totalstoppingpower(old_glue, "Old glue", True)
print("")
totalstoppingpower(compositions['glue'], "Glue", True)
print(" ({0:+0.2f}% longer)".format(-100*(totalstoppingpower(compositions['glue'], "", False)/totalstoppingpower(old_glue, "", False)-1)))
print("")
oldstop = totalstoppingpower(old_soup, "Old Soup", True)
print("= {0:0.2f} planes/GeV".format(1000/oldstop/oldmeandensity/plane))
newstop = totalstoppingpower(soup, "Soup", True)
print("= {0:0.2f} planes/GeV".format(1000/newstop/newmeandensity/plane)),
print(" ({0:+0.2f}% (by dE/dx)/{1:+0.2f}% (by absolute distance) longer)".format(-100*(newstop/oldstop-1), -100*(newstop/oldstop*newmeandensity/oldmeandensity - 1)))

print("")
print("PVC:   Old Z/A = {0:.5f}".format(zoveramaterial(old_pvc)))
print("       New Z/A = {0:.5f} ({1:+0.2f}% nu interactions)".format(zoveramaterial(compositions['pvc']),
                          -((zoveramaterial(compositions['pvc'])
                         - zoveramaterial(old_pvc))/zoveramaterial(old_pvc))*100))
print("")
print("Scint: Old Z/A = {0:.5f}".format(zoveramaterial(old_scint)))
print(" early New Z/A = {0:.5f} ({1:+0.2f}% nu interactions)".format(zoveramaterial(compositions['scint early']),
                          -((zoveramaterial(compositions['scint early'])
                         - zoveramaterial(old_scint))/zoveramaterial(old_scint))*100))
print(" late  New Z/A = {0:.5f} ({1:+0.2f}% nu interactions)".format(zoveramaterial(compositions['scint late']),
                          -((zoveramaterial(compositions['scint late'])
                         - zoveramaterial(old_scint))/zoveramaterial(old_scint))*100))
print("")
print("Glue:  Old Z/A = {0:.5f}".format(zoveramaterial(old_glue)))
print("       New Z/A = {0:.5f} ({1:+0.2f}% nu interactions)".format(zoveramaterial(compositions['glue']),
                          -((zoveramaterial(compositions['glue'])
                         - zoveramaterial(old_glue))/zoveramaterial(old_glue))*100))
print("")
print("Soup:  Old Z/A = {0:.5f}".format(zoveramaterial(old_soup)))
print("       New Z/A = {0:.5f} ({1:+0.2f}% nu interactions)".format(zoveramaterial(soup),
                          -((zoveramaterial(soup)
                         - zoveramaterial(old_soup))/zoveramaterial(old_soup))*100))

print("Air:          Z/A = {0:.5f}".format(zoveramaterial(air_composition())))
print("Snow:         Z/A = {0:.5f}".format(zoveramaterial(snow_composition())))
print("Concrete:     Z/A = {0:.5f}".format(zoveramaterial(concrete_composition())))
print("Old concrete: Z/A = {0:.5f}".format(zoveramaterial(old_fdconcrete)))
print("Barite:       Z/A = {0:.5f}".format(zoveramaterial(barite_composition())))
print("Old barite:   Z/A = {0:.5f}".format(zoveramaterial(old_barite)))
print("Old rock:     Z/A = {0:.5f}".format(zoveramaterial(old_rock)))
print("Soil:         Z/A = {0:.5f}".format(zoveramaterial(soil_composition())))
print("FD berm rock: Z/A = {0:.5f}".format(zoveramaterial(fdbermrock_composition())))
print("FD Bedrock:   Z/A = {0:.5f}".format(zoveramaterial(fdbedrock_composition())))
