from atomicdata import *
from util import *

compositions = { 
  # Methacrylic acid
  'adhesive MAA'  : { 'C':4, 'H':6, 'O':2 },

  # methyl methacrylate 
  'adhesive MMA'  : { 'C':5, 'H':8, 'O':2 },

  # Cholorosulfonated polyethylene
  'adhesive CSP'  : { 'C':101, 'H':173, 'Cl':13, 'O':2, 'S':1 },

  # Carbon tetrachloride
  'adhesive CCl4' : { 'C':1, 'Cl':4 },

  # p(BD/MMA/STY), CAS 25053092,
  # 2-Propenoic acid, 2-methyl-, methyl ester, polymer with
  # 1,3-butadiene and ethenylbenzene
  # Not sure this is really an exact formula.  Given on one data sheet and
  # listed as "trade secret" on another.
  'adhesive other': { 'C':17, 'H': 22, 'O': 2 },

  'activator MMA': { 'C':5, 'H':8, 'O':2 }, 

   # 3,5-Diethyl-1,2-dihydro-1-phenyl-2-propylpyridine
  'activator PDHP': { 'C':18, 'H':25, 'N': 1}
}

fractions = {
  # Trade secret components are ignored, since we have no idea
  # what they are.  The rest is scaled up to match.

  # One data sheet says 1-10 and another says 5-10...
  'adhesive MAA':   6,

  # Listed as 30-60 on one sheet and 40-60 on another. Take into account
  # that doc-8946 suggests that 5% of the cured mixture is lost to
  # outgassing MMA vapors.
  'adhesive MMA':  47.7-2*5,

  # Listed as 20-30 on one sheet and 30-60 on another.  What.
  'adhesive CSP':  30,

  # 0.1-1 on one sheet, <1 on another.
  'adhesive CCl4':  0.3,

  # Both sheets say 60-100.  Amazing!
  'activator MMA': 80,

  # 1-5 on one sheet and 1-10 on another
  'activator PDHP': 4,
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

def glue_composition():
  return make_composition("glue", fractions, compositions)
