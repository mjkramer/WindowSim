#include <cstdio>

#include "G4VPhysicalVolume.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UserSteppingAction.hh"
#include "G4UImessenger.hh"
#include "G4Run.hh"

#include "SteppingAction.hh"

#define ID_PROTON 2212

using namespace CLHEP;

void SteppingAction::UserSteppingAction(const G4Step *step)
{
  G4VPhysicalVolume* preVol = step->GetPreStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume* postVol = step->GetPostStepPoint()->GetPhysicalVolume();

  G4String preName = preVol ? preVol->GetName() : G4String("NULL");
  G4String postName = postVol ? postVol->GetName() : G4String("NULL");

  if (preName == "window" && postName == "world") {
    G4int id = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

    G4ThreeVector p = step->GetPostStepPoint()->GetMomentum();
    G4double e_mev = step->GetPostStepPoint()->GetKineticEnergy()/MeV;

    printf("parent = %d\n", step->GetTrack()->GetParentID()); // XXX
//    fEventAction->Register(id, p.cosTheta(), p.mag());
    fEventAction->Register(id, p.cosTheta(), e_mev, p.mag()/MeV);
  }
}
