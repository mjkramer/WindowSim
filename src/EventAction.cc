#include <algorithm>
#include <array>

#include "G4Track.hh"
#include "G4RunManager.hh"

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
    fEdepHistIncl->Write();
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
    fTree->Branch("trackId", fTrackId, "trackId[count]/I");

    fEdepHist = new TH1F("edep", "Deposited energy vs x", 4000, -40, 0);
    fEdepHist->SetXTitle("[cm]");
    fEdepHist->SetYTitle("[MeV / 0.1 mm]");

    fEdepHistIncl = new TH1F("edepIncl", "Deposited energy vs x", 4000, -40, 0);
    fEdepHistIncl->SetXTitle("[cm]");
    fEdepHistIncl->SetYTitle("[MeV / 0.1 mm]");
  }
}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fSeenParticles.clear();
}

void EventAction::EndOfEventAction(const G4Event*)
{

  fCount = 0;

  for (auto&& pair : fSeenParticles) {
    int id = pair.first;
    ParticleData& p = pair.second;

    fPartId[fCount] = p.partId;
    fCosTheta[fCount] = p.cosTheta;
    fEnergyMeV[fCount] = p.energyMeV;
    fMomMeV[fCount] = p.momMeV;
    fExitXcm[fCount] = p.exitXcm;
    // fIsPrimary[fCount] = id == 1;
    fTrackId[fCount] = id;

    ++fCount;
  }

  if (fCount) fTree->Fill();
}

void EventAction::Register(G4int trackID, G4int partId, G4double cosTheta, G4double energyMeV, G4double momMeV, G4double exit_x_cm)
{
  fSeenParticles[trackID] = {partId, cosTheta, energyMeV, momMeV, exit_x_cm};
}
