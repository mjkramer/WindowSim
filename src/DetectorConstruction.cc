#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "DetectorConstruction.hh"

using namespace CLHEP;

DetectorConstruction::DetectorConstruction(G4double thickness)
  : fThickness(thickness) {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* man = G4NistManager::Instance();
  G4Material* vacuum = man->FindOrBuildMaterial("G4_Galactic", false);
  G4Material* aluminum = man->FindOrBuildMaterial("G4_Al");

  G4VisAttributes* glassVisAtt 
    = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.3));

  G4Box* worldBox = new G4Box("worldBox", 10.0*m, 10.0*m, 10.0*m);
  G4LogicalVolume* worldL = new G4LogicalVolume(worldBox, vacuum, "worldL");
  worldL->SetVisAttributes(G4VisAttributes::Invisible);
  G4VPhysicalVolume* worldP =
    new G4PVPlacement(0, G4ThreeVector(), worldL, "world", 0, false, 0, true);                 

//  G4double inch = 2.54*cm;

  G4Box* windowBox = new G4Box("windowBox", 0.5*m, 0.5*m, fThickness*mm);
  G4LogicalVolume* windowL = new G4LogicalVolume(windowBox, aluminum, "windowL");
  windowL->SetVisAttributes(glassVisAtt);
  G4VPhysicalVolume* windowP =
      new G4PVPlacement(G4Transform3D::Identity, windowL, "window", worldL, false, 0, true);

  (void) windowP;       // suppress warning
  return worldP;
}

