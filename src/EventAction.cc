#include <algorithm>
#include <cstdio>

#include "EventAction.hh"

#include "G4VPhysicalVolume.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UserSteppingAction.hh"
#include "G4UImessenger.hh"
#include "G4Run.hh"

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
  fCount = 0;
}
    
void EventAction::EndOfEventAction(const G4Event*)
{
  if (fCount) fTree->Fill();
}

void EventAction::Register(G4int partId, G4double cosTheta, G4double energyMeV, G4double momMeV)
{
  fPartId[fCount] = partId;
  fCosTheta[fCount] = cosTheta;
  fEnergyMeV[fCount] = energyMeV;
  fMomMeV[fCount] = momMeV;
  ++fCount;

  printf("pID = %d\t\tcos = %f\t\te = %f\t\tp = %f\n", partId, cosTheta, energyMeV, momMeV);
}
