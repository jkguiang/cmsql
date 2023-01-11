import awkward as ak
import numpy as np
from coffea import processor
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
        events["Electron", "etaCorr"] = events.Electron.eta + events.Electron.deltaEtaSC

        # Implement PKU lepton ID
        events["Electron", "pkuVeto"] = ((events.Electron.pt > 10) & (events.Electron.cutBased >= 1))
        events["Electron", "pkuTight"] = (
            events.Electron.pkuVeto
            & (events.Electron.pt > 35) 
            & (events.Electron.cutBased >= 3)
            & (events.Electron.etaCorr < 2.5)
            & (
                ((abs(events.Electron.dz) < 0.1) & (abs(events.Electron.dxy) < 0.05) & (events.Electron.etaCorr < 1.479))
                | ((abs(events.Electron.dz) < 0.2) & (abs(events.Electron.dxy) < 0.1) & (events.Electron.etaCorr > 1.479))
            )
        )
        events["Muon", "pkuVeto"] = (
            events.Muon.tightId
            & (events.Muon.pfRelIso04_all < 0.4)
            & (events.Muon.pt > 10)
        )
        events["Muon", "pkuTight"] = (
            events.Muon.pkuVeto
            & events.Muon.tightId
            & (events.Muon.pfRelIso04_all < 0.15)
            & (events.Muon.pt > 26)
            & (abs(events.Muon.eta) < 2.4)
        )

        # Count leptons passing veto and tight lepton ID
        n_veto_leps = (
            ak.sum(events.Electron.pkuVeto, axis=-1) 
            + ak.sum(events.Muon.pkuVeto, axis=-1)
        )
        n_tight_leps = (
            ak.sum(events.Electron.pkuTight, axis=-1) 
            + ak.sum(events.Muon.pkuTight, axis=-1)
        )

        HAS_1_LEP = ((n_veto_leps == 1) & (n_tight_leps == 1))

        SKIM_SELECTION = HAS_1_LEP

        chunk_id = events.behavior["__events_factory__"]._partition_key.split("/")[-1]
        output_file = f"{self.output_name}_{chunk_id}.root"
        with Arbol.recreate(output_file) as arbol:
            arbol.write(events[SKIM_SELECTION], branch_names=["Electron*", "Muon*"])

        return {events.metadata["dataset"]: {"output_file": [output_file], "n_events": np.sum(SKIM_SELECTION)}}

    def postprocess(self, accumulator):
        return 
