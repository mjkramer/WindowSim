#include "G4PhysListFactory.hh"
#include "G4StepLimiterPhysics.hh"

#include "RunManager.hh"
#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

RunManager::RunManager()
{
  fDirectory = new G4UIdirectory("/sim/");
  fDirectory->SetGuidance("Parameters for the simulation");

  fPhysListCmd = new G4UIcmdWithAString("/sim/setReferencePhysList", this);
  fPhysListCmd->SetGuidance("Set reference physics list to be used");

  fSeedCmd = new G4UIcmdWithAnInteger("/sim/seed", this);
  fSeedCmd->SetGuidance("Seed RNG with provided integer");

  fSeedWithTimeCmd = new G4UIcmdWithoutParameter("/sim/seedWithTime", this);
  fSeedWithTimeCmd->SetGuidance("Seed RNG with UNIX time");
}

RunManager::~RunManager() {}

void RunManager::InitActions()
{
  SetUserAction(new PrimaryGeneratorAction);

  EventAction *eventAction = new EventAction;
  SetUserAction(eventAction);

  SetUserAction(new SteppingAction(eventAction));
}

void RunManager::SetNewValue(G4UIcommand *cmd, G4String args)
{
  if (cmd == fPhysListCmd) {
    G4VModularPhysicsList* physList = (new G4PhysListFactory)->GetReferencePhysList(args);
    physList->RegisterPhysics(new G4StepLimiterPhysics);
    SetUserInitialization(physList);

    InitActions();              // phys list must be set before primary gen
  }

  else if (cmd == fSeedWithTimeCmd) {
    int seed = time(0);
    G4Random::setTheSeed(seed);
    std::cout << "Seed: " << seed << std::endl;
  }

  else if (cmd == fSeedCmd) {
    G4int seed = fSeedCmd->GetNewIntValue(args);
    G4Random::setTheSeed(seed);
    std::cout << "Seed: " << seed << std::endl;
  }
}
