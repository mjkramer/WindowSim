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
  G4StepPoint* preStepPt = step->GetPreStepPoint();
  G4StepPoint* postStepPt = step->GetPostStepPoint();

  double e1 = preStepPt->GetKineticEnergy()/MeV;
  double e2 = postStepPt->GetKineticEnergy()/MeV;
  double dx = (preStepPt->GetPosition() - postStepPt->GetPosition()).mag()/mm;

  G4VPhysicalVolume *preStepVol = preStepPt->GetPhysicalVolume();
  G4VPhysicalVolume *postStepVol = postStepPt->GetPhysicalVolume();

  G4String preName = preStepVol ? preStepVol->GetName() : G4String("NULL");
  G4String postName = postStepVol ? postStepVol->GetName() : G4String("NULL");

  G4String partName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  G4int partId = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

  // printf("Step (%s:0x%llx): %s (%f MeV) -> %s (%f MeV), dx = %f mm\n",
  // 	 partName.data(), (long long)step->GetTrack(),
  // 	 preName.data(), e1, postName.data(), e2, dx);

  if (partId == ID_PROTON && preStepVol->GetName() == "detector"
      && postStepVol->GetName() == "detector") {
    fEventAction->Register(step->GetTrack(), e1, e2, dx);
  }
}
