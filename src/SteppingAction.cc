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

    G4ThreeVector p = step->GetPostStepPoint()->GetMomentum();
    G4ThreeVector p_rot = HepRotationY(90*deg) * p;
    G4double e_mev = step->GetPostStepPoint()->GetKineticEnergy()/MeV;

    // FIXME: rotate axes DONE
    fEventAction->Register(trackID, pid, p_rot.cosTheta(), e_mev, p.mag()/MeV);
  }
}
