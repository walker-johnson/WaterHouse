//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4Material;
class DetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  DetectorConstruction();
  ~DetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  void SetSize     (G4double, G4double, G4double);              
  void SetMaterial (G4String);
    

  G4Material* 
  MaterialWithSingleIsotope(G4String, G4String, G4double, G4int, G4int);
         
  const
  G4VPhysicalVolume* GetWorld()      {return worldP;};           
                          
  G4Material*        GetMaterial()   {return fMaterial;};
  G4double           GetSize()       {return fBoxX;};
  void               PrintParameters();

  //world
  G4LogicalVolume* worldL;
  G4VPhysicalVolume* worldP;
  //room
  G4LogicalVolume* roomL;
  G4VPhysicalVolume* roomP;

  

private:

  G4double fBoxX;
  G4double fBoxY;
  G4double fBoxZ;
  G4double fRoom_x;
  G4double fRoom_y;
  G4double fRoom_z;
  G4double fTank_x;
  G4double fTank_y;
  G4double fTank_z;
  G4double fChamber_x;
  G4double fChamber_y;
  G4double fChamber_z;
  G4double fSideThk;
  G4double fTopThk;
  G4double fInc;
  G4Material* fMaterial;
  DetectorMessenger* fDetectorMessenger;


  //tank
  G4LogicalVolume* tankL;
  G4VPhysicalVolume* tankP;
  //chamber
  G4LogicalVolume* chamberL;
  G4VPhysicalVolume* chamberP;
  //gaps
  G4LogicalVolume* gapL;
  
    
  void               DefineMaterials();
  G4VPhysicalVolume* ConstructVolumes();     
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif

