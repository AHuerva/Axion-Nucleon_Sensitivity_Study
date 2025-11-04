#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TBranch.h>
#include <iostream>
#include <cmath>



void GetAnalysisEfficiencyPrimakoff(const char* analysisFileName, const char* histogramFileName, int nL, const char* outputFileName) {

    // --- Open the histogram file ---
    TFile* file1 = TFile::Open(histogramFileName);
    if (!file1 || file1->IsZombie()) {
        std::cerr << "Error: Unable to open histogram file: " << histogramFileName << std::endl;
        return;
    }

    // --- Open the analysis file ---
    TFile* file2 = TFile::Open(analysisFileName);
    if (!file2 || file2->IsZombie()) {
        std::cerr << "Error: Unable to open analysis file: " << analysisFileName << std::endl;
        file1->Close();
        return;
    }

    // --- Load the nominal Primakoff histogram ---
    TH1D* hist = dynamic_cast<TH1D*>(file1->Get("primakoff"));
    if (!hist) {
        std::cerr << "Error: Histogram 'primakoff' not found in " << histogramFileName << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    // --- Create a rebinned histogram ---
    const int number_of_bins = 1420;
    double range_min = hist->GetXaxis()->GetXmin();
    double range_max = hist->GetXaxis()->GetXmax();
    TH1F* hist1 = new TH1F("hist1", "Rebinned Primakoff Flux", number_of_bins, range_min, range_max);

    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        hist1->Fill(hist->GetBinCenter(bin), hist->GetBinContent(bin));
    }

    // --- Compute the total integral of the flux ---
    double integral = hist1->Integral();
    std::cout << "Nominal flux integral: " << integral << std::endl;

    // --- Access the analysis tree ---
    TTree* analysisTree = (TTree*)file2->Get("AnalysisTree");
    if (!analysisTree) {
        std::cerr << "Error: 'AnalysisTree' not found in " << analysisFileName << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    // --- Branch setup ---
    double totalEdep = 0.0;
    double initialEnergy = 0.0;
    double sensitiveVolumeEnergy = 0.0;

    analysisTree->SetBranchAddress("hitsAnaAfterEDiffusion_energy", &totalEdep);
    analysisTree->SetBranchAddress("g4Ana_totalEdep", &initialEnergy);
    analysisTree->SetBranchAddress("g4Ana_Chamber_gasVolumeEDep", &sensitiveVolumeEnergy);

    // --- Create a histogram for the detected energy ---
    TH1F* hist2 = new TH1F("hist2", "Detected Energy Distribution", number_of_bins, 0.0, 10.0);

    Long64_t entries = analysisTree->GetEntries();
    int filledEntries = 0;

    for (Long64_t i = 0; i < entries; ++i) {
        if (analysisTree->GetEntry(i) <= 0) continue;
        if (totalEdep > 0 && initialEnergy > 0 && sensitiveVolumeEnergy / initialEnergy > 0.95) {
            hist2->Fill(totalEdep);
            filledEntries++;
        }
    }

    std::cout << "Entries processed and filled: " << filledEntries << std::endl;

    // --- Create resulting flux histogram ---
    TH1F* histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux Histogram", number_of_bins, 0.0, 10.0);

    for (int i = 1; i <= hist2->GetNbinsX(); ++i) {
        double binContent1 = hist1->GetBinContent(i);
        double binContent2 = hist2->GetBinContent(i);
        double resultingFlux = 0.0;
        double error = 0.0;

        if (binContent1 > 0 && nL > 0) {
            resultingFlux = binContent2 * integral / nL;
            error = std::sqrt(binContent2) * integral / nL;
        }

        histResultingFlux->SetBinContent(i, resultingFlux);
        histResultingFlux->SetBinError(i, error);
    }

    // --- Save the resulting histogram ---
    TFile* outputFile = TFile::Open(outputFileName, "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error: Could not create output file: " << outputFileName << std::endl;
    } else {
        histResultingFlux->Write();
        outputFile->Close();
        std::cout << "Result saved to " << outputFileName << std::endl;
    }

    // --- Clean up ---
    file1->Close();
    file2->Close();
}
