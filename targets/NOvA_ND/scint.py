from atomicdata import *
from util import *

# This composition is a further refinement of the update reported
# on in doc-15338, "Detector Simulation Technote".

compositions = { 
  # pseudocumeme was required to have less than 1e-6 sulfur
  # as per doc-13014.  So 0.3e-6 is a reasonable guess.  That's by
  # weight, so 0.15e-6 by count. Assume similar N and O.  
  'Pseudocumene'  : { 'C':9, 'H':12, 'S':0.15e-6, 'N': 0.3e-6, 'O': 0.3e-6 },

  # As per doc-13911. Analysis seems questionable to me, as it
  # excludes odd-numbered chains, except, bizzarely, for chains of
  # length 19.  But short of getting a new analysis, I don't see
  # any way to correct for this.
  #
  # And it is "Virtually free of nitrogen, sulfur, oxygen and aromatic
  # hydrocarbons." says the Renoil brochure. Use the pseudocumeme
  # tolerance as a guide. Just a guess.
  'Oil'  : { 'C':1, 'H':2.0555, 'S': 0.15e-6, 'N': 0.3e-6, 'O': 0.3e-6},

  'PPO' : { 'C':15, 'H':11, 'N':1, 'O':1 },

  'Bis-MSB' : { 'C':24, 'H':22 },

  # Assuming an even mix of alpha, beta, gamma and delta tocopherols.
  # It hardly makes a difference. The variation is from C27H46O2 to
  # C29H50O2.
  'Vitamin E':{ 'C':28, 'H':48, 'O':2 },

  # Obsessively model Stadis-425 even though it is only 0.001%. Not
  # *too* obsessively. Take what Wikipedia says about the average chain
  # length of heavy naptha, assume 1:1 mix of straight and branched and
  # move on.
  'Stadis - heavy naptha': { 'C': 9, 'H': 9*2+1},

  # Easy.
  'Stadis - o-xylene': { 'C': 8, 'H': 10},

  # Again take Wikipedia's statement about average chain length
  'Stadis - kerosene': { 'C': 11, 'H': 11*2+1},

  'Stadis - acid': { 'C': 18, 'H': 30, 'O': 3, 'S':1 },

  # Trade secrets...  Just guessing something vaguely sensible
  'Stadis - secret with sulfur'  : { 'C':1, 'H':2, 'S':1 },
  'Stadis - secret with nitrogen': { 'C':1, 'H':2, 'N':1 },

  # A contaminant
  'Water': { 'H':2, 'O':1},

  # Dissolved
  'Nitrogen': { 'N': 2},
  'Oxygen': { 'O': 2},

  # Since the MC does not model the fiber, mix it in such that the
  # scintillator is really a scintillator+fiber soup
  # From doc-2665

  # Ignore wavelength shifter, which is a very small fraction of a very small
  # fraction and involves ruthenium, which I don't want to put in
  'Fiber core': { 'C':1, 'H': 1 },

  # We know what PMMA is, so use the formula instead of their atom counts 
  # (which agree with the formula, but are rounded off)
  'Fiber inner cladding': { 'C':5, 'H': 8, 'O': 2 },

  # This is "flourinated polymer".  Assume teflon, CF2, and then ignore the flourine
  # since I don't want to add another element with a tiny fraction.
  'Fiber outer cladding': { 'C':1 },
}

# Most of the blends of scintillator, with higher pseudocumene and PPO
# concentrations.
fractions_late = {
  # in kg
  'Oil':         7658656, # doc-13014-v5, blends 3-25
  'Pseudocumene': 423278,
  'PPO' :          11331,
  'Bis-MSB' :        129,

  'Stadis - kerosene':             81*0.65,
  'Stadis - o-xylene':             81*0.15,
  'Stadis - acid':                 81*0.05,
  'Stadis - heavy naptha':         81*0.05,
  'Stadis - secret with sulfur'  : 81*0.05,
  'Stadis - secret with nitrogen': 81*0.05,

  'Vitamin E':        78,

  # As per doc-13014, limit is 50ppm, so guess 15ppm.  Is it silly to include
  # this?  Maybe, but there is more water in the scintillator (best estimate)
  # than either Stadis or vitamin E.
  'Water': 8000000 * 15e-6,

  # As suggested by doc-7236, but assuming full saturation based on comments
  # received at a Detsim meeting.  This is the biggest contribution of oxygen
  # in the scintillator.
  'Oxygen': 8000000 * 2*atomic_weight['O']/(19*atomic_weight['C'] + (19*2+1)*atomic_weight['H']) * 5.048e-3,

  # Two shady sources, jbc.org/content/72/2/545.full.pdf and
  # nttworldwide.com/docs/dga2102.pdf suggest that the solubility of nitrogen
  # is very close to half that of oxygen, and it should be fully saturated.
  # This is the biggest contribution of nitrogen in the scintillator.
  'Nitrogen': 8000000 * 2*atomic_weight['N']/(19*atomic_weight['C'] + (19*2+1)*atomic_weight['H']) * 5.048e-3 / 2,

  # Assume 26 blocks worth of fiber -- not yet sure of the exact number (but
  # also not that important).  Using fiber mass from doc-11905, fractions
  # derived from doc-2665.  Corrected down by 6% to account for the difference
  # between the mass of blended scintillator above vs. amount in the relevant
  # FD volume.
  'Fiber core':           0.94 * 0.4505 * 12 * 32 * 26 * 0.86,  # about 5000kg
  'Fiber inner cladding': 0.94 * 0.4505 * 12 * 32 * 26 * 0.06, 

  # Deweight by the ignored flourine
  'Fiber outer cladding': 0.94 * 0.4505 * 12 * 32 * 26 * 0.08 *
    atomic_weight['C']/(atomic_weight['C'] + 2 * atomic_weight['F']), 
}


# The first two blends of scintillator, with lower pseudocumene and PPO
# concentrations.
fractions_early = {
  # in kg
  'Oil':          691179, # doc-13014-v5, blends 1-2
  'Pseudocumene':  36266.7, # corrected typo in doc-13014-v5
  'PPO' :            801,
  'Bis-MSB' :         11.7,

  'Stadis - kerosene':             7.3*0.65,
  'Stadis - o-xylene':             7.3*0.15,
  'Stadis - acid':                 7.3*0.05,
  'Stadis - heavy naptha':         7.3*0.05,
  'Stadis - secret with sulfur'  : 7.3*0.05,
  'Stadis - secret with nitrogen': 7.3*0.05,

  'Vitamin E':        7.1,

  'Water': 700000 * 15e-6,

  'Oxygen': 700000 * 2*atomic_weight['O']/(19*atomic_weight['C'] + (19*2+1)*atomic_weight['H']) * 5.048e-3,

  'Nitrogen': 700000 * 2*atomic_weight['N']/(19*atomic_weight['C'] + (19*2+1)*atomic_weight['H']) * 5.048e-3 / 2,

  # Assume 2 blocks worth of fiber -- not yet sure of the exact number
  # (but also not that important)
  'Fiber core':           0.94 * 0.4505 * 12 * 32 * 2 * 0.86,
  'Fiber inner cladding': 0.94 * 0.4505 * 12 * 32 * 2 * 0.06, 
  'Fiber outer cladding': 0.94 * 0.4505 * 12 * 32 * 2 * 0.08 *
    atomic_weight['C']/(atomic_weight['C'] + 2 * atomic_weight['F']), 
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

def scint_composition_late():
  return make_composition("scint", fractions_late, compositions)

def scint_composition_early():
  return make_composition("scint", fractions_early, compositions)
