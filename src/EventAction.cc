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
  fKB(0), fFile(NULL), fProTree(NULL), fStepTree(NULL), fEventId(0)
{
  fFileNameCmd = new G4UIcmdWithAString("/sim/setFileName", this);
  fFileNameCmd->SetGuidance("Set file name");

  fKBCmd = new G4UIcmdWithADouble("/sim/setKB", this);
  fKBCmd->SetGuidance("Set Birks constant");
}

EventAction::~EventAction()
{
  if (fFile) {
    fProTree->Write();
    fStepTree->Write();
    fFile->Close();
  }
}

void EventAction::SetNewValue(G4UIcommand *cmd, G4String args)
{
  if (cmd == fFileNameCmd) {
    fFile = new TFile(args, "RECREATE");

    fProTree = new TTree("proTree", "Proton Tree");
    fProTree->Branch("Nprotons", &fNprotons, "Nprotons/I");
    fProTree->Branch("proEdep", fProtonEdep, "proEdep[Nprotons]/F");
    fProTree->Branch("proEtrue", fProtonEtrue, "proEtrue[Nprotons]/F");

    fStepTree = new TTree("stepTree", "Step Tree");
    fStepTree->Branch("eventId", &fEventId, "eventId/I");
    fStepTree->Branch("Ei", &fEi, "Ei/F");
    fStepTree->Branch("Ef", &fEf, "Ef/F");
    fStepTree->Branch("dx", &fdx, "dx/F");
  }
  else if (cmd == fKBCmd) {
    fKB = fKBCmd->GetNewDoubleValue(args)/(mm/MeV);
  }
}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fProtons.clear();

  // printf("\n***********************************\nNew event\n");
}
    
void EventAction::EndOfEventAction(const G4Event*)
{
  if (fProTree) {
    fNprotons = fProtons.size();

    std::fill(fProtonEdep, fProtonEdep+fNprotons, 0);
    std::fill(fProtonEtrue, fProtonEtrue+fNprotons, 0);

    int i=0;
    for (std::map<G4Track*, Proton>::iterator it = fProtons.begin();
	 it != fProtons.end(); ++it, ++i) {
      Proton& p = it->second;
      fProtonEdep[i] = p.Edep;
      fProtonEtrue[i] = p.Etrue;
      // printf("Proton: Edep = %f, Etrue = %f\n", p.Edep, p.Etrue);
    }

    fProTree->Fill();
  }

  ++fEventId;
}

void EventAction::Register(G4Track* track, double e1, double e2, double dx)
{
  Proton& p = fProtons[track];

  if (p.Etrue == 0) {		// new proton
    p.Etrue = e1;
    // printf("Setting Etrue = %f\n", e1);
  }

  double Edep = (e1-e2) / (1 + fKB * (e1-e2)/dx);
  // printf("Adding Edep = %f\n", Edep);
  p.Edep += Edep;

  fEi = e1;
  fEf = e2;
  fdx = dx;
  fStepTree->Fill();
}
