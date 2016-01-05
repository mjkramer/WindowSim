#ifndef EventAction_h
#define EventAction_h 1

#include <map>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>

#include "G4UserEventAction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UImessenger.hh"

const int BUFSIZE = 1024;

class G4Track;

class EventAction : public G4UserEventAction, public G4UImessenger
{
public:
  EventAction();
  ~EventAction();

  void BeginOfEventAction(const G4Event *event);
  void EndOfEventAction(const G4Event *event);
  void SetNewValue(G4UIcommand *cmd, G4String args);

  void Register(G4int trackID, G4int partId, G4double cosTheta, G4double energyMeV, G4double momMeV,
                G4double exit_x_cm);
  void RememberParent(G4Track *track);

private:
  G4UIcmdWithAString* fFileNameCmd;
  TFile* fFile;
  TTree* fTree;
  TH1F* fEdepHist, *fEdepHistIncl; // Incl - includes secondary KE

  struct ParticleData {
    int partId;
    float cosTheta, energyMeV, momMeV, exitXcm;
  };

  // key: track ID
  std::map<G4int, ParticleData> fSeenParticles;
  std::vector<G4int> fParentTracks;

  // branches
  int fCount;
  int fPartId[BUFSIZE];
  float fCosTheta[BUFSIZE], fEnergyMeV[BUFSIZE], fMomMeV[BUFSIZE];
  float fExitXcm[BUFSIZE];
  int fTrackId[BUFSIZE];

  friend class SteppingAction;
};

#endif
