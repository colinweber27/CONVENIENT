> These scripts produce the elemental composition of our detector 
materials. They are output as GDML fragments.  These fragments are 
read into the scripts one directory up that create the full GDML 
files.  The Makefile two directories up takes care of getting this 
done.

> This is a patchwork of scripts in different languages not to be 
contrary, but because I wasn't aware that Perl was in use when I 
decided to write the composition scripts in Python.  I have attempted to avoid using constructs that are likely to break when the 
interpreter for either language is updated, but nevertheless 
apologize in advance for the breakage rate being at least twice as 
high as you might like."

- Original Author unknown but possibly Matt Strait, who pointed me to the DocDB (20816) that explains more.

The entire contents of this folder was copied from 
`$SRT_PUBLIC_CONTEXT/Geometry/gdml/compositions`. The only 
modifications I've made to existing files are to `soup.py` to get it to 
output more digits and show ND results. I also wrote several other 
files listing the ND target composition in GENIE and NEUT format to various 
precisions. The number in the file names of these indicates the 
number of decimal points of precision used in the fraction of each 
element.
