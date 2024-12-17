from air   import air_composition

# weight fractions in the MC prior to this work
old_pvc = {
  'H' : 0.0411,
  'C' : 0.3267,
  'Cl': 0.4822,
  'Ti': 0.0899,
  'O' : 0.0601,
  'Na': 0.00000,
  'S' : 0.00000,
  'Sn': 0.00000,
  'Ca': 0.00000,
  'N' : 0.00000,
}

# Yes, the glue is modeled as pure diamonds at the moment
old_glue = {
  'C': 1,
}

old_scint = {
  'H': 0.1454,
  'C': 0.8545,
  'N': 0.00005,
  'O': 0.00005,
}

old_compositions = {
  'glue': old_glue,
  'pvc' : old_pvc,
  'scint':old_scint,
  'air': air_composition(),
}

old_fractions = {
  'glue': 49369.273184,
  'pvc': 4879106.942957,
  'scint': 8915111.995643,
  'air' :      440, # same as new without bubbles
}

old_densities = {
  'glue':  1.34,
  'pvc':   1.49,
  'scint': 0.859,
  'air':   0.001205
}

# Not part of the old soup, but useful old materials for comparisons.

# We used to use the same composition for rock, soil, etc.
old_rock = {
  'O': 0.437,
  'Si': 0.257,
  'Na': 0.222,
  'Fe': 0.020,
  'Al': 0.049,
  'K' : 0.015,
}

old_fdconcrete = {
  'O': 0.531,
  'Si': 0.332,
  'Ca': 0.061,
  'Na': 0.015,
  'Fe': 0.020,
  'Al': 0.041,
}

old_barite = {
  'Ba': 0.5885,
  'S': 0.1374,
  'O': 0.2742,
}
