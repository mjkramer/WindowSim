#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4UImanager.hh"

#include "EventAction.hh"

class G4UIcmdWithoutParameter;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;

class SteppingAction : public G4UserSteppingAction, public G4UImessenger {
private:
  EventAction *fEventAction;
  const G4LogicalVolume *fExitVol;

  bool fDebug = false;
  G4UIcmdWithoutParameter* fDebugCmd;
  G4UIcmdWithADoubleAndUnit* fStepSizeCmd;
  G4UIcmdWithAString* fExitVolCmd;

public:
  SteppingAction(EventAction *eventAction);

  void UserSteppingAction(const G4Step *step) override;
  void SetNewValue(G4UIcommand* cmd, G4String args) override;
};

#endif
