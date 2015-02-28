#include <cstring>
#include <iostream>

#include <TROOT.h>
#include "G4UIExecutive.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisExecutive.hh"
#include "globals.hh"

#include "RunManager.hh"

int main(int argc, char** argv)
{
  char* macroFile;
  bool vis = false;

  if (argc >= 3 && !strcmp(argv[1], "-v")) {
    vis = true;
    macroFile = argv[2];
  } else if (argc >= 2) {
    macroFile = argv[1];
  } else {
      std::cout << "No macro file provided!" << std::endl;
      return 1;
  }

  RunManager* runManager = new RunManager;

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
