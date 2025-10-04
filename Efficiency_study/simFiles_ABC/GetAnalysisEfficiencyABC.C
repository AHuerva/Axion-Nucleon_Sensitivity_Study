#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TH1.h>

void GetAnalysisEfficiencyABC() {

    // Open the files
    TFile *file1 = TFile::Open("ABC_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "An error has occurred while opening histogram file!" << std::endl;
        return;
    }
    
    TFile *file2 = TFile::Open("Analysis_R00060_2.0bar_ABC-histogram_IAXO-D1_6cmDrift_v2.4.3_signals.root");
    if (!file2 || file2->IsZombie()) {
        std::cerr << "An error has occurred while opening spectrum file!" << std::endl;
        return;
    }
    
    
    // Load the histogram
    TH1D *hist = dynamic_cast<TH1D*>(file1->Get("ABC"));
    if (!hist) {
        std::cerr << "Error: Histogram not found!" << std::endl;
        file1->Close();
        return;
    }
    
    // Create a new histogram with the new number of bins
    int number_of_bins = 1800;
    float range_min = hist->GetXaxis()->GetXmin();
    float range_max = hist->GetXaxis()->GetXmax();
    TH1F *hist1 = new TH1F("hist1", "Histogram with new bins", number_of_bins, range_min, range_max);

    // Fill the new histogram with data from the original histogram
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        hist1->Fill(hist->GetBinCenter(bin), hist->GetBinContent(bin));
    } 

    
    // Get bin-by-bin information
    int nBins = hist1->GetNbinsX();
    double integral = 0;
    for (int i = 1; i <= nBins; ++i) {
        double binContent = hist1->GetBinContent(i);
        integral += binContent;        
    }
    
    // Save the Rebinned Flux histogram to a new file
//    TFile *output_flux = TFile::Open("Rebinned_ABC.root", "RECREATE");
//    if (!output_flux || output_flux->IsZombie()) {
//        std::cerr << "Error opening the rebinned flux file." << std::endl;
//        file1->Close();
//        file2->Close();
//        return;
//    }
    
//    output_flux->cd();
//    hist1->Write(); // Write the new histogram
//    output_flux->Close();
    
    
    // Check if the file was opened correctly
    if (file2 && !file2->IsZombie()) {
        // Access the 'AnalysisTree' from the file
        TRestAnalysisTree *analysisTree = (TRestAnalysisTree*)file2->Get("AnalysisTree");

        // Check if the AnalysisTree is valid
        if (analysisTree) {
            std::cout << "'AnalysisTree' found." << std::endl;
        }

        // Access the branch 'hitsAnaAfterSmear_energy' (which contains TLeafD)
        TBranch* branch = analysisTree->GetBranch("hitsAnaAfterEDiffusion_energy");
        TBranch* branch2 = analysisTree->GetBranch("g4Ana_Chamber_gasVolumeEDep");
        TBranch* branch3 = analysisTree->GetBranch("g4Ana_totalEdep");
        

        if (branch && branch2 && branch3) {
            std::cout << "Branch 'hitsAnaAfterEDiffusion_energy' found." << std::endl;
            std::cout << "Branch 'g4Ana_Chamber_gasVolumeEDep' found." << std::endl;
            std::cout << "Branch 'g4Ana_totalEdep' found." << std::endl;

            // Declare a variable to store the energy value from the branch
            double totalEdep = 0.0;
            branch->SetAddress(&totalEdep);  // Set the address of the branch to the variable
            double initialEnergy=0.0;
            branch3->SetAddress(&initialEnergy);
            double sensitiveVolumeEnergy=0.0;
            branch2->SetAddress(&sensitiveVolumeEnergy);

            // Create a histogram to store energy values
            int nBins = 1800;  // Number of bins
            double xMin = 0.0;  // Minimum energy value
            double xMax = 12.0;  // Maximum energy value (adjust according to your data range)
            TH1F *hist2 = new TH1F("hist2", "Energy Distribution", nBins, xMin, xMax);

            // Check if the branch has entries
            if (branch->GetEntries() > 0) {
                int entriesFilled = 0;  // Count how many entries we are filling

                for (int i = 0; i < branch->GetEntries(); i++) {
                    int status = branch->GetEntry(i);  // Get data from the current entry
                    int status2 = branch2->GetEntry(i);
                    int status3 = branch3->GetEntry(i);

                    if (status <= 0) {
                        std::cerr << "Error: Failed to get entry " << i << " (status " << status << "). Skipping this entry." << std::endl;
                        continue;  // Skip the current iteration if the entry cannot be retrieved
                    }
            if (totalEdep > 0 && sensitiveVolumeEnergy / initialEnergy > 0.95) {
                    // Fill the histogram with the energy value
                    hist2->Fill(totalEdep);  // Fill the histogram with the energy value
                    entriesFilled++;
                    }
                }
            std::cout << entriesFilled << std::endl;
            }
            
   
    // Create a histogram for ResultingFlux
    TH1F *histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux Histogram", number_of_bins, xMin, xMax);

    
    
    // Get bin-by-bin efficiency and reescalated histogram
    int nL=344870;
    int nBins2 = hist2->GetNbinsX();
    for (int i = 1; i <= nBins2; ++i) {
        double binContent = hist1->GetBinContent(i);
        double binContent2 = hist2->GetBinContent(i);
        double ResultingFlux=binContent*binContent2/(binContent / integral * nL);
        if (binContent==0) {
          ResultingFlux=0;
        }
        double error=sqrt(binContent2)/nL*integral;
        if (binContent2==0){
          error=10e-6;
        }
        histResultingFlux->SetBinContent(i, ResultingFlux); // Fill the new histogram
        histResultingFlux->SetBinError(i, error);
        //std::cerr << error << std::endl;
               
    }
    
    // Save the ResultingFlux histogram to a new file
    TFile *output_file = TFile::Open("AnalysisABC6cmXenon2.0bar.root", "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "Error opening the output file." << std::endl;
        file1->Close();
        file2->Close();
        return;
    }
    
    output_file->cd();
    histResultingFlux->Write(); // Write the new histogram
    output_file->Close();
    
      }
    }
    
    
    
    
    file1->Close();
    file2->Close();
    
}