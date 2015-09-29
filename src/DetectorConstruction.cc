#include "G4GDMLParser.hh"
#include "G4Tubs.hh"

#include "DetectorConstruction.hh"

using namespace CLHEP;

DetectorConstruction::DetectorConstruction(const char *_gdmlFile)
  : gdmlFile(_gdmlFile) { }

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4GDMLParser p;
  p.Read(gdmlFile);
  // G4VPhysicalVolume *physWorld =
  //   const_cast<G4VPhysicalVolume*>(p.GetWorldVolume());

  G4VPhysicalVolume *physWorld = p.GetWorldVolume();

  // G4Tubs *tube = new G4Tubs()

  return physWorld;
}

