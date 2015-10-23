#include <cstdio>

#include "G4VPhysicalVolume.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"

#include "SteppingAction.hh"

using namespace CLHEP;

SteppingAction::SteppingAction(EventAction *eventAction) :
  fEventAction(eventAction), fExitVol(NULL)
{
}

static const G4LogicalVolume* getExitVol(const char* name)
{
  G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();

  for (auto iter = lvs->begin(); iter != lvs->end(); ++iter) {
    const G4LogicalVolume *vol = *iter;
    if (vol->GetName() == name) {
      return vol;
    }
  }

  return NULL;
}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
  if (fExitVol == NULL) fExitVol = getExitVol("plugSkinBackOut");

  G4VPhysicalVolume* preVol = step->GetPreStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume* postVol = step->GetPostStepPoint()->GetPhysicalVolume();

  int trackID = step->GetTrack()->GetTrackID();

  G4VPhysicalVolume *worldVol = G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking()->GetWorldVolume();

  bool outTheFront = preVol->GetLogicalVolume() == fExitVol;

  if (preVol != worldVol && postVol == worldVol && outTheFront) {
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
