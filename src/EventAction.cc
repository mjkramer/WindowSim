#include "EventAction.hh"

#include "G4ParticleTable.hh"

using namespace CLHEP;


EventAction::EventAction() :
  fFile(NULL), fTree(NULL)
{
  fFileNameCmd = new G4UIcmdWithAString("/sim/setFileName", this);
  fFileNameCmd->SetGuidance("Set file name");
}

EventAction::~EventAction()
{
  if (fFile) {
    fTree->Write();
    fFile->Close();
  }
}

void EventAction::SetNewValue(G4UIcommand *cmd, G4String args)
{
  if (cmd == fFileNameCmd) {
    fFile = new TFile(args, "RECREATE");

    fTree = new TTree("particles", "Secondary particles");
    fTree->Branch("count", &fCount, "count/I");
    fTree->Branch("partId", fPartId, "partId[count]/I");
    fTree->Branch("cosTheta", fCosTheta, "cosTheta[count]/F");
    fTree->Branch("energyMeV", fEnergyMeV, "energyMeV[count]/F");
    fTree->Branch("momMeV", fMomMeV, "momMeV[count]/F");
  }
}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fSeenParticles.clear();
}
    
void EventAction::EndOfEventAction(const G4Event*)
{
  fCount = 0;

  for (auto&& pair : fSeenParticles) {
    ParticleData& p = pair.second;
    fPartId[fCount] = p.partId;
    fCosTheta[fCount] = p.cosTheta;
    fEnergyMeV[fCount] = p.energyMeV;
    fMomMeV[fCount] = p.momMeV;
    ++fCount;
  }

  if (fCount) fTree->Fill();
}

void EventAction::Register(G4int trackID, G4int partId, G4double cosTheta, G4double energyMeV, G4double momMeV)
{
  fSeenParticles[trackID] = {partId, cosTheta, energyMeV, momMeV};
  // fPartId[fCount] = partId;
  // fCosTheta[fCount] = cosTheta;
  // fEnergyMeV[fCount] = energyMeV;
  // fMomMeV[fCount] = momMeV;
  // ++fCount;
}
