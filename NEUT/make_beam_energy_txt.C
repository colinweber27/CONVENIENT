// Author: Colin M Weber (webe1077@umn.edu)
// Date: 12 April 2024
// Purpose: To read in a ROOT file with a neutrino flux and output a text 
// file with the spectrum translated into NEUT beam energy format, and all 
// bins below 100 MeV set to 0.
//
// NEUT beam energy format is left edge[tab] value\n, where each new line is 
// a new bin.
// Command: root -q "NEUT/make_beam_energy_txt.C(
// 		\"flux file\", \"flux histo\")"
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
void make_beam_energy_txt(std::string flux_file_str, \
	std::string flux_histo_string, std::string output) {
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

	// Loop through the bins, and append their height to the string
	for (int bin = 1; bin <= n_bins; bin++)
	{
		double low_edge = flux_histo->GetBinLowEdge(bin);
		beam_energy += std::to_string(low_edge);
		beam_energy += "\t";
		double bin_height;
		if(low_edge < 0.1) {
			bin_height = 0.0;
		}
		else {
			bin_height = flux_histo->GetBinContent(bin);
		}
		beam_energy += std::to_string(bin_height);
		beam_energy += "\n";
	}

	// Create the new file
	std::ofstream new_file(output.c_str());

	// Write the beam energy to the file
	new_file << beam_energy;

	// Save and close the new file
	new_file.close();

	// Close the original file
	flux_file->Close();
}
