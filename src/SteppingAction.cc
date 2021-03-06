#include <cstdio>
#include <vector>
#include <numeric>
#include <algorithm>

#include "G4VPhysicalVolume.hh"
#include "G4VProcess.hh"
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
#include "TrackInformation.hh"

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

static bool isInteraction(const G4Step* step)
{
  auto endsWith = [](const G4String& haystack, const G4String& needle) {
    return (haystack.size() >= needle.size())
      && std::equal(needle.rbegin(), needle.rend(), haystack.rbegin());
  };
  const G4String& name = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  return endsWith(name, "lastic") || endsWith(name, "Nuclear");
}

static void debug(const G4Step* step)
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
  G4cout << Form("Track %d, %s, %.4f, %.4f - %.4f cm, %.4f - %.4f MeV/c, %.4f - %.4f MeV",
                 step->GetTrack()->GetTrackID(), pname.data(), trackMom, preX, postX,
                 preMom, postMom, preKin, postKin) << G4endl;

  G4String preProcName, postProcName;

  const G4VProcess* preProc = step->GetPreStepPoint()->GetProcessDefinedStep();
  if (preProc) preProcName = preProc->GetProcessName();

  const G4VProcess* postProc = step->GetPostStepPoint()->GetProcessDefinedStep();
  if (postProc) postProcName = postProc->GetProcessName();

  G4cout << Form("preProc: %s, postProc: %s", preProcName.data(), postProcName.data()) << G4endl;

  double depEn = step->GetTotalEnergyDeposit();
  G4cout << Form("Deposited %.4f MeV", depEn) << G4endl;

  if (isInteraction(step))
    G4cout << "INTERACTION, BABY!!!" << G4endl;

  const std::vector<const G4Track*>& kids = *step->GetSecondaryInCurrentStep();
  for (const G4Track* track : kids) {
    int id = track->GetTrackID();
    const G4String& kidpname = track->GetParticleDefinition()->GetParticleName();
    double mom = track->GetMomentum().mag() / MeV;
    double kin = track->GetKineticEnergy() / MeV;
    G4cout << Form("--> Track %d, %s, %.4f MeV/c, %.4f MeV", id, kidpname.data(), mom, kin) << G4endl;

    const G4String& creator = track->GetCreatorProcess()->GetProcessName();
    G4cout << Form("* creator: %s", creator.data()) << G4endl;
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

  if (fDebug) {
    auto& kids = *step->GetSecondaryInCurrentStep();
    bool hadElastic = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "hadElastic";
    if (hadElastic) debug(step);
    else for (auto track : kids) {
      if (track->GetKineticEnergy() / MeV > 10) {
        debug(step);
        break;
      }
    }
  }

  double edep = step->GetTotalEnergyDeposit();

  auto getX = [](G4StepPoint* p) { return p->GetPosition().x() / cm; };
  double x = (getX(step->GetPreStepPoint()) + getX(step->GetPostStepPoint())) / 2;

  if (trackID == 1)
    fEventAction->fEdepHist->Fill(x, edep);
  fEventAction->fEdepHistIncl->Fill(x, edep);

  if (! step->GetTrack()->GetUserInformation()) {
    auto info = new TrackInformation;
    info->prodXcm = getX(step->GetPreStepPoint());
    step->GetTrack()->SetUserInformation(info);
  }

  if (isInteraction(step)) {
    auto info = dynamic_cast<TrackInformation*>(step->GetTrack()->GetUserInformation());
    info->iActXcm = getX(step->GetPostStepPoint());
  }

  double sameSecoKE = 0.;
  TrackInformation* sameSecoInfo = nullptr;
  const bool parentStopped = step->GetPostStepPoint()->GetKineticEnergy() == 0;

  for (const G4Track* _track : *step->GetSecondaryInCurrentStep()) {
    G4Track* track = const_cast<G4Track*>(_track);
    auto info = new TrackInformation;
    info->prodXcm = getX(step->GetPostStepPoint());
    track->SetUserInformation(info);

    const bool secoSame = step->GetTrack()->GetDefinition() == track->GetDefinition();
    if (parentStopped && secoSame) {
      if (track->GetKineticEnergy() > sameSecoKE) {
        sameSecoKE = track->GetKineticEnergy();
        sameSecoInfo = info;
      }
    }
  }

  if (sameSecoInfo) {
    sameSecoInfo->iActXcm = getX(step->GetPostStepPoint());
  }

  bool outTheFront = preVol->GetLogicalVolume() == fExitVol;

  if (preVol != worldVol && postVol == worldVol && outTheFront) {
    G4int pid = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    G4StepPoint *postPt = step->GetPostStepPoint();

    G4ThreeVector p = postPt->GetMomentum();
    G4ThreeVector p_rot = HepRotationY(90*deg) * p;
    G4double e_mev = postPt->GetKineticEnergy() / MeV;
    G4double exit_x_cm = postPt->GetPosition().x() / cm;

    auto info = dynamic_cast<TrackInformation*>(step->GetTrack()->GetUserInformation());

    EventAction::ParticleData data =
      {pid, p_rot.cosTheta(), e_mev, p.mag()/MeV, exit_x_cm, info->prodXcm, info->iActXcm};

    fEventAction->Register(trackID, data);
  }
}

const G4LogicalVolume* getVolume(const G4String& name)
{
  G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();

  for (auto iter = lvs->begin(); iter != lvs->end(); ++iter) {
    const G4LogicalVolume *vol = *iter;
    if (vol->GetName() == name)
      return vol;
  }

  return NULL;
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

  else if (cmd == fExitVolCmd)
    fExitVol = getVolume(args);
}
