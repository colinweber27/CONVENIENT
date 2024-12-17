// Author: Colin Weber (webe1077@umn.edu)
// Date: 24 April 2024
// Purpose: To read in a list of Convenient files and combine them. This 
// macro does no reweighting, so it assumes that the event weight has 
// already been set appropriately.

// Includes
// ROOT includes
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TH1.h"

// C++ includes
#include <iostream>
#include <sstream>


void make_convenient_from_convenient(TString input="", TString outname="convenient_output.root") {
	/* Takes a list of Convenient outputs and merges them together.*/
	// Parse the input into a vector of file names
	std::string input_string(input.Data());
	std::vector<std::string> filenames;
	std::string word;
	istringstream iss(input_string, istringstream::in);
	while( iss >> word ) {
		filenames.push_back(word);
	}

	// Create a TList that all the trees will get appended to
	TList* tree_list = new TList;
	
	// For each file:
	for(std::string file_name : filenames) {
		// Open the file
		TFile* old_file = TFile::Open(file_name.c_str());

		// Extract the tree from the old file
		auto tree = old_file->Get<TTree>("generator_data");

		// Append the tree to the TList
		tree_list->Add(tree); 
	}

	// Extract just one of the fluxes, since all the files we are combining 
	// have been created with the same flux.
	TFile* another_old_file = TFile::Open(filenames[0].c_str());
	TH1D* flux = (TH1D*) another_old_file->Get("FlatTree_FLUX");
	
	// Create a new file to hold everything
	std::string outname_string(outname.Data());
	std::unique_ptr<TFile> new_file(TFile::Open(outname_string.c_str(), \
		"RECREATE"));

	// Create a new tree to hold the merged tree
	TTree* new_tree = TTree::MergeTrees(tree_list);
	new_tree->SetName("generator_data");

	// Write the merged tree to the new file
	new_tree->Write();

	// Write the flux to the new file
	flux->Write("FlatTree_FLUX");

	// Close all files
	new_file->Close();
}

