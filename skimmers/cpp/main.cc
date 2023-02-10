// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TTreePerfStats.h"
#include <iostream>
#include <math.h>

float Phi_mpi_pi(float x)
{
    if (isnan(x))
    {
        std::cout << "Phi_mpi_pi(float)function called with NaN" << std::endl;
        return x;
    }
    while (x >= M_PI) x -= 2*M_PI;
    while (x < -M_PI) x += 2*M_PI;
    return x;
}

float DeltaR(float eta1, float phi1, float eta2, float phi2)
{
    float deta = eta1 - eta2;
    float dphi = Phi_mpi_pi(phi1 - phi2);
    return sqrt(deta * deta + dphi * dphi);
}

//__________________________________________________________________________________________________________________________________
int main() 
{

    // Open up the file and get the ROOT's TTree data
    auto file = TFile::Open("root://redirector.t2.ucsd.edu:1095//store/user/jguiang/testfiles/TTToHadronic_TuneCP5_13TeV_RunIISummer20UL18NanoAODv9.root");
    TTree* tree = (TTree*) file->Get("Events");

    //======================================================================
    // Configurations to play with
    //----------------------------------------------------------------------
    tree->SetCacheSize(10000000);
    tree->SetCacheEntryRange(0, tree->GetEntries());
    tree->AddBranchToCache("*");
    bool TurnOnPartialBranches = true;
    //======================================================================

    if (TurnOnPartialBranches)
    {
        // Turning off the status of the unused branches to not load them every time
        // This speed things up in terms of skimming however, it also means none of the other branches are copied
        tree->SetBranchStatus("*", false);
        tree->SetBranchStatus("nElectron*", true);
        tree->SetBranchStatus("nMuon*", true);
        tree->SetBranchStatus("nFatJet*", true);
        tree->SetBranchStatus("nJet*", true);
        tree->SetBranchStatus("Electron*", true);
        tree->SetBranchStatus("Muon*", true);
        tree->SetBranchStatus("Jet*", true);
        tree->SetBranchStatus("Tau*", true);
        tree->SetBranchStatus("GenPart*", true);
        tree->SetBranchStatus("Generator*", true);
        tree->SetBranchStatus("FatJet*", true);
        tree->SetBranchStatus("MET*", true);
        tree->SetBranchStatus("event*", true);
        tree->SetBranchStatus("run*", true);
        tree->SetBranchStatus("luminosityBlock*", true);
        tree->SetBranchStatus("genWeight*", true);
        tree->SetBranchStatus("btagWeight*", true);
        tree->SetBranchStatus("LHE*", true);
        tree->SetBranchStatus("*Weight*", true);
        tree->SetBranchStatus("Flag*", true);
        tree->SetBranchStatus("SubJet*", true);
        tree->SetBranchStatus("HLT_*", true);
        tree->SetBranchStatus("Pileup*", true);
    }

    // TTree reading performance measurement tool
    TTreePerfStats *ps= new TTreePerfStats("ioperf", tree);

    // Skim output file
    TFile* ofile = new TFile("output.root", "recreate");
    TTree* otree = tree->CloneTree(0);

    // Defining "column" variable memories
    // --- Electron columns
    unsigned int nElectron;
    float Electron_pt[90];
    float Electron_eta[90];
    float Electron_phi[90];
    float Electron_deltaEtaSC[90];
    float Electron_dz[90];
    float Electron_dxy[90];
    int Electron_cutBased[90];
    // --- Muon columns
    unsigned int nMuon;
    bool Muon_tightId[90];
    float Muon_pfRelIso04_all[90];
    float Muon_pt[90];
    float Muon_eta[90];
    float Muon_phi[90];
    // --- Jet columns
    unsigned int nJet;
    float Jet_pt[250];
    float Jet_eta[250];
    float Jet_phi[250];
    // --- FatJet columns
    unsigned int nFatJet;
    float FatJet_pt[18];
    float FatJet_eta[18];
    float FatJet_phi[18];
    float FatJet_mass[18];
    float FatJet_msoftdrop[18];
    float FatJet_particleNetMD_Xbb[18];
    float FatJet_particleNetMD_QCD[18];
    // --- MET columns
    float MET_pt;

    // Setting the address of variable memories to the TTree object's address
    // This way, whenever ''tree->GetEntry(ith-entry)'' is called the column variables
    // get assigned with the values for the ith-event.
    // --- Electron columns
    tree->SetBranchAddress("nElectron", &nElectron);
    tree->SetBranchAddress("Electron_pt", &Electron_pt);
    tree->SetBranchAddress("Electron_eta", &Electron_eta);
    tree->SetBranchAddress("Electron_phi", &Electron_phi);
    tree->SetBranchAddress("Electron_deltaEtaSC", &Electron_deltaEtaSC);
    tree->SetBranchAddress("Electron_dz", &Electron_dz);
    tree->SetBranchAddress("Electron_dxy", &Electron_dxy);
    tree->SetBranchAddress("Electron_cutBased", &Electron_cutBased);
    // --- Muon columns
    tree->SetBranchAddress("nMuon", &nMuon);
    tree->SetBranchAddress("Muon_pt", &Muon_pt);
    tree->SetBranchAddress("Muon_eta", &Muon_eta);
    tree->SetBranchAddress("Muon_phi", &Muon_phi);
    tree->SetBranchAddress("Muon_tightId", &Muon_tightId);
    tree->SetBranchAddress("Muon_pfRelIso04_all", &Muon_pfRelIso04_all);
    // --- Jet columns
    tree->SetBranchAddress("nJet", &nJet);
    tree->SetBranchAddress("Jet_pt", &Jet_pt);
    tree->SetBranchAddress("Jet_eta", &Jet_eta);
    tree->SetBranchAddress("Jet_phi", &Jet_phi);
    // --- FatJet columns
    tree->SetBranchAddress("nFatJet", &nFatJet);
    tree->SetBranchAddress("FatJet_pt", &FatJet_pt);
    tree->SetBranchAddress("FatJet_eta", &FatJet_eta);
    tree->SetBranchAddress("FatJet_phi", &FatJet_phi);
    tree->SetBranchAddress("FatJet_mass", &FatJet_mass);
    tree->SetBranchAddress("FatJet_msoftdrop", &FatJet_msoftdrop);
    tree->SetBranchAddress("FatJet_particleNetMD_Xbb", &FatJet_particleNetMD_Xbb);
    tree->SetBranchAddress("FatJet_particleNetMD_QCD", &FatJet_particleNetMD_QCD);
    // --- MET columns
    tree->SetBranchAddress("MET_pt", &MET_pt);

    // Looping over the events in the TTree
    for (unsigned int ientry = 0; ientry < tree->GetEntries(); ++ientry)
    {

        if (ientry % 10000 == 0)
            std::cout <<  " ientry: " << ientry <<  std::endl;

        // With the following function call, the "column" variables are filled
        tree->GetEntry(ientry);

        // Per event variables that we want to compute (i.e. new "column" variables to compute)
        int nVetoLepton = 0;
        int nTightLepton = 0;
        int nGoodJet = 0;
        int nGoodFatJet = 0;

        std::vector<float> TightLepton_pt;
        std::vector<float> TightLepton_eta;
        std::vector<float> TightLepton_phi;

        float MaxHbbScore = -999;
        float HbbFatJet_pt = 0;

        // Selection for Electrons
        for (unsigned int iElec = 0; iElec < nElectron; ++iElec)
        {
            // Veto lepton selections
            if (Electron_pt[iElec] <= 10)
                continue;
            if (Electron_cutBased[iElec] < 1)
                continue;

            // Count the number of veto leptons
            nVetoLepton++;

            // Tight lepton selections
            if (Electron_pt[iElec] <= 35)
                continue;
            if (Electron_cutBased[iElec] < 3)
                continue;
            if (fabs(Electron_eta[iElec] + Electron_deltaEtaSC[iElec]) >= 2.5)
                continue;
            if (fabs(Electron_eta[iElec] + Electron_deltaEtaSC[iElec]) >= 1.479)
            {
                if (fabs(Electron_dz[iElec]) >= 0.2)
                    continue;
                if (fabs(Electron_dxy[iElec]) >= 0.1)
                    continue;
            }
            else
            {
                if (fabs(Electron_dz[iElec]) >= 0.1)
                    continue;
                if (fabs(Electron_dxy[iElec]) >= 0.05)
                    continue;
            }

            TightLepton_pt.push_back(Electron_pt[iElec]);
            TightLepton_eta.push_back(Electron_eta[iElec]);
            TightLepton_phi.push_back(Electron_phi[iElec]);

            // Count the number of tight leptons
            nTightLepton++;
        }

        // Selection for Muons
        for (unsigned int iMuon = 0; iMuon < nElectron; ++iMuon)
        {
            // Veto lepton selections
            if (not Muon_tightId[iMuon])
                continue;
            if (Muon_pfRelIso04_all[iMuon] >= 0.4)
                continue;
            if (Muon_pt[iMuon] <= 10)
                continue;

            // Count the number of veto leptons
            nVetoLepton++;

            // Tight lepton selections
            if (Muon_pfRelIso04_all[iMuon] >= 0.15)
                continue;
            if (Muon_pt[iMuon] <= 26)
                continue;
            if (fabs(Muon_eta[iMuon]) >= 2.4)
                continue;

            TightLepton_pt.push_back(Muon_pt[iMuon]);
            TightLepton_eta.push_back(Muon_eta[iMuon]);
            TightLepton_phi.push_back(Muon_phi[iMuon]);

            // Count the number of tight leptons
            nTightLepton++;
        }

        // First event level selection
        if (not (nVetoLepton == 1 and nTightLepton == 1))
            continue;

        for (unsigned int iFatJet = 0; iFatJet < nFatJet; ++iFatJet)
        {
            if (FatJet_pt[iFatJet] <= 250)
                continue;
            if (FatJet_mass[iFatJet] <= 50)
                continue;
            if (FatJet_msoftdrop[iFatJet] <= 40)
                continue;

            // Compute the distance in eta v. phi space
            float dr = DeltaR(TightLepton_eta[0], TightLepton_phi[0], FatJet_eta[iFatJet], FatJet_phi[iFatJet]);
            bool is_overlap = dr < 0.8; // if the distance is less than 0.8, it is considered overlapping

            // if overlapping skip
            if (is_overlap)
                continue;

            nGoodFatJet++;

            // Compute the Xbb score (this is a score for a machine learning output classifying Higgs boson from non-Higgs boson initiated fatjets)
            float xbb_score = FatJet_particleNetMD_Xbb[iFatJet] / (FatJet_particleNetMD_Xbb[iFatJet] + FatJet_particleNetMD_QCD[iFatJet]);

            // Keep track of the fatjet with the highest Xbb score
            if (xbb_score > MaxHbbScore)
            {
                MaxHbbScore = xbb_score;
                HbbFatJet_pt = FatJet_pt[iFatJet];
            }

        }

        // Second event level selection
        if (nGoodFatJet < 1)
            continue;


        // Third event level selection Require ST > 800 GeV
        float ST = HbbFatJet_pt + TightLepton_pt[0] + MET_pt;
        if (ST < 800)
            continue;

        // Selection for Jets
        for (unsigned int iJet = 0; iJet < nJet; ++iJet)
        {
            // each jet must have pt greater than 20
            if (Jet_pt[iJet] <= 20)
                continue;

            // Compute the distance in eta v. phi space
            float dr = DeltaR(TightLepton_eta[0], TightLepton_phi[0], Jet_eta[iJet], Jet_phi[iJet]);
            bool is_overlap = dr < 0.4; // if the distance is less than 0.4, it is considered overlapping

            // if overlapping skip
            if (is_overlap)
                continue;

            // Count the number of good jets
            nGoodJet++;
        }

        // Fourth event level selection
        if (nGoodJet < 2)
            continue;

        // Once we reach this point we fill the event to the output ttree if only if it reaches here
        otree->Fill();

    }

    ps->Print();
    ps->SaveAs("perf.root");
    otree->Write();
    ofile->Close();
    file->Close();

    return 0;
}
