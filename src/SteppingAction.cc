#include <cstdio>

#include "G4VPhysicalVolume.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"

#include "SteppingAction.hh"

using namespace CLHEP;

void SteppingAction::UserSteppingAction(const G4Step *step)
{
  G4VPhysicalVolume* preVol = step->GetPreStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume* postVol = step->GetPostStepPoint()->GetPhysicalVolume();

  int trackID = step->GetTrack()->GetTrackID();

  G4VPhysicalVolume *worldVol = G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking()->GetWorldVolume();

  if (preVol != worldVol && postVol == worldVol) {
    G4int pid = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    G4StepPoint *postPt = step->GetPostStepPoint();

    G4ThreeVector p = postPt->GetMomentum();
    G4ThreeVector p_rot = HepRotationY(90*deg) * p;
    G4double e_mev = postPt->GetKineticEnergy() / MeV;
    G4double exit_x_cm = postPt->GetPosition().x() / cm;

    // FIXME: rotate axes DONE
    fEventAction->Register(trackID, pid, p_rot.cosTheta(), e_mev, p.mag()/MeV,
                           exit_x_cm);
  }

  // totally ignore steps entirely in the world volume
  // don't worry about omitting their parents
  // if (!(preVol == worldVol && postVol == worldVol))
  //   fEventAction->RememberParent(step->GetTrack());
}
