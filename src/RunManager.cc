#include <iostream>

#include "G4PhysListFactory.hh"

#include "RunManager.hh"
#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

RunManager::RunManager() {
  SetUserInitialization(new DetectorConstruction);
  SetUserInitialization((new G4PhysListFactory)
			->GetReferencePhysList("QGSP_BERT_HP"));

  SetUserAction(new PrimaryGeneratorAction);
  EventAction *eventAction = new EventAction;
  SetUserAction(eventAction);
  SetUserAction(new SteppingAction(eventAction));

  fDirectory = new G4UIdirectory("/sim/");
  fDirectory->SetGuidance("Parameters for nuScint");

  fSeedWithTimeCmd = new G4UIcmdWithoutParameter("/sim/seedWithTime", this);
  fSeedWithTimeCmd->SetGuidance("Seed RNG with UNIX time");
}

RunManager::~RunManager() {
}

void RunManager::SetNewValue(G4UIcommand *cmd, G4String args)
{
  if (cmd == fSeedWithTimeCmd) {
    int seed = time(0);
    G4Random::setTheSeed(seed);
    std::cout << "Seed: " << seed << std::endl;
  } else if (cmd == fSeedCmd) {
    G4int seed = fSeedCmd->GetNewIntValue(args);
    G4Random::setTheSeed(seed);
    std::cout << "Seed: " << seed << std::endl;
  }
}
