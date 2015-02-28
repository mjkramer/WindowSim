#ifndef RunManager_h
#define RunManager_h 1

#include "G4RunManager.hh"
#include "globals.hh"
#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"


class RunManager : public G4RunManager, public G4UImessenger
{
public:
  RunManager();
  ~RunManager();

  void SetNewValue(G4UIcommand *cmd, G4String args);

private:
  void InitActions();

  G4UIdirectory* fDirectory;
  G4UIcmdWithAString* fPhysListCmd;
  G4UIcmdWithoutParameter* fSeedWithTimeCmd;
  G4UIcmdWithAnInteger* fSeedCmd;
};

#endif
