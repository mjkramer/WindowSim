#pragma once

#include <G4VUserTrackInformation.hh>

struct TrackInformation : G4VUserTrackInformation {
  double iActXcm = 0;
  double prodXcm = 0;
  bool pi0     = false;
  // G4ThreeVector prodLoc;
};
