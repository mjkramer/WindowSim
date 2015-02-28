#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4Event;
class G4GeneralParticleSource;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  virtual void GeneratePrimaries(G4Event*);
  PrimaryGeneratorAction();

private:
  G4GeneralParticleSource* fParticleGun;
};

#endif
