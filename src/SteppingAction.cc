#include <cstdio>
#include <vector>
#include <numeric>

#include "G4VPhysicalVolume.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UIsession.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4Region.hh"
#include "G4UserLimits.hh"

#include <TROOT.h>              // Form()

#include "SteppingAction.hh"

using namespace CLHEP;

SteppingAction::SteppingAction(EventAction *eventAction) :
  fEventAction(eventAction), fExitVol(NULL)
{
  fDebugCmd = new G4UIcmdWithoutParameter("/sim/debug", this);
  fDebugCmd->SetGuidance("Enable debugging");

  fStepSizeCmd = new G4UIcmdWithADoubleAndUnit("/sim/stepSize", this);
  fStepSizeCmd->SetGuidance("Set maximum step size");

  fExitVolCmd = new G4UIcmdWithAString("/sim/exitVol", this);
  fExitVolCmd->SetGuidance("Set exit volume");
}

void debug(const G4Step* step)
{
  // track#, pdgID, momentumAmp
  const G4String& pname = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  double trackMom = step->GetTrack()->GetMomentum().mag() / MeV;
  double preMom  = step->GetPreStepPoint()->GetMomentum().mag() / MeV;
  double postMom = step->GetPostStepPoint()->GetMomentum().mag() / MeV;
  double preKin  = step->GetPreStepPoint()->GetKineticEnergy() / MeV;
  double postKin = step->GetPostStepPoint()->GetKineticEnergy() / MeV;
  double preX = step->GetPreStepPoint()->GetPosition().x() / cm;
  double postX = step->GetPostStepPoint()->GetPosition().x() / cm;
  // G4cout << "Track " << trackID << ", " << pname << ", " << G4endl;
  G4cout << Form("Track %d, %s, %.4f, %.4f - %.4f cm, %.4f - %.4f MeV/c, %.4f - %.4f MeV",
                 step->GetTrack()->GetTrackID(), pname.data(), trackMom, preX, postX,
                 preMom, postMom, preKin, postKin) << G4endl;

  double depEn = step->GetTotalEnergyDeposit();
  G4cout << Form("Deposited %.4f MeV", depEn) << G4endl;

  const std::vector<const G4Track*>& kids = *step->GetSecondaryInCurrentStep();
  for (const G4Track* track : kids) {
    int id = track->GetTrackID();
    const G4String& kidpname = track->GetParticleDefinition()->GetParticleName();
    double mom = track->GetMomentum().mag() / MeV;
    double kin = track->GetKineticEnergy() / MeV;
    G4cout << Form("--> Track %d, %s, %.4f MeV/c, %.4f MeV", id, kidpname.data(), mom, kin) << G4endl;
  }

  G4cout << "============================================================" << G4endl;
}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
  G4VPhysicalVolume* preVol = step->GetPreStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume* postVol = step->GetPostStepPoint()->GetPhysicalVolume();

  int trackID = step->GetTrack()->GetTrackID();

  G4VPhysicalVolume *worldVol = G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking()->GetWorldVolume();

  bool outTheFront = preVol->GetLogicalVolume() == fExitVol;

  if (fDebug) debug(step);

  if (trackID == 1)  {          // primary
    double edep = step->GetTotalEnergyDeposit();
    const std::vector<const G4Track*>& kids = *step->GetSecondaryInCurrentStep();
    for (const G4Track* track : kids) {
      edep += track->GetKineticEnergy() / MeV;
    }

    auto getX = [](G4StepPoint* p) { return p->GetPosition().x() / cm; };
    double x = (getX(step->GetPreStepPoint()) + getX(step->GetPostStepPoint())) / 2;

    fEventAction->fEdepHist->Fill(x, edep);
  }

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

void SteppingAction::SetNewValue(G4UIcommand* cmd, G4String args)
{
  if (cmd == fDebugCmd)
    fDebug = true;

  else if (cmd == fStepSizeCmd) {
    G4Region *root = G4TransportationManager::GetTransportationManager()
      ->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume()->GetRegion();
    root->SetUserLimits(new G4UserLimits(fStepSizeCmd->GetNewDoubleValue(args)));
  }

  else if (cmd == fExitVolCmd) {
    G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();

    for (auto iter = lvs->begin(); iter != lvs->end(); ++iter) {
      const G4LogicalVolume *vol = *iter;
      if (vol->GetName() == args) {
        fExitVol = vol;
        break;
      }
    }
  }
}
