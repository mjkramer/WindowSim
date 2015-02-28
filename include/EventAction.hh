#ifndef EventAction_h
#define EventAction_h 1

#include <map>

#include <TFile.h>
#include <TTree.h>

#include "globals.hh"
#include "G4UserEventAction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UImessenger.hh"
#include "G4Run.hh"

class G4Track;


class EventAction : public G4UserEventAction, public G4UImessenger
{
public:
  EventAction();
  ~EventAction();

  void BeginOfEventAction(const G4Event *event);
  void EndOfEventAction(const G4Event *event);
  void SetNewValue(G4UIcommand *cmd, G4String args);

  void Register(G4Track* track, double e1, double e2, double dx);

private:
  struct Proton {
    float Etrue, Edep;
    Proton() : Etrue(0), Edep(0) {}
  };

  std::map<G4Track*, Proton> fProtons;

  G4double fKB;

  G4UIcmdWithAString *fFileNameCmd;
  G4UIcmdWithADouble *fKBCmd;
  TFile *fFile;

  TTree *fProTree;
  G4int fNprotons;
  G4float fProtonEdep[128], fProtonEtrue[128];

  TTree *fStepTree;
  G4int fEventId;
  G4float fEi, fEf, fdx;
};

#endif
