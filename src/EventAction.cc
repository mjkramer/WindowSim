#include <algorithm>
#include <array>

#include "G4ParticleTable.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"

#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

using namespace CLHEP;


EventAction::EventAction() :
  fFile(NULL), fTree(NULL)
{
  fFileNameCmd = new G4UIcmdWithAString("/sim/setFileName", this);
  fFileNameCmd->SetGuidance("Set file name");
}

EventAction::~EventAction()
{
  if (fFile) {
    fTree->Write();
    fEdepHist->Write();
    fFile->Close();
  }
}

void EventAction::SetNewValue(G4UIcommand *cmd, G4String args)
{
  if (cmd == fFileNameCmd) {
    fFile = new TFile(args, "RECREATE");

    fTree = new TTree("particles", "Secondary particles");
    fTree->Branch("count", &fCount, "count/I");
    fTree->Branch("partId", fPartId, "partId[count]/I");
    fTree->Branch("cosTheta", fCosTheta, "cosTheta[count]/F");
    fTree->Branch("energyMeV", fEnergyMeV, "energyMeV[count]/F");
    fTree->Branch("momMeV", fMomMeV, "momMeV[count]/F");
    fTree->Branch("exitXcm", fExitXcm, "exitXcm[count]/F");

    G4ParticleGun* gun = dynamic_cast<const PrimaryGeneratorAction*>
      (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction())->GetGun();
    G4ParticleDefinition* particle = gun->GetParticleDefinition();

    std::array<G4ParticleDefinition*, 4> stables =
      {G4Proton::ProtonDefinition(), G4Gamma::GammaDefinition(),
       G4Electron::ElectronDefinition(), G4Neutron::NeutronDefinition()};

    bool is_stable = std::find(stables.begin(), stables.end(),
                               particle) != stables.end();

    double max_e = 1.1 * gun->GetParticleEnergy()/MeV + (is_stable ? 0 : particle->GetPDGMass()/MeV);

    fEdepHist = new TH2F("edep", "Deposited energy vs x", 4000, -40, 0, int(max_e), 0, int(max_e));
    fEdepHist->SetXTitle("[cm]");
    fEdepHist->SetYTitle("[MeV]");
  }
}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fSeenParticles.clear();
  fParentTracks.clear();
}

void EventAction::EndOfEventAction(const G4Event*)
{
  for (auto&& parentID : fParentTracks) {
    if (fSeenParticles.find(parentID) != fSeenParticles.end())
      fSeenParticles.erase(parentID);
  }

  fCount = 0;

  for (auto&& pair : fSeenParticles) {
    ParticleData& p = pair.second;
    fPartId[fCount] = p.partId;
    fCosTheta[fCount] = p.cosTheta;
    fEnergyMeV[fCount] = p.energyMeV;
    fMomMeV[fCount] = p.momMeV;
    fExitXcm[fCount] = p.exitXcm;
    ++fCount;
  }

  if (fCount) fTree->Fill();
}

void EventAction::Register(G4int trackID, G4int partId, G4double cosTheta, G4double energyMeV, G4double momMeV, G4double exit_x_cm)
{
  fSeenParticles[trackID] = {partId, cosTheta, energyMeV, momMeV, exit_x_cm};
  // fPartId[fCount] = partId;
  // fCosTheta[fCount] = cosTheta;
  // fEnergyMeV[fCount] = energyMeV;
  // fMomMeV[fCount] = momMeV;
  // ++fCount;
}

void EventAction::RememberParent(G4Track *track)
{
  // store parents so that when we're filling the tree, we omit any particle
  // that produced a secondary
  fParentTracks.push_back(track->GetParentID());
}
