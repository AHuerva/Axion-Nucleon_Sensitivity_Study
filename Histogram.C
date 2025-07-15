#include <TRestGeant4Event.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TH1D.h>
#include <iostream>

void Histogram() {
    // List of .root files to process
    const char* filenames[] = {
        "Sim5.1.root",
        "Sim5.2.root"
    };
    const int numFiles = sizeof(filenames) / sizeof(filenames[0]);

    // Histogram settings
    double range_min = 0;
    double range_max = 20;
    int number_of_bins = 60;

    // Create cumulative histograms
    TH1D* hist = new TH1D("Background", "Particle Energy; Energy (keV); Counts", number_of_bins, range_min, range_max);
    

    // Loop over each file
    for (int f = 0; f < numFiles; ++f) {
        const char* filename = filenames[f];
        std::cout << "Processing file: " << filename << std::endl;

        TFile* file = TFile::Open(filename);
        if (!file || file->IsZombie()) {
            std::cerr << "Could not open file: " << filename << std::endl;
            continue;
        }

        TTree* tree = (TTree*)file->Get("EventTree");
        if (!tree) {
            std::cerr << "EventTree not found in: " << filename << std::endl;
            file->Close();
            continue;
        }

        TRestGeant4Event* event = nullptr;
        tree->SetBranchAddress("TRestGeant4EventBranch", &event);

        for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
            tree->GetEntry(i);
            if (!event) continue;

            TString particleType = event->GetPrimaryEventParticleName();
            double totalDepositedEnergy = event->GetTotalDepositedEnergy();

            
            hist->Fill(totalDepositedEnergy / 1000.);
                
        }

        file->Close();
    }

    TFile* out = TFile::Open("Background6cm10LeadArgon2.0barNeutronsandGammas.root", "RECREATE");
    hist->Write();
    out->Close();
}
