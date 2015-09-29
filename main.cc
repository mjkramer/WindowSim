#include <cstring>
#include <iostream>

#include <TROOT.h>
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "RunManager.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

int main(int argc, char** argv)
{
  const char *macroFile, *gdmlFile;
  bool vis = false;

  int argstart = 1;
  if (argc >= 4 && !strcmp(argv[1], "-v")) {
    vis = true;
    argstart = 2;
  } else if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " macro_file gdml_file" << std::endl;
    return 1;
  }
  macroFile = argv[argstart];
  gdmlFile = argv[argstart+1];

  RunManager* runManager = new RunManager;
  runManager->SetUserInitialization(new DetectorConstruction(gdmlFile));

#ifdef G4VIS_USE
  if (vis) {
      G4VisManager* visManager = new G4VisExecutive;
      visManager->Initialize();
  }
  G4UIExecutive* uiExec = vis ? new G4UIExecutive(argc, argv) : NULL;
#endif

  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  uiManager->ApplyCommand(Form("/control/execute %s", macroFile));

#ifdef G4VIS_USE
  if (vis) uiExec->SessionStart();
#endif

  delete runManager; // necessary!

  return 0;
}
