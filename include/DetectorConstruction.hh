#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4PhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction(const char *gdmlFile);
  virtual G4VPhysicalVolume* Construct();

private:
  const char *gdmlFile;
};

#endif
