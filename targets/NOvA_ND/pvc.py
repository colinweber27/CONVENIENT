from atomicdata import *
from util import *

# takes a mass fraction composition
def totalstopppingpower(compos, name):
  totstop = 0
  totcomp = 0
  for elem in elemlist:
    if elem in compos:
      totstop += compos[elem]*stoppingpower[elem]
      totcomp += compos[elem]
  answer = totstop/totcomp
  print("{0:12s} stopping power: {1:.3f} MeV/cm^2/g".format(name, answer))
  return answer

# From http://arxiv.org/abs/1601.00908
# These are number fractions.
compositions = { 

  # Pure PVC is just C2H3Cl. This is the polymer unit and does not change
  # upon polymerization 
  'pvc'                      : { 'C':2, 'H':3, 'Cl':1 },

   # Rohm & Haas Advastab TM-181 tin stabilizer
   # From the safety data sheet, we learn that "monomethyl tin" is actually
   # 2-ethylhexyl 10-ethyl-4-[[2-[(2- ethylhexyl)oxy]-2-oxoethyl]thio]-4-
   # methyl-7-oxo-8-oxa-3,5-dithia-4-stannatetradecanoate,
   # CAS 57583-34-3
  'tin stabilizer monomethyl': { 'Sn':1, 'C':22, 'H':44, 'O':4, 'S':2 },

  # And this is 2-ethylhexyl 10-ethyl-4,4-dimethyl-7-oxo-8-oxa-3,5- dithia-
  # 4-stannatetradecanoate, CAS 57583-35-4
  'tin stabilizer dimethyl'  : { 'Sn':1, 'C':31, 'H':60, 'O':6, 'S':3 },

  # And this is Ethylhexyl thioglycolate, CAS 7659-86-1
  'tin stabilizer ehtg'      : { 'S':1, 'O':2, 'C':9, 'H':20 },

  # Our titanium dioxide is Kronos 1000, which is ">= 98.5%" TiO2. The
  # crystals, anatase, are not intentionally coated (as per our PVC
  # paper, section 3), so I don't know what the other <= 1.5% might be.
  # Just model as pure.
  'titanium dioxide'         : { 'Ti':1, 'O':2 },

  # Also easy!
  'calcium stearate'         : { 'Ca':1, 'C':36, 'H':70, 'O':4 },

  # Annoying.  Have to guess at the average chain length and amount of
  # branching.  Wikipedia suggests 30 for the chain length, and 
  # a page at chemicalland21.com suggests 99% straight chains.
  'paraffin wax'             : { 'C':30, 'H':61.97 }, 

  # Has many names.  In IUPAC, 6-hydroxy-5-methyl-4,11-dioxoundecanoic acid
  # See pubchem.ncbi.nlm.nih.gov
  'oxidized polyethylene'    : { 'C':12, 'H':20, 'O':5 },

  # Also look this one up at pubchem.ncbi.nlm.nih.gov
  'glycerol monostearate'    : { 'C':21, 'H':42, 'O':4 },

  # I could not find the composition of the impact modifier, but
  # based on my reading, methylmethacrylate is not a crazy guess
  'acrlyic impact modifier polymer'    :{ 'C': 5, 'H':8, 'O':2 },

  # The datasheet lists this just as "proprietary", guess sodium lauryl sulfate
  'acrlyic impact modifier surfactant' :{ 'C':12, 'H':25, 'Na':1, 'O':4, 'S':1 },

  # Just calcium carbonate
  'acrlyic impact modifier cacarbonate':{ 'Ca':1, 'C':1, 'O':3 },

  # Datasheet says the processing aid is 
  # 98-99% CAS 9010-88-2, 1-2% Sodium lauryl sulfate
  'processing aid polymer'  : { 'C':10, 'H':16,'O':4 },
  'processing aid detergent': { 'C':12, 'H':25, 'Na':1, 'O':4, 'S':1 },
}


fractions = {
  'pvc'                      :100,

  # Advastab TM-181 data sheet says that these are 20-30%, 60-70%
  # and 1-10%, respectively.  An analysis at SpecialChem.com claims
  # an overall tin content higher than the highest one can get from
  # those numbers, so maximize the tin within the datasheet range.
  'tin stabilizer monomethyl':  2.5*0.19,
  'tin stabilizer dimethyl'  :  2.5*0.80,
  'tin stabilizer ehtg'      :  2.5*0.01,

  'titanium dioxide'         : 19,

  'calcium stearate'         :  0.8,
  'paraffin wax'             :  1.1,
  'oxidized polyethylene'    :  0.2,
  'glycerol monostearate'    :  0.3,

  # Took middle-of-the-road guesses on the fractions.
  'acrlyic impact modifier polymer'      :  4*0.95,
  'acrlyic impact modifier surfactant'   :  4*0.025,
  'acrlyic impact modifier cacarbonate'  :  4*0.025,

  'processing aid polymer'   :  1*0.985, # 98-99%
  'processing aid detergent' :  1*0.015, #  1- 2%
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

def pvc_composition():
  return make_composition("pvc", fractions, compositions)
