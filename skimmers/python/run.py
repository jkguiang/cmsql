from coffea import nanoevents
from coffea import processor
from skimprocessor import SkimProcessor

if __name__ == "__main__":
    skimmer = SkimProcessor(
        keep_branches=[
            "Electron*", "nElectron",
            "Muon*", "nMuon",
            "Tau*", "nTau",
            "Jet*", "nJet",
            "FatJet*", "nFatJet",
            "GenPart*", "nGenPart",
            "GenJet*", "nGenJet",
            "Generator*",
            "MET*",
            "event*",
            "run*",
            "luminosityBlock*",
            "genWeight*",
            "btagWeight*",
            "LHE*",
            "*Weight*",
            "Flag*",
            "SubJet*",
            "HLT_*",
            "Pileup*",
            "fixedGridRhoFastjetAll"
        ]
    )

    runner = processor.Runner(
        executor=processor.IterativeExecutor(status=True), 
        savemetrics=True,
        schema=nanoevents.NanoAODSchema
    )

    accumulators, metrics = runner(
        {"example": ["/workdir/example_RunIISummer20UL18.root"]}, 
        "Events", 
        processor_instance=skimmer
    )
    print(accumulators)
