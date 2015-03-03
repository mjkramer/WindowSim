#ifndef EventAction_h
#define EventAction_h 1

#include <vector>
#include <string>

#include <TFile.h>
#include <TTree.h>

#include "G4UserEventAction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UImessenger.hh"

class G4Track;

class EventAction : public G4UserEventAction, public G4UImessenger
{
public:
  EventAction();
  ~EventAction();

  void BeginOfEventAction(const G4Event *event);
  void EndOfEventAction(const G4Event *event);
  void SetNewValue(G4UIcommand *cmd, G4String args);

  void Register(G4int partId, G4double cosTheta, G4double energyMeV, G4double momMeV);

private:
  G4UIcmdWithAString* fFileNameCmd;
  TFile* fFile;
  TTree* fTree;

  // branches
  int fCount;
  int fPartId[128];
  float fCosTheta[128], fEnergyMeV[128], fMomMeV[128];
  std::vector<std::string> fPartName, *fpPartName;
};

#endif
