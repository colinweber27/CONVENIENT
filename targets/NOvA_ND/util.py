from atomicdata import *

elemlist = [ 'H', 'C', 'O', 'N', 'S', 'Cl', 'Ti', 'Sn', 'Ca', 'Na',
             'Al', 'Fe', 'Si', 'K', 'Mn', 'P', 'Ba' ]

printmore = False

def tot_weight(compos):
  weight = 0
  for element, number in compos.items():
     weight += number * atomic_weight[element]
  return weight

def tot_fractions(fractions):
  tions = 0
  for material, fraction in fractions.items():
     tions += fraction
  return tions

def elem_frac(compos, elem):
  if elem in compos:
    return compos[elem] * atomic_weight[elem] / tot_weight(compos)
  else:
    return 0

def elem_frac_total(elem, compositions, fractions):
  frac = 0
  for material, compos in compositions.items():
    if material in fractions:
      frac += elem_frac(compos, elem) * fractions[material]
  return frac / tot_fractions(fractions)

def zovera(elem):
  # Not quite correct since atomic_weight in amu is only protons +
  # neutrons up to the mass defect
  return atomic_number[elem]/atomic_weight[elem]

# takes a weight-fraction composition
def zoveramaterial(compos):
  zoverasum = 0
  for elem in elemlist:
    if elem in compos:
      zoverasum += zovera(elem)*compos[elem]
  return zoverasum

def make_composition(name, fractions, compositions):
  the_composition = { }

  for elem in elemlist:
    the_composition[elem] = elem_frac_total(elem, compositions, fractions)

  return the_composition


def print_composition(composition):
  for elem in elemlist:
    if elem in composition and composition[elem] > 0:
      print("    <fraction n=\"{0:.7f}\" ref={1:<10s}/>"
        .format(composition[elem],
                "\"{0:s}\"".format(atomic_names[elem]))),
      if elem == 'Cl':
         print("<!-- Cl-35: {0:.4f}, Cl-37: {1:.4f} -->".
           format(composition[elem]*cl35massfrac,
                  composition[elem]*cl37massfrac))
      else:
         print("")
  if printmore:
    for elem in elemlist:
      if elem in composition and composition[elem] > 0:
        print("const double {0:s}_{1:<2s} = {2:f};"
          .format(name, elem, elem_frac_total(elem, compositions, fractions)))
