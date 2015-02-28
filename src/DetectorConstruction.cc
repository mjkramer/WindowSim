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

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* man = G4NistManager::Instance();

  G4Material* vacuum = man->FindOrBuildMaterial("G4_Galactic", false);

  G4Element* hydrogen = man->FindOrBuildElement("H");
  G4Element* carbon = man->FindOrBuildElement("C");

  // G4Material* detectorMat = man->FindOrBuildMaterial("G4_POLYETHYLENE", false);

  G4Material* detectorMat = new G4Material("EJ-301", 0.87*g/cm3, 2);
  detectorMat->AddElement(carbon, 8);
  detectorMat->AddElement(hydrogen, 10);

  G4VisAttributes* glassVisAtt 
    = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.3));

  G4Box* worldBox = new G4Box("worldBox", 10.0*m, 10.0*m, 10.0*m);
  G4LogicalVolume* worldL = new G4LogicalVolume(worldBox, vacuum, "worldL");
  worldL->SetVisAttributes(G4VisAttributes::Invisible);
  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0, G4ThreeVector(), worldL, "world", 0, false, 0, true);                 

  G4double inch = 2.54*cm;
  G4Tubs* detectorS = new G4Tubs("detectorS", 0, 2.5*inch, 2.5*inch, 0, 2.0*pi);
  G4LogicalVolume* detectorL 
    = new G4LogicalVolume(detectorS, detectorMat, "detectorL");
  detectorL->SetVisAttributes(glassVisAtt);
  new G4PVPlacement(G4Transform3D::Identity,
		    detectorL, "detector", worldL, false, 0, true);

  return physWorld;
}

