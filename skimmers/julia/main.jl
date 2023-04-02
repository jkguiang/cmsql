using Arrow, DataFrames, LorentzVectorHEP

const _dfa = Arrow.Table("./nanoAOD_nocomp.feather");
const dfa = DataFrame(_dfa; copycols=false);

function main(dfa)
    (; nElectron, Electron_pt, Electron_phi, Electron_cutBased,
    Electron_eta, Electron_deltaEtaSC, Electron_dxy, Electron_dz,
    nMuon, Muon_tightId, Muon_pfRelIso04_all, Muon_pt, Muon_eta,
    nFatJet, FatJet_pt, FatJet_mass, FatJet_msoftdrop, nJet, Jet_pt, Jet_eta, Jet_phi) = dfa

    mask = _kernal(axes(dfa,1), nElectron, Electron_pt, Electron_phi, Electron_cutBased, 
    Electron_eta, Electron_deltaEtaSC, Electron_dxy, Electron_dz, nMuon, 
    Muon_tightId, Muon_pfRelIso04_all, Muon_pt, Muon_eta, nFatJet, FatJet_pt,
    FatJet_mass, FatJet_msoftdrop, nJet, Jet_pt, Jet_eta, Jet_phi)

    df_out = @view dfa[mask, :]
    Arrow.write(joinpath(@__DIR__, "output.feather"), df_out)
end

function _kernal(idxs, nElectrons, Electron_pts, Electron_phis, Electron_cutBaseds, 
    Electron_etas, Electron_deltaEtaSCs, Electron_dxys, Electron_dzs, nMuons,
    Muon_tightIds, Muon_pfRelIso04_alls, Muon_pts, Muon_etas,
    nFatJets, FatJet_pts, FatJet_masss, FatJet_msoftdrops,
    nJets, Jet_pts, Jet_etas, Jet_phis)

    map(idxs) do i
        nVetoLepton = 0
        nTightLepton = 0

        nElectron = nElectrons[i]
        Electron_pt = Electron_pts[i]
        Electron_phi = Electron_phis[i]
        Electron_cutBased = Electron_cutBaseds[i]
        Electron_eta = Electron_etas[i]
        Electron_deltaEtaSC = Electron_deltaEtaSCs[i]
        Electron_dxy = Electron_dxys[i]
        Electron_dz = Electron_dzs[i]

        for iElec in 1:nElectron
            Electron_pt[iElec] <= 10 && continue
            Electron_cutBased[iElec] < 1 && continue
            nVetoLepton += 1
            Electron_pt[iElec] <= 35 && continue
            Electron_cutBased[iElec] < 3 && continue
            abs(Electron_eta[iElec] + Electron_deltaEtaSC[iElec]) >= 2.5 && continue
            if abs(Electron_eta[iElec] + Electron_deltaEtaSC[iElec]) >= 1.479
                abs(Electron_dz[iElec]) >= 0.2 && continue
                abs(Electron_dxy[iElec]) >= 0.1 && continue
            else
                abs(Electron_dz[iElec]) >= 0.1 && continue
                abs(Electron_dxy[iElec]) >= 0.05 && continue
            end

            nTightLepton+=1
        end

        if (nVetoLepton > 1 || nTightLepton > 1)
            return false
        end

        nMuon = nMuons[i]
        Muon_tightId = Muon_tightIds[i]
        Muon_pfRelIso04_all = Muon_pfRelIso04_alls[i]
        Muon_pt = Muon_pts[i]
        Muon_eta = Muon_etas[i]

        for iMuon in 1:nMuon
            !(Muon_tightId[iMuon]) && continue
            Muon_pfRelIso04_all[iMuon] >= 0.4 && continue
            Muon_pt[iMuon] <= 10 && continue
            nVetoLepton += 1

            Muon_pfRelIso04_all[iMuon] >= 0.15 && continue
            Muon_pt[iMuon] <= 26 && continue
            abs(Muon_eta[iMuon]) >= 2.4 && continue
            nTightLepton+=1
        end

        if !(nVetoLepton == nTightLepton == 1)
            return false
        end

        nFatJet = nFatJets[i]
        FatJet_pt = FatJet_pts[i]
        FatJet_mass = FatJet_masss[i]
        FatJet_msoftdrop = FatJet_msoftdrops[i]

        nGoodFatJet = 0

        for iFatJet in 1:nFatJet
            FatJet_pt[iFatJet] <= 250 && continue
            FatJet_mass[iFatJet] <= 50 && continue
            FatJet_msoftdrop[iFatJet] <= 40 && continue

            #FIXME complete this with dR()
            nGoodFatJet+=1
        end

        if nGoodFatJet < 1
            return false
        end


        nJet = nJets[i]
        Jet_pt = Jet_pts[i]
        Jet_eta = Jet_etas[i]
        Jet_phi = Jet_phis[i]
        nGoodJet = 0

        for iJet in 1:nJet
            Jet_pt[iJet] <= 20 && continue
            #FIXME complete this with dR()
            nGoodJet += 1
        end

        return true
    end
end
