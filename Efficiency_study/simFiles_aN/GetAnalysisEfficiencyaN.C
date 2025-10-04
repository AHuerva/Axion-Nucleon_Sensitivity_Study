#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TH1.h>

void GetAnalysisEfficiencyaN() {

    // Open the files
    TFile *file1 = TFile::Open("aN_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "An error has occurred while opening histogram file!" << std::endl;
        return;
    }
    
    TFile *file2 = TFile::Open("Analysis_R00085_1.2bar_aN-histogram_IAXO-D1_3cmDrift_v2.4.3_signals.root");
    if (!file2 || file2->IsZombie()) {
        std::cerr << "An error has occurred while opening analysis file!" << std::endl;
        return;
    }
    
    TFile *file3 = TFile::Open("RestG4_run00085_1.2bar_aN-histogram_Argon1ISO_3cmDrift_v2.4.3.root");
    if (!file3 || file3->IsZombie()) {
        std::cerr << "An error has occurred while opening spectrum file!" << std::endl;
        return;
    }
    
    
    // Load the histogram
    TH1D *hist = dynamic_cast<TH1D*>(file1->Get("aN"));
    if (!hist) {
        std::cerr << "Error: Histogram not found!" << std::endl;
        file1->Close();
        return;
    }


    
    // Create a new histogram with the new number of bins
    int number_of_bins = 180000;
    float range_min = hist->GetXaxis()->GetXmin();
    float range_max = hist->GetXaxis()->GetXmax();
    TH1F *hist1 = new TH1F("hist1", "Histogram with new bins", number_of_bins, range_min, range_max);

    // Fill the new histogram with data from the original histogram
        const double threshold = 1e-300; 
for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
    double binContent = hist->GetBinContent(bin);
    if (binContent > threshold) {
        hist1->Fill(hist->GetBinCenter(bin), hist->GetBinContent(bin));
        //std::cout << "Bin " << bin << ": " << binContent << std::endl;
    }
    else{
    binContent=0;
    hist1->Fill(hist->GetBinCenter(bin), binContent);
    }
}

    
    // Get bin-by-bin information
    int nBins = hist1->GetNbinsX();
    double integral = 0;
    for (int i = 1; i <= nBins; ++i) {
        double binContent = hist1->GetBinContent(i);
        integral += binContent;   
    }
    //std::cout << integral << std::endl;
    
    // Save the Rebinned Flux histogram to a new file
//    TFile *output_flux = TFile::Open("Rebinned_aN.root", "RECREATE");
//    if (!output_flux || output_flux->IsZombie()) {
//        std::cerr << "Error opening the rebinned flux file." << std::endl;
//        file1->Close();
//        file2->Close();
//        return;
//    }
    
//    output_flux->cd();
//    hist1->Write(); // Write the new histogram
//    output_flux->Close();
    
    
    
    // Search the tree
    TTree *tree = (TTree*)file3->Get("EventTree");
    if (!tree) {
        std::cerr << "TTree not found!" << std::endl;
        file3->Close();
        return;
    }
;
    
    // Create a histogram with specified number of bins
    int range_min2=range_min;
    int range_max2=range_max;
    TH1F *hist3 = new TH1F("hist3", "Deposited Energy Histogram", number_of_bins, range_min2, range_max2);

    // Define variables for branches
    double totalDepositedEnergy;
    
    // Associate the branches with the variables
    tree->SetBranchAddress("fTotalDepositedEnergy", &totalDepositedEnergy);
    
    // Loop over the tree entries and apply the energy cut
    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        hist3->Fill(totalDepositedEnergy); 
    }
    
    // Get bin-by-bin information
    int nBins3 = hist3->GetNbinsX();
    double integral2 = 0;
    for (int i = 1; i <= nBins3; ++i) {
        double binContent3 = hist3->GetBinContent(i);
        integral2 += binContent3;   
    }
    //std::cout << integral2 << std::endl;
    
    
    
    
    
    
    
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
            int nBins = number_of_bins;  // Number of bins
            float xMin = 0;  // Minimum energy value
            float xMax = 18;  // Maximum energy value (adjust according to your data range)
            TH1F *hist2 = new TH1F("hist2", "Energy Distribution", nBins, xMin, xMax);

            // Check if the branch has entries
            if (branch->GetEntries() > 0) {
                int entriesFilled = 0;  // Count how many entries we are filling

                for (int i = 0; i < branch->GetEntries(); i++) {
                    int status = branch->GetEntry(i);  // Get data from the current entry
                    int status2 = branch2->GetEntry(i);
                    int status3 = branch3->GetEntry(i);
                    
                    if (status <= 0 || status2 <= 0 || status3 <= 0) {
    std::cerr << "Error: Failed to get entry " << i << " for one or more branches. Skipping this entry." << std::endl;
    continue;
}

                    if (status <= 0) {
                        std::cerr << "Error: Failed to get entry " << i << " (status " << status << "). Skipping this entry." << std::endl;
                        continue;  // Skip the current iteration if the entry cannot be retrieved
                    }
            //if (totalEdep > 0 && sensitiveVolumeEnergy / initialEnergy > 0.95) {
                    // Fill the histogram with the energy value
                    hist2->Fill(totalEdep);  // Fill the histogram with the energy value
                    entriesFilled++;
                    //}
                }
            std::cout << entriesFilled << std::endl;
            }
   
    // Create a histogram for ResultingFlux
    TH1F *histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux Histogram", number_of_bins, xMin, xMax);

  //double factor=integral/integral2;
  //std::cout << factor << std::endl;

    
    
    // Get bin-by-bin efficiency and reescalated histogram
    int nBins2 = hist2->GetNbinsX();
    int nL=1517113;
    for (int i = 1; i <= nBins2; ++i) {
        double binContent2 = hist2->GetBinContent(i);
        //std::cout << binContent2 << std::endl;
        double ResultingFlux=binContent2*integral/nL;
        //double ResultingFlux=binContent2;
        
        //std::cout << ResultingFlux << std::endl;
        double error=sqrt(binContent2)*integral/nL;
        //double error=0;        
        histResultingFlux->SetBinContent(i, ResultingFlux); // Fill the new histogram
        histResultingFlux->SetBinError(i, error);
        //std::cerr << error << std::endl;
        
               
    }
    
    // Save the ResultingFlux histogram to a new file
    TFile *output_file = TFile::Open("AnalysisaN3cmArgon1.2bar.root", "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "Error opening the output file." << std::endl;
        file1->Close();
        file2->Close();
        file3->Close();
        return;
    }
    
    output_file->cd();
    histResultingFlux->Write(); // Write the new histogram
    output_file->Close();
    
    }
}
    
    
    
    file1->Close();
    file2->Close();
    file3->Close();
    
}