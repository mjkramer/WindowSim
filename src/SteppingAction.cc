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

  bool outTheFront = step->GetPostStepPoint()->GetPosition().x() < -32*cm;

  if (preVol != worldVol && postVol == worldVol && outTheFront) {
    G4int pid = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

    G4ThreeVector p = step->GetPostStepPoint()->GetMomentum();
    G4ThreeVector p_rot = HepRotationY(90*deg) * p;
    G4double e_mev = step->GetPostStepPoint()->GetKineticEnergy()/MeV;

    // FIXME: rotate axes DONE
    fEventAction->Register(trackID, pid, p_rot.cosTheta(), e_mev, p.mag()/MeV);
  }

  // totally ignore steps entirely in the world volume
  // don't worry about omitting their parents
  if (!(preVol == worldVol && postVol == worldVol))
    fEventAction->RememberParent(step->GetTrack());
}
