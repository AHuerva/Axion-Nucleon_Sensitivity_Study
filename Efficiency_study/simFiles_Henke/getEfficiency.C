void getEfficiency (const std::string &filePattern){

    auto files = TRestTools::GetFilesMatchingPattern((std::string)filePattern);

    std::map<double, double> effValues; // Declaration of effValues map
    TGraphErrors *gr = new TGraphErrors();
    int c = 0;

    // Open an output file stream
    std::ofstream outFile("efficiencies.txt");

    for (const auto &fileName : files) {

        TRestRun run(fileName);

        TRestGeant4Metadata* G4Metadata = static_cast<TRestGeant4Metadata*>(run.GetMetadataClass("TRestGeant4Metadata"));
        const auto sensitiveVolumeName = G4Metadata->GetSensitiveVolume();
        const auto sensitiveVolID = G4Metadata->GetGeant4GeometryInfo().GetIDFromVolume(sensitiveVolumeName);

        int nCounts = 0;
        const int entries = run.GetEntries();
        const int nSimEvents = G4Metadata->GetNumberOfEvents();
        if (entries == 0) continue;

        TRestGeant4Event* g4Event = static_cast<TRestGeant4Event*>(run.GetInputEvent());

        double initialEnergy;
        for (int i = 0; i < entries; i++) {
            run.GetEntry(i);
            const double sensitiveVolumeEnergy = g4Event->GetEnergyInVolume(sensitiveVolumeName.Data());
            initialEnergy = g4Event->GetPrimaryEventEnergy();
            const auto meanPos = g4Event->GetMeanPositionInVolume(sensitiveVolID);
            const double dist = sqrt(meanPos.X() * meanPos.X() + meanPos.Y() * meanPos.Y());

            if (sensitiveVolumeEnergy / initialEnergy > 0.95) nCounts++;
        }

        double efficiency = (double)nCounts / nSimEvents;
        double error = sqrt(nCounts) / nSimEvents;
        
        // Add the point to the graph
        gr->SetPoint(c, initialEnergy, efficiency);
        gr->SetPointError(c, 0, error);
        c++;

        // Store the efficiency value for later writing to the file
        effValues[initialEnergy] = efficiency;
    }

    // Write energies and efficiencies to the file after processing all files
    for (const auto& [energy, efficiency] : effValues) {
        outFile << energy << "\t" << efficiency << std::endl;
    }

    // Close the file
    outFile.close();

    // Draw the plot
    gr->SetTitle("Efficiency vs Energy");
    gr->GetXaxis()->SetTitle("Energy (keV)");
    gr->GetYaxis()->SetTitle("Efficiency");
    gr->Draw("ALP");
}
