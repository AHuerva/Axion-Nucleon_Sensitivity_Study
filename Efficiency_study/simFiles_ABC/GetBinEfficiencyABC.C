#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TH1.h>
#include <cmath>
#include <string>

void GetBinEfficiencyABC(std::string file2_name = "", int nL = 0, std::string output_name = "") {

    // Ask for parameters if not provided
    if (file2_name.empty()) {
        std::cout << "Enter the name of the spectrum ROOT file: ";
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

    // Open histogram file
    TFile *file1 = TFile::Open("ABC_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "An error has occurred while opening histogram file!" << std::endl;
        return;
    }

    // Open spectrum file
    TFile *file2 = TFile::Open(file2_name.c_str());
    if (!file2 || file2->IsZombie()) {
        std::cerr << "An error has occurred while opening spectrum file!" << std::endl;
        return;
    }

    // Load the histogram from file1
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
    const double threshold = 1e-10;
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        double binContent = hist->GetBinContent(bin);
        if (binContent > threshold) {
            hist1->Fill(hist->GetBinCenter(bin), binContent);
        } else {
            hist1->Fill(hist->GetBinCenter(bin), 0);
        }
    }

    // Compute integral
    int nBins = hist1->GetNbinsX();
    double integral = 0;
    for (int i = 1; i <= nBins; ++i) {
        integral += hist1->GetBinContent(i);
    }

    // Access the EventTree from file2
    TTree *tree = (TTree*)file2->Get("EventTree");
    if (!tree) {
        std::cerr << "TTree not found!" << std::endl;
        file2->Close();
        return;
    }

    // Create a histogram for deposited energy
    int range_min2 = 0;
    int range_max2 = 12;
    TH1F *hist2 = new TH1F("hist2", "Deposited Energy Histogram", number_of_bins, range_min2, range_max2);

    // Branch variables
    double totalDepositedEnergy;
    double sensitiveVolumeEnergy;

    tree->SetBranchAddress("fTotalDepositedEnergy", &totalDepositedEnergy);
    tree->SetBranchAddress("fSensitiveVolumeEnergy", &sensitiveVolumeEnergy);

    // Fill hist2 with energy cut
    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        if (totalDepositedEnergy > 0 && sensitiveVolumeEnergy / totalDepositedEnergy > 0.95) {
            hist2->Fill(totalDepositedEnergy);
        }
    }

    // Create histogram for ResultingFlux
    TH1F *histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux Histogram", number_of_bins, range_min2, range_max2);

    // Compute efficiency and errors
    int nBins2 = hist2->GetNbinsX();
    for (int i = 1; i <= nBins2; ++i) {
        double binContent = hist1->GetBinContent(i);
        double binContent2 = hist2->GetBinContent(i);
        double ResultingFlux = 0;

        if (binContent > threshold) {
            ResultingFlux = binContent * binContent2 / (binContent / integral * nL);
        }

        double error = (binContent2 > 0) ? sqrt(binContent2) / nL * integral : 1e-6;

        histResultingFlux->SetBinContent(i, ResultingFlux);
        histResultingFlux->SetBinError(i, error);
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
    histResultingFlux->Write();
    output_file->Close();

    file1->Close();
    file2->Close();

    std::cout << "Output saved as: " << output_name << std::endl;
}
