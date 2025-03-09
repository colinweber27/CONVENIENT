// Author: Colin M Weber (webe1077@umn.edu)
// Date: 17 August 2023
// Purpose: To read in a ROOT file with an energy spectrum and output a text 
// file with the spectrum translated into NuWro beam energy format.
//
// NuWro beam energy format is 'E0 E1 a1 ... an', where E0 is the minimum 
// beam energy, E1 is the maximum beam energy, and ai are the heights of the 
// bins. The bins need not be normalized.

// Command: root -q "make_beam_energy.C(\"flux file\", \"flux histo\")"
// where flux file is the path to the ROOT flux file containing the energy 
// spectrum, and flux histo is the name of the energy spectrum in the flux 
// file.

// Includes
// C++ includes
#include <string>
#include <algorithm>
#include <iostream>
// ROOT includes
#include <TFile.h>
#include <TH1D.h>

// Define the function. It takes two required inputs: a string that is the 
// path to the ROOT flux file, and a string that is the histogram in the 
// file that contains the energy spectrum.
void make_beam_energy(std::string flux_file_str, \
	std::string flux_histo_string, std::string output_filename)
{
	// Read in the flux file
	TFile* flux_file = new TFile(flux_file_str.c_str());

	// Read in the energy spectrum from the flux file
	TH1D* flux_histo = (TH1D*)flux_file->Get(flux_histo_string.c_str());
	
	// Initialize the beam energy string, which will be constructed 
	// throughout the rest of the macro.
	std::string beam_energy = "";

	// Get the number of bins in the histogram, which we use for looping 
	// through the bins.
	int n_bins = flux_histo->GetNbinsX();

	// Get E0, the minimum energy, and append it to the beam energy string.
	// * 1000 converts from GeV to MeV.
	int low_E = flux_histo->GetBinLowEdge(1) * 1000;
	beam_energy += std::to_string(low_E);
	beam_energy += " ";

	// Get E1, the maximum energy, and append it to the beam energy string.
	// * 1000 converts from GeV to MeV.
	int high_E = (flux_histo->GetBinLowEdge(n_bins) + \
		flux_histo->GetBinWidth(n_bins)) * 1000;
	beam_energy += std::to_string(high_E);
	beam_energy += " ";

	// Loop through the bins, and append their height to the string
	for (int bin = 1; bin <= n_bins; bin++)
	{
		double bin_height = flux_histo->GetBinContent(bin);
		beam_energy += std::to_string(bin_height);
		beam_energy += " ";
	}

	// Create the new file
	std::ofstream new_file(output_filename.c_str());

	// Write the beam energy to the file
	new_file << beam_energy;

	// Save and close the new file
	new_file.close();

	// Close the original file
	flux_file->Close();
}
