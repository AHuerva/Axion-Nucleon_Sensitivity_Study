#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <iostream>
#include <string>
#include <cmath>


void GetBinEfficiencyaN(std::string file2_name = "", int nL = 0, std::string output_name = "") {

    // Ask for parameters if not provided
    if (file2_name.empty()) {
        std::cout << "Enter the name of the ROOT file: ";
        std::cin >> file2_name;
    }
    if (nL == 0) {
        std::cout << "Enter the value of nL: ";
        std::cin >> nL;
    }
    if (output_name.empty()) {
        std::cout << "Enter the name of the output ROOT file: ";
        std::cin >> output_name;
    }
    // Open the files
    TFile *file1 = TFile::Open("aN_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "An error has occurred while opening histogram file!" << std::endl;
        return;
    }
    
    TFile *file2 = TFile::Open(file2_name.c_str());
    if (!file2 || file2->IsZombie()) {
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
    int number_of_bins = 160000;
    float range_min = hist->GetXaxis()->GetXmin();
    float range_max = hist->GetXaxis()->GetXmax();
    TH1F *hist1 = new TH1F("hist1", "Histogram with new bins", number_of_bins, range_min, range_max);

    // Fill the new histogram with data from the original histogram
        const double threshold = 1e-10; 
for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
    double binContent = hist->GetBinContent(bin);
    if (binContent > threshold) {
        hist1->Fill(hist->GetBinCenter(bin), hist->GetBinContent(bin));
        std::cout << "Bin " << bin << ": " << binContent << std::endl;
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
    
    
    
    
    // Search the tree
    TTree *tree = (TTree*)file2->Get("EventTree");
    if (!tree) {
        std::cerr << "TTree not found!" << std::endl;
        file2->Close();
        return;
    }
;
    
    // Create histogram for deposited energy
    TH1F *hist2 = new TH1F("hist3", "Deposited Energy Histogram", number_of_bins, range_min, range_max);

    // Define variables for branches
    double totalDepositedEnergy;
    double sensitiveVolumeEnergy;
    
    // Associate the branches with the variables
    tree->SetBranchAddress("fTotalDepositedEnergy", &totalDepositedEnergy);
    tree->SetBranchAddress("fSensitiveVolumeEnergy", &sensitiveVolumeEnergy);
    
    // Loop over the tree entries and apply the energy cut
    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        // Apply the cut
        if (totalDepositedEnergy > 0 && sensitiveVolumeEnergy / totalDepositedEnergy > 0.95) {
            hist2->Fill(totalDepositedEnergy);  // Fill the histogram if the condition is met
        }
    }
   
    // Create a histogram for ResultingFlux
    TH1F *histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux Histogram", number_of_bins, range_min, range_max);

    
    
    // Get bin-by-bin efficiency and reescalated histogram
    int nBins2 = hist2->GetNbinsX();
    for (int i = 1; i <= nBins2; ++i) {
        double binContent = hist1->GetBinContent(i);
        double binContent2 = hist2->GetBinContent(i);

if (binContent==0) {
          ResultingFlux=0;
        }
        double ResultingFlux=binContent*binContent2/(binContent / integral * nL);
        
        double error=sqrt(binContent2)/nL*integral;
        histResultingFlux->SetBinContent(i, ResultingFlux); // Fill the new histogram
        histResultingFlux->SetBinError(i, error);
        //std::cerr << error << std::endl;
               
    }
    
    // Save the ResultingFlux histogram to a new file
    TFile *output_file = TFile::Open(output_name.c_str(), "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "Error opening the output file." << std::endl;
        file1->Close();
        file2->Close();
        return;
    }
    
    output_file->cd();
    histResultingFlux->Write(); // Write the new histogram
    output_file->Close();
    

    
    file1->Close();
    file2->Close();

std::cout << "Output saved as: " << output_name << std::endl;
    
}
