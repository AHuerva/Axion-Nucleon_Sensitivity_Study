void GetEfficiencyaN(const char* filename){
   
    // Open the files
    TFile *file1 = TFile::Open("aN_histogram.root");
    if (!file1 || file1->IsZombie()) {
        std::cerr << "An error has occurred while opening histogram file!" << std::endl;
        return;
    }
    
    TFile *file2 = TFile::Open(filename);
    if (!file2 || file2->IsZombie()) {
        std::cerr << "An error has occurred while opening spectrum file!" << std::endl;
        return;
    }
    
    // Load the histogram from file1
    TH1D *hist = dynamic_cast<TH1D*>(file1->Get("aN"));
    if (!hist) {
        std::cerr << "Error: Histogram not found!" << std::endl;
        file1->Close();
        return;
    }


    // Get bin-by-bin information from hist1
    int nBins = hist->GetNbinsX();
    double integral = 0;
    for (int i = 1; i <= nBins; ++i) {
        double binContent = hist->GetBinContent(i);
        integral += binContent;        
    }
    
    // Load the histogram from file2
    TH1F *hist2 = dynamic_cast<TH1F*>(file2->Get("histResultingFlux"));
    if (!hist2) {
        std::cerr << "Error: Histogram not found!" << std::endl;
        file1->Close();
        return;
    }
    
    // Get bin-by-bin information from hist2
    int nBins2 = hist2->GetNbinsX();
    double integral2 = 0;
    for (int i = 1; i <= nBins2; ++i) {
        double binContent2 = hist2->GetBinContent(i);
        integral2 += binContent2;        
    }
    
    std::cout << integral2/integral << std::endl;
    

    // Close input files
    file1->Close();
    file2->Close();
}