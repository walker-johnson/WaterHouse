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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4VSolid.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:G4VUserDetectorConstruction(),
 worldP(0), worldL(0), fMaterial(0), fDetectorMessenger(0)
{
  fTank_x = 7*2.5*9*cm;
  fTank_y = 9*2.5*9*cm;
  fTank_z = 18*6*cm;
  fBoxX = fTank_x+2*m; //World size X
  fBoxY = fTank_y+2*m; //World size Y
  fBoxZ = fTank_z+1*m;  //World size Z
  fRoom_x = fTank_x+1*m;
  fRoom_y = fTank_y+1*m;
  fRoom_z = fTank_z+0.5*m;
  fSideThk = 9*2.5*2*cm;
  fTopThk = 3*18*cm;
  fChamber_x = fTank_x - 2*fSideThk;
  fChamber_y = fTank_y - 2*fSideThk;
  fChamber_z = fTank_z - fTopThk;
  fInc = 0.25*m;
  DefineMaterials();
  SetMaterial("G4_AIR");   //Sets the material of the world
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ delete fDetectorMessenger;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // specific element name for thermal neutronHP
  // (see G4ParticleHPThermalScatteringNames.cc)

  G4int ncomponents, natoms;

  // pressurized water
  G4Element* H  = new G4Element("TS_H_of_Water" ,"H" , 1., 1.0079*g/mole);
  G4Element* O  = new G4Element("Oxygen"        ,"O" , 8., 16.00*g/mole);
  G4Material* H2O = 
  new G4Material("Water_ts", 1.000*g/cm3, ncomponents=2,
                         kStateLiquid, 593*kelvin, 150*bar);
  H2O->AddElement(H, natoms=2);
  H2O->AddElement(O, natoms=1);
  H2O->GetIonisation()->SetMeanExcitationEnergy(78.0*eV);
  
  // heavy water
  G4Isotope* H2 = new G4Isotope("H2",1,2);
  G4Element* D  = new G4Element("TS_D_of_Heavy_Water", "D", 1);
  D->AddIsotope(H2, 100*perCent);  
  G4Material* D2O = new G4Material("HeavyWater", 1.11*g/cm3, ncomponents=2,
                        kStateLiquid, 293.15*kelvin, 1*atmosphere);
  D2O->AddElement(D, natoms=2);
  D2O->AddElement(O, natoms=1);
  
  // graphite
  G4Isotope* C12 = new G4Isotope("C12", 6, 12);  
  G4Element* C   = new G4Element("TS_C_of_Graphite","C", ncomponents=1);
  C->AddIsotope(C12, 100.*perCent);
  G4Material* graphite = 
  new G4Material("graphite", 2.27*g/cm3, ncomponents=1,
                         kStateSolid, 293*kelvin, 1*atmosphere);
  graphite->AddElement(C, natoms=1);

  
 ///G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* DetectorConstruction::MaterialWithSingleIsotope( G4String name,
                           G4String symbol, G4double density, G4int Z, G4int A)
{
 // define a material from an isotope
 //
 G4int ncomponents;
 G4double abundance, massfraction;

 G4Isotope* isotope = new G4Isotope(symbol, Z, A);
 
 G4Element* element  = new G4Element(name, symbol, ncomponents=1);
 element->AddIsotope(isotope, abundance= 100.*perCent);
 
 G4Material* material = new G4Material(name, density, ncomponents=1);
 material->AddElement(element, massfraction=100.*perCent);

 return material;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4bool checkOverlaps = true;        //option to check for overlapping geometry
  
  G4Box*
  worldS    = new G4Box("World",                             //its name
                        fBoxX/2,fBoxY/2,fBoxZ/2);            //its dimensions

  worldL    = new G4LogicalVolume(worldS,                    //its shape
                                  fMaterial,                 //its material
                                  "World");                  //its name

  worldP    = new G4PVPlacement(0,                          //no rotation
                                G4ThreeVector(),   //at 0,0,0
                                worldL,                     //its logical volume
                                "World",                    //its name
                                0,                          //its mother  volume
                                false,                      //no boolean operation
 			        0,                          //copy number
				checkOverlaps);             //option to check for overlaps

  G4Box* roomS = new G4Box("Room",
			   fRoom_x/2,
			   fRoom_y/2,
			   fRoom_z/2);

  roomL = new G4LogicalVolume(roomS,
			      fMaterial,
			      "Room");


  roomP = new G4PVPlacement(0,
			    G4ThreeVector(0,0,-fBoxZ/2+fRoom_z/2),
			    roomL,
			    "Room",
			    worldL,
			    false,
			    0,
			    checkOverlaps);


  G4Material* water = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
  
  G4Box* tankS = new G4Box("tank",
			   fTank_x/2,
			   fTank_y/2,
			   fTank_z/2);

  tankL = new G4LogicalVolume(tankS,
			      water,
			      "Tank");

  tankP = new G4PVPlacement(0,
			    G4ThreeVector(0,0,-fRoom_z/2+fTank_z/2),
			    tankL,
			    "Tank",
			    roomL,
			    false,
			    0,
			    checkOverlaps);


  G4Box* chamberS = new G4Box("Chamber",
			      fChamber_x/2,
			      fChamber_y/2,
			      fChamber_z/2);

  chamberL = new G4LogicalVolume(chamberS,
				 fMaterial,
				 "Chamber");

  chamberP = new G4PVPlacement(0,
			      G4ThreeVector(0,0,-fTank_z/2 + fChamber_z/2),
			      chamberL,
			      "Chamber",
			      tankL,
			      false,
			      0,
			      checkOverlaps);

  G4VSolid* gapS = new G4Tubs("Gap",
			    0,
			    0.5*cm,
			    (fTank_x/2 - fChamber_x/2)/2,
			    0.0*deg, 360.0*deg);

  gapL = new G4LogicalVolume(gapS,
			     fMaterial,
			     "Gap");

  G4PVPlacement* gapsP [24];

  G4RotationMatrix* rMatrix = new G4RotationMatrix();
  rMatrix->rotateY(90.*deg);

  for(int i =0; i< 6; i++){
    for(int j=0; j<4; j++){
      //placements in here
      std::string stri = std::to_string(i);
      std::string strj = std::to_string(j);
      std::string name = "gap";
      std::string name2 = name.append(stri);
      std::string name3 = name2.append(strj);
      G4double yTrans = -fTank_x/2 + (fTank_x/2-fChamber_x/2)/2 + i*22.5*cm; 
      G4double zTrans = -j*15*cm; 
      G4double xTrans = -fTank_y/2+(fTank_x/2 - fChamber_x/2);
      gapsP[i+j] = new G4PVPlacement(rMatrix,
				     G4ThreeVector(xTrans, yTrans, zTrans),
				     gapL,
				     name3,
				     tankL,
				     false,
				     0,
				     checkOverlaps);
    }
  }
			     
			    
		 
			   

  
  //always return the root volume
  //
  return worldP;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void DetectorConstruction::PrintParameters()
{
  G4cout << "\n The World is " << G4BestUnit(fBoxX,"Length")
	 << "x" << G4BestUnit(fBoxY, "Length")
	 << "x" << G4BestUnit(fBoxY, "Length")
         << " of " << fMaterial->GetName() 
         << "\n \n" << fMaterial << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMaterial(G4String materialChoice)
{
  // search the material by its name
  G4Material* pttoMaterial =
     G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);   
  
  if (pttoMaterial) { 
    if(fMaterial != pttoMaterial) {
      fMaterial = pttoMaterial;
      if(worldL) { worldL->SetMaterial(pttoMaterial); }
      G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    }
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetMaterial : "
           << materialChoice << " not found" << G4endl;
  }              
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetSize(G4double x, G4double y, G4double z)
{
  fBoxX = x;
  fBoxY = y;
  fBoxZ = z;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

