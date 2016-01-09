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

protected:
  struct ParticleData {
    G4int partId;
    G4double cosTheta, energyMeV, momMeV, exitXcm, prodXcm, iActXcm;
  };

  void Register(G4int trackID, const ParticleData& data);

private:
  G4UIcmdWithAString* fFileNameCmd;
  TFile* fFile;
  TTree* fTree;
  TH1F* fEdepHist, *fEdepHistIncl; // Incl - includes secondary KE

  // key: track ID
  std::map<G4int, ParticleData> fSeenParticles;

  // branches
  int fCount;
  int fPartId[BUFSIZE];
  float fCosTheta[BUFSIZE], fEnergyMeV[BUFSIZE], fMomMeV[BUFSIZE];
  float fExitXcm[BUFSIZE];
  float fProdXcm[BUFSIZE];
  float fIActXcm[BUFSIZE];
  int fTrackId[BUFSIZE];

  friend class SteppingAction;
};

#endif
