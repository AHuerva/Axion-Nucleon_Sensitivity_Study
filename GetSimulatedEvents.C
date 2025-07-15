#include <TFile.h>
#include <iostream>
#include "TRestGeant4Metadata.h"

void GetSimulatedEvents(const char* filename) {
    TFile* file = TFile::Open(filename);
    if (!file || file->IsZombie()) {
        std::cerr << "Could not open file." << std::endl;
        return;
    }
    

    auto meta = (TRestGeant4Metadata*) file->Get("defaultTRestGeant4Metadata");
    meta->Print();

    
    file->Close();
}


