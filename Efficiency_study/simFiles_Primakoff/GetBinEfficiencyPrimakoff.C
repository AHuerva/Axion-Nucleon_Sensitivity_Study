#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <iostream>
#include <string>
#include <cmath>

void GetBinEfficiencyPrimakoff(std::string file2_name = "", int nL = 0, std::string output_name = "") {

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

    // --- Open the nominal histogram file ---
    TFile *file1 = TFile::Open("Primakoff_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "Error: Unable to open histogram file 'Primakoff_histogram.root'!" << std::endl;
        return;
    }

    // --- Open the input file (user provided) ---
    TFile *file2 = TFile::Open(file2_name.c_str());
    if (!file2 || file2->IsZombie()) {
        std::cerr << "Error: Unable to open spectrum file " << file2_name << "!" << std::endl;
        file1->Close();
        return;
    }

    // --- Load the Primakoff histogram ---
    TH1D *hist = dynamic_cast<TH1D*>(file1->Get("primakoff"));
    if (!hist) {
        std::cerr << "Error: Histogram 'primakoff' not found!" << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    // --- Create a rebinned histogram ---
    int number_of_bins = 1420;
    float range_min = hist->GetXaxis()->GetXmin();
    float range_max = hist->GetXaxis()->GetXmax();
    TH1F *hist1 = new TH1F("hist1", "Histogram with new bins", number_of_bins, range_min, range_max);

    // --- Fill the new histogram ---
    const double threshold = 1e-10;
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        double binContent = hist->GetBinContent(bin);
        if (binContent > threshold) {
            hist1->Fill(hist->GetBinCenter(bin), binContent);
            std::cout << "Bin " << bin << ": " << binContent << std::endl;
        } else {
            hist1->Fill(hist->GetBinCenter(bin), 0);
        }
    }

    // --- Compute integral ---
    int nBins = hist1->GetNbinsX();
    double integral = 0;
    for (int i = 1; i <= nBins; ++i) {
        integral += hist1->GetBinContent(i);
    }

    std::cout << "Integral of the nominal flux: " << integral << std::endl;

    // --- Access EventTree ---
    TTree *tree = (TTree*)file2->Get("EventTree");
    if (!tree) {
        std::cerr << "Error: TTree 'EventTree' not found!" << std::endl;
        file2->Close();
        file1->Close();
        return;
    }

    // --- Create a histogram for deposited energy ---
    int range_min2 = 0;
    int range_max2 = 10;
    TH1F *hist2 = new TH1F("hist2", "Deposited Energy Histogram", number_of_bins, range_min2, range_max2);

    // --- Define and link branches ---
    double totalDepositedEnergy = 0;
    double sensitiveVolumeEnergy = 0;

    tree->SetBranchAddress("fTotalDepositedEnergy", &totalDepositedEnergy);
    tree->SetBranchAddress("fSensitiveVolumeEnergy", &sensitiveVolumeEnergy);

    // --- Loop over entries and apply cuts ---
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        if (totalDepositedEnergy > 0 && sensitiveVolumeEnergy / totalDepositedEnergy > 0.95) {
            hist2->Fill(totalDepositedEnergy);
        }
    }

    // --- Create the resulting flux histogram ---
    TH1F *histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux Histogram", number_of_bins, range_min2, range_max2);

    // --- Compute bin-by-bin efficiency and rescaled histogram ---
    int nBins2 = hist2->GetNbinsX();
    for (int i = 1; i <= nBins2; ++i) {
        double binContent = hist1->GetBinContent(i);
        double binContent2 = hist2->GetBinContent(i);

        double resultingFlux = 0;
        if (binContent != 0)
            resultingFlux = binContent * binContent2 / (binContent / integral * nL);

        double error = (binContent2 > 0) ? sqrt(binContent2) / nL * integral : 1e-5;

        histResultingFlux->SetBinContent(i, resultingFlux);
        histResultingFlux->SetBinError(i, error);
    }

    // --- Save the resulting flux histogram ---
    TFile *output_file = TFile::Open(output_name.c_str(), "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "Error opening output file: " << output_name << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    output_file->cd();
    histResultingFlux->Write();
    output_file->Close();

    std::cout << "Output saved as: " << output_name << std::endl;

    // --- Clean up ---
    file1->Close();
    file2->Close();
}
