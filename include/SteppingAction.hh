#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

#include "EventAction.hh"

class SteppingAction : public G4UserSteppingAction {
private:
  EventAction *fEventAction;
  const G4LogicalVolume *fExitVol;

public:
  SteppingAction(EventAction *eventAction);

  void UserSteppingAction(const G4Step *step);
};

#endif
