// Author: Colin Weber (webe1077@umn.edu)
// Date: 11 July 2023
// Purpose: To read in a Convenient file and output the same file but 
// reweighted

// Command: root -q "make_convenient_from_convenient_reweight.C(\"input\", \"outname\", \"str_reweight\")"

// Parameters
// 	input : str, defaults to ""
// 		The CONVENIENT file to be reweighted.
// 	outname : str, defaults to "convenient_output.root"
// 		The name of the output file. Must end in ".root"
// 	str_reweight : str, defaults to 1.0
// 		The weight to apply to the events in the file.

// Outputs
// 	newfile2 : TFile*
// 		CONVENIENT file containing the events from the input but reweighted 
// 		by a constant given by str_reweight.

// Includes
// ROOT includes
#include "TTree.h" // For working with trees
#include "TFile.h" // For writing files
#include "TH1.h" // For working with 1D histograms
#include "TTreeReader.h" // For creating a TTreeReader
#include "TTreeReaderValue.h" // For using the TTreeReader to read ordinary
							  // data types
#include "TTreeReaderArray.h" // For using the TTreeReader to read arrays

// C++ includes
#include <iostream> // For writing files
#include <vector> // For using vectors

void make_convenient_from_convenient_reweight(TString input="", TString outname="convenient_output.root", std::string str_reweight="1.0") {
	// Read the reweight
	double reweight = stod(str_reweight);

	TString outname1 = "1" + outname;
	TString outname2 = "2" + outname;

	// Current working directory
	TString cwd(get_current_dir_name());
	
	// Construct input file path and name using the above
	TString old_file_str(cwd + "/" + input);

	gSystem->CopyFile(old_file_str, outname1);
		
	// Initialize a pointer-to-file that points to the old Convenient file
	TFile* old_file = new TFile(outname1, "update");

	TTree* t = (TTree*)old_file->Get("generator_data");

	double old_ew;
	double new_ew;

	TBranch* b_new_ew = t->Branch("ew", &new_ew, "ew/D");
	t->SetBranchAddress("EventWeight", &old_ew);

	int n_entries = t->GetEntries();

	for (int i = 0; i < n_entries; i++) {
		t->GetEntry(i);
		new_ew = old_ew * reweight;

		b_new_ew->Fill();
	}

	t->Write();
	delete old_file;

	TFile oldfile(outname1);
  	TTree *oldtree;
	oldfile.GetObject("generator_data", oldtree);
	TH1D* flux; 
	oldfile.GetObject("FlatTree_FLUX", flux);

	// Activate all branches
	oldtree->SetBranchStatus("*", 1);

  	// Deactivate one branch
  	oldtree->SetBranchStatus("EventWeight", 0);

   // Create a new file + a clone of old tree in new file
	TFile newfile(outname2, "recreate");
	auto newtree = oldtree->CloneTree(-1, "fast");
	flux->Write("FlatTree_FLUX");
	newfile.Write();
	newfile.Close();

	// Now reverse it
	// Initialize a pointer-to-file that points to the old Convenient file
	TFile* old_file2 = new TFile(outname2, "update");

	TTree* t2 = (TTree*)old_file2->Get("generator_data");

	double old_ew2;
	double new_ew2;

	TBranch* b_new_ew2 = t2->Branch("EventWeight", &new_ew2, "ew/D");
	t2->SetBranchAddress("ew", &old_ew2);

	int n_entries2 = t2->GetEntries();

	for (int i = 0; i < n_entries2; i++) {
		t2->GetEntry(i);
		new_ew2 = old_ew2;

		b_new_ew2->Fill();
	}
	t2->Write();
	delete old_file2;

	TFile oldfile2(outname2);
  	TTree* oldtree2;
	oldfile2.GetObject("generator_data", oldtree2);
	TH1D* flux2; 
	oldfile2.GetObject("FlatTree_FLUX", flux2);

	// Activate all branches
	oldtree2->SetBranchStatus("*", 1);

  	// Deactivate one branch
  	oldtree2->SetBranchStatus("ew", 0);

   // Create a new file + a clone of old tree in new file
	TFile newfile2(outname, "recreate");
	auto newtree2 = oldtree2->CloneTree(-1, "fast");
	flux2->Write("FlatTree_FLUX");
	newfile2.Write();
	newfile2.Close();

	gSystem->Exec(Form("rm -f %s", outname1.Data()));
	gSystem->Exec(Form("rm -f %s", outname2.Data()));
}
//
//
// Save for now, in the event that we bring back interaction types
/*	bool flagCCINC;
	old_tree->SetBranchAddress("flagCCINC", &flagCCINC);
	bool flagNCINC;
	old_tree->SetBranchAddress("flagNCINC", &flagNCINC);

	bool flagCCQELike;
	old_tree->SetBranchAddress("flagCCQELike", &flagCCQELike);

	bool flagCCQE;
	old_tree->SetBranchAddress("flagCCQE", &flagCCQE);

	bool flagNCEL;
	old_tree->SetBranchAddress("flagNCEL", &flagNCEL);

	bool flagCCcoh;
	old_tree->SetBranchAddress("flagCCcoh", &flagCCcoh);

	bool flagNCcoh;
	old_tree->SetBranchAddress("flagNCcoh", &flagNCcoh);

	int Mode;
	old_tree->SetBranchAddress("Mode", &Mode);
	std::vector<int> res_mode_list {17, 22, 23, 38, 39, 42, 43, 44, 45};
	std::vector<int> dis_mode_list {46};*/
//
/*  std::cout << flagCCINC << std::endl;
	std::cout << "Mode: " << Mode << "\n" << std::endl;
	if(flagCCINC) {
		if(flagCCQE) int_type = 10;
		else if(flagCCQELike && ~flagCCQE) int_type = 11;
		else if(std::find(res_mode_list.begin(), res_mode_list.end(), Mode) != res_mode_list.end()) int_type = 12;
		else if(std::find(dis_mode_list.begin(), dis_mode_list.end(), Mode) != dis_mode_list.end()) int_type = 13;
		else if(flagCCcoh) int_type = 14;
		else int_type = 19;
	}
	else if(flagNCINC) {
		if(flagNCEL) int_type = 20;
		// No NC MEC possible b/c not implemented in NEUT
		else if(std::find(res_mode_list.begin(), res_mode_list.end(), Mode) != res_mode_list.end()) int_type = 22;
		else if(std::find(dis_mode_list.begin(), dis_mode_list.end(), Mode) != dis_mode_list.end()) int_type = 23;
		else if(flagNCcoh) int_type = 24;
		else int_type = 29;
	}
	else {
		throw runtime_error(Form("Encountered interaction type in \
			Event %d that is neither CC or NC.", i));
	} */
//
