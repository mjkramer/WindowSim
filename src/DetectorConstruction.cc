#include "G4GDMLParser.hh"

#include "DetectorConstruction.hh"

using namespace CLHEP;

DetectorConstruction::DetectorConstruction(const char *_gdmlFile)
  : gdmlFile(_gdmlFile) { }

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4GDMLParser p;
  p.Read(gdmlFile);

  G4VPhysicalVolume *physWorld = p.GetWorldVolume();

  return physWorld;
}

