// Author: Colin Weber (webe1077@umn.edu)
// Date: 26 March 2024
// Purpose: To extract histograms from fSpecsFakeData.root, which is 
// the output of makeFakeDataFluxes.C, which uses the DeriveFlux 
// function to extract the NuMI flux from CAF files.

// Command: root -q "extract_histos.C(\"full file path\", \
//     \"full histo path within file\", \"new_file_name\", \
//     \"new_histo_name\")"

// Ex: root -q "extract_histos.C(\"/exp/nova/app/users/kjvocker/generator_predictions/flux/NOvA_ND_FHC_numubar_flux_PPFX.root\", \"\", \"/exp/nova/app/users/colweber/Flux/DeriveFlux/FHC/numubar/DeriveFlux50MeVwidth_ppfx/numubar.root\", \"numubar\")"

// Parameters
// 	full_file_path : str
// 		The full file path of the input .root file containing the histograms
// 	full_histo_path : str
// 		The full path to the histograms within the file at full_file_path
// 	new_file_name : str
// 		The name of the output file
// 	new_histo_name : str
// 		The name of the output histogram

// Outputs
// 	new_file : ROOT TFile*
// 		The file containing the extracted ROOT histograms

// ROOT includes
#include "TFile.h"
#include "TH1.h"

void extract_histos(const char* full_file_path, \
	const char* full_histo_path, const char* new_file_name, \
	const char* new_histo_name) {
	// Open the file containing the histo
	TFile* old_file = TFile::Open(full_file_path);
	// Open the histogram within that file
	TH1D* old_histo = (TH1D*) old_file->Get(full_histo_path);

	// Create the new file
	std::unique_ptr<TFile> new_file(TFile::Open(new_file_name, "RECREATE"));

	// Copy the old histogram
	TH1D* new_histo = (TH1D*) old_histo->Clone(new_histo_name);

	// Save the new histogram to the file
	new_file->Write();
	new_file->Close();
	old_file->Close();
}
