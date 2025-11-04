#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TBranch.h>
#include <iostream>
#include <cmath>


void GetAnalysisEfficiencyaN(const char* file2Name, const char* file3Name, int nL, const char* outputFileName) {

    // --- Open the base histogram file ---
    TFile* file1 = TFile::Open("aN_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "Error: Unable to open 'aN_histogram.root'!" << std::endl;
        return;
    }

    // --- Open user-specified files ---
    TFile* file2 = TFile::Open(file2Name);
    if (!file2 || file2->IsZombie()) {
        std::cerr << "Error: Unable to open analysis file: " << file2Name << std::endl;
        file1->Close();
        return;
    }

    TFile* file3 = TFile::Open(file3Name);
    if (!file3 || file3->IsZombie()) {
        std::cerr << "Error: Unable to open spectrum file: " << file3Name << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    // --- Load the nominal histogram ---
    TH1D* hist = dynamic_cast<TH1D*>(file1->Get("aN"));
    if (!hist) {
        std::cerr << "Error: Histogram 'aN' not found in aN_histogram.root!" << std::endl;
        file1->Close();
        file2->Close();
        file3->Close();
        return;
    }

    // --- Create a rebinned version of the histogram ---
    const int number_of_bins = 180000;
    double range_min = hist->GetXaxis()->GetXmin();
    double range_max = hist->GetXaxis()->GetXmax();

    TH1F* hist1 = new TH1F("hist1", "Rebinned Flux", number_of_bins, range_min, range_max);

    const double threshold = 1e-300;
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        double content = hist->GetBinContent(bin);
        if (content < threshold) content = 0;
        hist1->Fill(hist->GetBinCenter(bin), content);
    }

    double integral = hist1->Integral();

    // --- Access EventTree from the spectrum file ---
    TTree* tree = (TTree*)file3->Get("EventTree");
    if (!tree) {
        std::cerr << "Error: 'EventTree' not found in " << file3Name << std::endl;
        file1->Close();
        file2->Close();
        file3->Close();
        return;
    }

    // --- Fill deposited energy histogram ---
    TH1F* hist3 = new TH1F("hist3", "Deposited Energy Histogram", number_of_bins, range_min, range_max);
    double totalDepositedEnergy = 0;
    tree->SetBranchAddress("fTotalDepositedEnergy", &totalDepositedEnergy);

    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        hist3->Fill(totalDepositedEnergy);
    }

    double integral2 = hist3->Integral();

    // --- Access AnalysisTree from the analysis file ---
    TTree* analysisTree = (TTree*)file2->Get("AnalysisTree");
    if (!analysisTree) {
        std::cerr << "Error: 'AnalysisTree' not found in " << file2Name << std::endl;
        file1->Close();
        file2->Close();
        file3->Close();
        return;
    }

    // --- Branch setup ---
    double totalEdep = 0.0;
    double sensitiveVolumeEnergy = 0.0;
    double initialEnergy = 0.0;

    analysisTree->SetBranchAddress("hitsAnaAfterEDiffusion_energy", &totalEdep);
    analysisTree->SetBranchAddress("g4Ana_Chamber_gasVolumeEDep", &sensitiveVolumeEnergy);
    analysisTree->SetBranchAddress("g4Ana_totalEdep", &initialEnergy);

    // --- Histogram for the detected energy ---
    TH1F* hist2 = new TH1F("hist2", "Energy Distribution", number_of_bins, 0, 18);

    Long64_t entries = analysisTree->GetEntries();
    int filledEntries = 0;

    for (Long64_t i = 0; i < entries; ++i) {
        if (analysisTree->GetEntry(i) <= 0) continue;
        hist2->Fill(totalEdep);
        filledEntries++;
    }

    std::cout << "Entries processed: " << filledEntries << std::endl;

    // --- Create the resulting flux histogram ---
    TH1F* histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux", number_of_bins, 0, 18);

    for (int i = 1; i <= hist2->GetNbinsX(); ++i) {
        double binContent = hist2->GetBinContent(i);
        double value = binContent * integral / nL;
        double error = std::sqrt(binContent) * integral / nL;
        histResultingFlux->SetBinContent(i, value);
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
    file3->Close();
}
