import awkward as ak
import numpy as np
from coffea import processor
from coffea.nanoevents.methods.vector import LorentzVector
from arbol import Arbol

import time

class SkimProcessor(processor.ProcessorABC):
    def __init__(self, year="2018", output_name="output", keep_branches=None):
        self.year = year
        self.output_name = output_name
        self.keep_branches = keep_branches or []

    @property
    def accumulator(self):
        return self._accumulator

    def process(self, events: ak.Array):
        """
        Returns skimmed events which pass preselection cuts and with the branches 
        listed in self._skimvars
        """
        elec_eta_plus_deltaEtaSC = events.Electron.eta + events.Electron.deltaEtaSC

        # Implement PKU lepton ID
        events["Electron", "pkuVetoID"] = ((events.Electron.pt > 10) & (events.Electron.cutBased >= 1))
        events["Electron", "pkuTightID"] = (
            events.Electron.pkuVetoID
            & (events.Electron.pt > 35) 
            & (events.Electron.cutBased >= 3)
            & (elec_eta_plus_deltaEtaSC < 2.5)
            & (
                ((abs(events.Electron.dz) < 0.1) & (abs(events.Electron.dxy) < 0.05) & (elec_eta_plus_deltaEtaSC < 1.479))
                | ((abs(events.Electron.dz) < 0.2) & (abs(events.Electron.dxy) < 0.1) & (elec_eta_plus_deltaEtaSC >= 1.479))
            )
        )
        events["Muon", "pkuVetoID"] = (
            events.Muon.tightId
            & (events.Muon.pfRelIso04_all < 0.4)
            & (events.Muon.pt > 10)
        )
        events["Muon", "pkuTightID"] = (
            events.Muon.pkuVetoID
            & events.Muon.tightId
            & (events.Muon.pfRelIso04_all < 0.15)
            & (events.Muon.pt > 26)
            & (abs(events.Muon.eta) < 2.4)
        )

        # Count leptons passing veto and tight lepton ID
        n_veto_leps = (
            ak.sum(events.Electron.pkuVetoID, axis=-1) 
            + ak.sum(events.Muon.pkuVetoID, axis=-1)
        )
        n_tight_leps = (
            ak.sum(events.Electron.pkuTightID, axis=-1)
            + ak.sum(events.Muon.pkuTightID, axis=-1)
        )

        # First event-level selection
        events = events[(n_veto_leps == 1) & (n_tight_leps == 1)]

        # Select the one-and-only-one tight lepton
        events["Lepton"] = ak.concatenate(
            [
                events.Electron[events.Electron.pkuTightID],
                events.Muon[events.Muon.pkuTightID]
            ],
            axis=1
        )

        # Get dR(fatjet_i, lepton) for all fat jets
        _, fatjet_nearest_lep_dRs = events.FatJet.nearest(events.Lepton, return_metric=True)

        # Fat jet selection
        events["FatJet", "isGood"] = (
            (events.FatJet.pt > 250)
            & (events.FatJet.mass > 50)
            & (events.FatJet.msoftdrop > 40)
            & (fatjet_nearest_lep_dRs >= 0.8)
        )
        
        # Count good fat jets
        n_good_fatjets = ak.sum(events.FatJet.isGood, axis=-1)

        # Second event-level selection
        events = events[n_good_fatjets >= 1]

        # Select good fat jets
        good_fatjets = events.FatJet[events.FatJet.isGood]

        max_xbb_good_fatjet_idxs = ak.argmax(
            good_fatjets.particleNetMD_Xbb
            / (good_fatjets.particleNetMD_Xbb + good_fatjets.particleNetMD_QCD), 
            axis=1,
            keepdims=True
        )

        events["HbbFatJet"] = good_fatjets[max_xbb_good_fatjet_idxs]

        events["ST"] = (
            ak.flatten(events.HbbFatJet.pt) 
            + ak.flatten(events.Lepton.pt)
            + events.MET.pt
        )

        # Third event-level selection
        events = events[events.ST >= 800]

        # Get dR(fatjet_i, lepton) for all fat jets
        _, jet_nearest_lep_dRs = events.Jet.nearest(events.Lepton, return_metric=True)

        # Jet selection
        events["Jet", "isGood"] = ((events.Jet.pt > 20) & (jet_nearest_lep_dRs >= 0.4))

        # Count good jets
        n_good_jets = ak.sum(events.Jet.isGood, axis=-1)

        # Fourth event-level selection
        events = events[n_good_jets >= 2]

        chunk_id = events.behavior["__events_factory__"]._partition_key.split("/")[-1]
        output_file = f"{self.output_name}_{chunk_id}.root"
        # with Arbol.recreate(output_file) as arbol:
        #     arbol.write(events, branch_names=["Electron*", "Muon*"])

        return {events.metadata["dataset"]: {"output_file": [output_file], "n_events": len(events)}}

    def postprocess(self, accumulator):
        return 
