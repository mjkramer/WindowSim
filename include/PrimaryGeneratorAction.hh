#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

class G4Event;
class G4GeneralParticleSource;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  virtual void GeneratePrimaries(G4Event*);
  PrimaryGeneratorAction();

  G4ParticleGun* GetGun() const { return fParticleGun; }


private:
  G4ParticleGun* fParticleGun;
};

#endif
