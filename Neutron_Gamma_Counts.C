#include <TRestGeant4Event.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>

void Neutron_Gamma_Counts(const std::string &filePattern) {
  
    auto files = TRestTools::GetFilesMatchingPattern(filePattern);
    
    // Define variables
    int gammas=0; 
    int gamma_prim=0;
    int gamma_aN=0;
    int gamma_0_10=0;
    int gamma_0_20=0;
    int neutrons=0;
    int neutron_prim=0;
    int neutron_aN=0;
    int neutron_0_10=0;
    int neutron_0_20=0;
    int Generated_events=0;

    
    for (const auto &fileName : files) {
        std::cout << "Processing file: " << fileName << std::endl;

        TFile* file = TFile::Open(fileName.c_str());
        if (!file || file->IsZombie()) {
            std::cerr << "Could not open file: " << fileName << std::endl;
            continue;
        }
        
        auto meta = (TRestGeant4Metadata*) file->Get("defaultTRestGeant4Metadata");
        Generated_events+=meta->GetNumberOfEvents();
        //meta->Print();
    
    
    // Access the EventTree from file
    TTree* tree = (TTree*)file->Get("EventTree");
        if (!tree) {
            std::cerr << "EventTree not found in: " << fileName << std::endl;
            file->Close();
            continue;
        }
    
    
    
    
    
    
    //Define variables for the loop to discard events
    TRestGeant4Event* event = nullptr;    
    tree->SetBranchAddress("TRestGeant4EventBranch", &event);
    
    
    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);


    if (!event) {
        std::cerr << "Event object is null" << std::endl;
        return;
    }

    TString particleType = event->GetPrimaryEventParticleName();
    double totalDepositedEnergy = event->GetTotalDepositedEnergy();

    if (particleType == "gamma") {
        gammas+=1;
        if (totalDepositedEnergy < 10000){
        gamma_0_10+=1;
        }
        if (totalDepositedEnergy > 2000 && totalDepositedEnergy < 7000){
        gamma_prim+=1;
        }
        if (totalDepositedEnergy > 12000 && totalDepositedEnergy < 17000){
        gamma_aN+=1;
        }
        if (totalDepositedEnergy < 20000){
        gamma_0_20+=1;
        }
    } else if (particleType == "neutron") {
        neutrons+=1;
        if (totalDepositedEnergy < 10000){
        neutron_0_10+=1;
        }
        if (totalDepositedEnergy > 2000 && totalDepositedEnergy < 7000){
        neutron_prim+=1;
        }
        if (totalDepositedEnergy > 12000 && totalDepositedEnergy < 17000){
        neutron_aN+=1;
        }
        if (totalDepositedEnergy < 20000){
        neutron_0_20+=1;
        }
    } else {
        std::cout << "Event"<< i<< ": is another type of event: " << particleType.Data() << std::endl;
    }
    
    }
    
    file->Close();
    
    }
    
    std::cout << "Number of generated events:   "<< Generated_events << std::endl;
    std::cout << "Number of gammas:   "<< gammas << std::endl;
    std::cout << "Number of neutrons:   "<< neutrons << std::endl;
    std::cout << "Number of gammas in range 0-10 keV:   "<< gamma_0_10 << std::endl;
    std::cout << "Number of gammas in range 2-7 keV (primakoff):   "<< gamma_prim << std::endl;
    std::cout << "Number of gammas in range 12-17 keV (aN):   "<< gamma_aN << std::endl;
    std::cout << "Number of gammas in range 0-20 keV (axions):   "<< gamma_0_20 << std::endl;
    std::cout << "Number of neutrons in range 0-10 keV:   "<< neutron_0_10 << std::endl;
    std::cout << "Number of neutrons in range 2-7 keV (primakoff):   "<< neutron_prim << std::endl;
    std::cout << "Number of neutrons in range 12-17 keV (aN):   "<< neutron_aN << std::endl;
    std::cout << "Number of neutrons in range 0-20 keV (axions):   "<< neutron_0_20 << std::endl;

    
   
    
}
    
