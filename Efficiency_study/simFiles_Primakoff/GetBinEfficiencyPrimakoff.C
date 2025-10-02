void GetBinEfficiencyPrimakoff() {

    // Open the files
    TFile *file1 = TFile::Open("Primakoff_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "An error has occurred while opening histogram file!" << std::endl;
        return;
    }
    
    TFile *file2 = TFile::Open("RestG4_run00048_2.0bar_Primakoff-histogram_XeNeISO_6cmDrift_v2.4.3.root");
    if (!file2 || file2->IsZombie()) {
        std::cerr << "An error has occurred while opening spectrum file!" << std::endl;
        return;
    }
    
    // Load the histogram from file1
    TH1D *hist = dynamic_cast<TH1D*>(file1->Get("primakoff"));
    if (!hist) {
        std::cerr << "Error: Histogram not found!" << std::endl;
        file1->Close();
        return;
    }

    // Create a new histogram with the desired number of bins
    int number_of_bins = 1420;
    float range_min = hist->GetXaxis()->GetXmin();
    float range_max = hist->GetXaxis()->GetXmax();
    TH1F *hist1 = new TH1F("hist1", "Histogram with new bins", number_of_bins, range_min, range_max);

    // Fill the new histogram with data from the original histogram
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        hist1->Fill(hist->GetBinCenter(bin), hist->GetBinContent(bin));
    }

    // Get bin-by-bin information from hist1
    int nBins = hist1->GetNbinsX();
    double integral = 0;
    for (int i = 1; i <= nBins; ++i) {
        double binContent = hist1->GetBinContent(i);
        integral += binContent;        
    }
    
    // Access the EventTree from file2
    TTree *tree = (TTree*)file2->Get("EventTree");
    if (!tree) {
        std::cerr << "TTree not found!" << std::endl;
        file2->Close();
        return;
    }
    
    // Create the histogram for deposited energy (hist2)
    int range_min2 = 0;
    int range_max2 = 10;
    TH1F *hist2 = new TH1F("hist2", "Deposited Energy Histogram", number_of_bins, range_min2, range_max2);

    
    // Define variables for branches
    double totalDepositedEnergy;
    double initialEnergy;
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
   
    // Create the resulting flux histogram (histResultingFlux)
    TH1F *histResultingFlux = new TH1F("histResultingFlux", "Resulting Flux Histogram", number_of_bins, range_min2, range_max2);
    
    // Calculate efficiency and rescale the resulting flux histogram
    int nL = 226887;
    int nBins2 = hist2->GetNbinsX();
    for (int i = 1; i <= nBins2; ++i) {
        double binContent = hist1->GetBinContent(i);
        double binContent2 = hist2->GetBinContent(i);
        double resultingFlux = binContent * binContent2 / (binContent / integral * nL);
        double error = sqrt(binContent2) / nL * integral;
        histResultingFlux->SetBinContent(i, resultingFlux);
        histResultingFlux->SetBinError(i, error);
    }
    
    // Save the resulting flux histogram to a new file
    TFile *output_file = TFile::Open("Primakoff6cmXenon2.0bar.root", "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        std::cerr << "Error opening the output file." << std::endl;
        file1->Close();
        file2->Close();
        return;
    }
    
    output_file->cd();
    histResultingFlux->Write(); // Write the resulting flux histogram to the output file
    output_file->Close();

    // Close input files
    file1->Close();
    file2->Close();
}
