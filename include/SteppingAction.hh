#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

#include "EventAction.hh"

class SteppingAction : public G4UserSteppingAction {
private:
  EventAction *fEventAction;

public:
  SteppingAction(EventAction *eventAction) :
    fEventAction(eventAction) {}

  void UserSteppingAction(const G4Step *step);
};

#endif
