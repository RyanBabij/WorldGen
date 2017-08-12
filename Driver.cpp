/* Driver.cpp
	WorldGen
	Project code name: WorldGen.

	This project allows the user to generate worlds. It is used for testing the world generator libs.
*/

const int VERSION = 1005;

//#include "GlobalSettings.cpp"
	// DYNAMICALLY GENERATED HEADER FILE WITH STRING WHICH COUNTS COMPILATIONS.
#include "CompileCount.hpp"

	// DEFINE GRAPHICS API.
	// ANYTHING APART FROM OPENGL WILL PROBABLY NEVER HAPPEN.
#define WILDCAT_USE_OPENGL
//#define WILDCAT_USE_DIRECT3D

	// DEFINE OPERATING SYSTEM. MAC SUPPORT IS NOT CURRENTLY PLANNED.
#define WILDCAT_WINDOWS
//#define WILDCAT_LINUX

	// TRACEABLE CONSOLE OUTPUT MACRO.
#undef FILEID
#define FILEID "WORLDGEN_DRIVER_CPP"
#undef SAY
#define SAY(x) std::cout << FILEID << ":" << __LINE__ << ":" << x << "\n"

#include <File/FileManagerStatic.hpp>

#include <Debug/RetCode.hpp>

//#include <iostream>

#include <WorldGenerator/WorldGenerator2.hpp>

#include <Graphics/Png/Png.hpp>

#include <Data/DataTools.hpp>

#include <Math/BasicMath/BasicMath.hpp>
#include <Misc/ArgReader.hpp>

#include <Container/Table/Table.hpp> // FOR STORING LANDMASS DATA.

int argWorldSize=-1;
bool compressPNG=false;

//bool wrappingMode = true;

bool wrapX=false;
bool wrapY=false;



void printHelp()
{
	std::cout<<"\nWorldgen by Garosoft (garosoft.org)\n";
	std::cout<<"  Worldgen is a command line tool that generates a world map and exports it as an uncompressed PNG.\n";
	std::cout<<"  License: Public domain. This program uses a modified version of LodePNG.\n";
	std::cout<<"  This is an alpha release, and is not fully functional.\n";

	std::cout<<"Options:\n";
	std::cout<<"  -o \"x\". Required. Output the world data as: x.png. Must be alphanumeric. Default value is \"world\".\n";
	std::cout<<"  -s x. Create world of size (x,x). x should be (2^x)+1. Default value: 1025. Large values may crash the program due to memory limitations.\n";
	std::cout<<"  -n x. Generate x number of worlds. Output files will have numbers appended. Default value: 1.\n";
	std::cout<<"  -ocean x. Approximate percentage of the world that will be ocean. Default value: 66. Value of x must be between 0-100.\n";
	std::cout<<"  -seed x. Seed used to generate world. Default value is random. Value will be saved to the text file in case you get a world you want to generate again later. The maximum value for seed is "<<INT_MAX<<" (INT_MAX).\n";
	std::cout<<"  -seedland x.\n";
	std::cout<<"  -i. Enables island mode. The map will have water around the border. Disabled by default.\n";
	std::cout<<"  -c. Compress PNG. Disabled by default.\n";
	std::cout<<"  -wrapX. Wrap the X-axis.\n";
	std::cout<<"  -wrapY. Wrap the Y-axis.\n";
	std::cout<<"  -caves. Optional. Number of caves to spawn into the world. Default value: 64.\n";
	//std::cout<<"  NOT IMPLEMENTED: -id x. Each world has a unique id, if you want to generate a particular world, you can enter its id.\n";

	std::cout<<"\nExample:\n<example>\n";

	std::cout<<"\n";
	std::cout<<"Version "<<VERSION<<".\n";
	std::cout<<"Compiled: "<<__DATE__<<". "<<__TIME__<<".\n";
	std::cout<<"Compile count: "<<COMPILE_COUNT<<".\n";
	std::cout<<"\n";
	
	std::cout<<"Testing Github commits.\n";
}

int main (int nArgs, char ** arg )
{
	std::string outputText = ""; // Useful info to output to a textfile alongside the PNG.

	ArgReader argReader;
	argReader.feed(nArgs,arg);

	int worldSize = 1025;
	int nWorlds = 1;
	std::string outFile = "world";
	//bool sampleMode=false;

	if (argReader.hasTag("-help") || argReader.hasTag("--help") || argReader.hasTag("-h"))
	{
		printHelp();
		return 0;
	}

	if (argReader.hasTag("-o"))
	{
		outFile = argReader.getStringTag("-o");
		outputText += "Output file: "+outFile+".\n";
	}
	
	if ( argReader.hasTag("-wrapX"))
	{
		std::cout<<"Arg: wrapX.\n";
		wrapX=true;
	}
	if ( argReader.hasTag("-wrapY"))
	{
		std::cout<<"Arg: wrapY.\n";
		wrapY=true;
	}
	
	if (argReader.hasTag("-c"))
	{
		std::cout<<"Compress PNG.\n";
		compressPNG=true;
	}

		// clean string to only allow alphanumeric filenames.

	if ( outFile == "" )
	{
		std::cout<<"Error: Invalid output file. Must be alphanumeric.\n";
		return 0;
	}

	if (argReader.hasTag("-s"))
	{
		worldSize = argReader.getIntTag("-s");
		if ( worldSize <=0 || worldSize > 999999999 )
		{
			std::cout<<"Error: Invalid worldsize.\n";
			return 0;
		}
	}

	if (argReader.hasTag("-n"))
	{
		nWorlds = argReader.getIntTag("-n");
		if ( nWorlds <=0 || nWorlds > 16385 )
		{
			std::cout<<"Error: Maximum value for argument -n is 1000.\n";
			return 0;
		}
	}

	// MAKE SOME BIOMES.
	//WorldGenerator2_Biome snow;

	WorldGenerator2 wg;
	wg.wrapX=wrapX;
	wg.wrapY=wrapY;
//(STEPPES, 0.05, 3, 0.86);
	wg.addBiome("Steppes",0.2,3,0.86);
	wg.addBiome("Snow",0.1,1,0.86);
	wg.addBiome("Hilly",0.05,10,0.86);
	wg.addBiome("Wetland",0.2,4,0.86);
	wg.addBiome("Forest",0.28,5,0.86);
	wg.addBiome("Jungle",0.28,2,0.86);
	wg.addBiome("Desert",0.1,1,0.86);
	wg.addBiome("Mountainous",0.05,5,0.86);

	wg.mapSize = worldSize;

		// DEFAULT OCEAN PERCENT VALUE.
	wg.oceanPercent=0.66;

	if (argReader.hasTag("-ocean"))
	{
		const int argOceanPercent = argReader.getIntTag("-ocean");
		if (argOceanPercent <= 100 && argOceanPercent >= 0)
		{
			wg.oceanPercent = (double)argOceanPercent/100;
		}
	}

	wg.seed=0;
	if (argReader.hasTag("-seed"))
	{
		std::cout<<"Commandline argument -seed.\n";
		const int argSeed = argReader.getIntTag("-seed");
		if (argSeed >= 0 && argSeed <= INT_MAX)
		{
			wg.seed = argSeed;
		}
		else
		{
			std::cout<<"Error: seed value of "<<argSeed<<" is invalid.\n";
			return 0;
		}
	}
	
	if ( argReader.hasTag("-seedland"))
	{
		std::cout<<"Commandline argument -seedland.\n";
		const int argSeed = argReader.getIntTag("-seedland");
		if (argSeed >= 0 && argSeed <= INT_MAX)
		{
			std::cout<<"Landform seed set to: "<<argSeed<<".\n";
			wg.landformSeed = argSeed;
		}
		else
		{
			std::cout<<"Error: seedland value of "<<argSeed<<" is invalid.\n";
			return 0;
		}
	}

	wg.mountainPercent=0.025;
	wg.forestPercent=0.25;
	wg.desertPercent=0.33;
	wg.snowPercent=0.04;

	wg.goodPercent=0.15;
	wg.evilPercent=0.15;


	wg.freeSteps=2;
	if (argReader.hasTag("-freesteps"))
	{
		const int argFreeSteps = argReader.getIntTag("-freesteps");
		if (argFreeSteps >= 0 && argFreeSteps <= 1000)
		{
			wg.freeSteps = argFreeSteps;
		}
	}

	if (argReader.hasTag("-i") || argReader.hasTag("-islandMode") || argReader.hasTag("-islandmode"))
	{
		wg.islandMode=true;
		std::cout<<"Island mode active.\n";
	}
	else
	{
		wg.islandMode=false;
	}

	if (argReader.hasTag("-landMode"))
	{
		wg.landMode=true;
	}
	else
	{
		wg.landMode=false;
	}

	wg.variance=400;
	if (argReader.hasTag("-variance"))
	{
		const int argVariance = argReader.getIntTag("-variance");
		if (argVariance >= 0 && argVariance <= 1000)
		{
			wg.variance = argVariance;
		}
	}

	wg.landSmoothing=0.88;
	if (argReader.hasTag("-smoothingPercent"))
	{
		const int argSmoothingPercent = argReader.getIntTag("-smoothingPercent");
		if (argSmoothingPercent >= 0 && argSmoothingPercent <= 100 )
		{
			wg.landSmoothing = (double)argSmoothingPercent/100;
		}
	}


		for ( int i=0;i<nWorlds;++i)
		{
			if ( wg.generate() != WILDCAT_FAIL )
			{
				//wg.createGoodEvil();
				std::cout<<"Exporting PNG.\n";
				if ( nWorlds==1 )
				{
						// DO NOT INCLUDE EXTENSION IN THE FILE.
					wg.exportPNG(outFile);
					
					// Generate save file.
					//outputText+="Save data will go here.\n";
					//outputText+="\nSeed: "+DataTools::toString(wg.seed)+".\n";
					
					//FileManagerStatic::writeString(outputText,outFile+".txt");
				}
				else
				{
					wg.exportPNG(outFile+DataTools::toString(i)+".png");
				}
				

				// EXPORT LANDMASSES

				// TEST: COUNT THE DIFFERENT LANDMASSES.
				
				//#ifdef NOPE
				//std::cout<<"Finding landmasses.\n";
				
				
				//std::cout<<"Building land array.\n";
				
				std::cout<<"Building landmass ID map.\n";
				
				ArrayS2 <bool> aIsLand (wg.mapSize,wg.mapSize,false);
				
				for ( int _y = 0; _y < aIsLand.nY; ++_y )
				{
					for ( int _x = 0; _x < aIsLand.nX; ++_x )
					{
							// ICE WON'T COUNT AS LANDMASS HERE
						if ( wg.aTerrainType(_x,_y) != wg.OCEAN && wg.aTerrainType(_x,_y) != wg.ICE )
						{
							aIsLand(_x,_y) = true;
						}
					}
				}
				
					// FOR SCANLINE FILL WE ONLY NEED 1 ARRAY. -1 = UNFILLED SO FAR. 0 = OCEAN. FILL ALL -1S AS YOU FIND THEM.
					// PROTIP: OCEANS CAN BE ID'D LATER TOO.
					
				ArrayS2 <int> aLandID (wg.mapSize, wg.mapSize, -1);
				
				// FILL IN THE OCEAN BARRIERS.
				for ( int _y = 0; _y < aIsLand.nY; ++_y )
				{
					for ( int _x = 0; _x < aIsLand.nX; ++_x )
					{
							// ICE WON'T COUNT AS LANDMASS HERE
						if ( wg.aTerrainType(_x,_y) == wg.OCEAN )
						{
							aLandID(_x,_y) = 0;
						}
					}
				}
				
						// *** EXPORT SCANLINE FILL TO ARRAY FUNCTION....
				

				// MAKE A LANDMASS ID ARRAY
				ArrayS2 <int> aLandmassID (wg.mapSize,wg.mapSize,-1);
				
				// VECTORS OF LANDMASS STATS
					// SIZE
				Vector <int> vLandmassSize;
				int landMassSize = 0;
				
				
				int currentID = 1;
				for ( int _y = 0; _y < aLandmassID.nY; ++_y )
				{
					for ( int _x = 0; _x < aLandmassID.nX; ++_x )
					{
						//std::cout<<".";
						if ( aLandmassID(_x,_y) == -1 && aIsLand(_x,_y) == true )
						{
						//	std::cout<<"New landmass.\n";
							
							
							
							
							
							
							
							// WE MUST PORT THIS CODE TO MAKE ID MAPS OF ANY BIOME.
							
      /* vectors of coordinates to be checked */
      Vector <int> vX;
      Vector <int> vY;
      int currentV=-1;
      int posX=_x;
	  int posY=_y;
      //heightMap->getIndexCoords(pos, &posX, &posY);
      /* scanline fill loop */
      while (true)
      {
        const int INITIAL_X=posX;
        /* go left to find the leftmost tile for this row which meets the criteria for water, filling along the way */
        /* we also check the tile directly above or below to see if there are any tiles that can be filled on those rows. */
        /* If we find such tiles, then we store the leftmost ones, for both the upper and lower row. */
        /* NOTE: y coords seems to encode upside-down, so we need to work upside-down here. */
        /* Fill left */
        while ( posX>=0 && aIsLand(posX,posY)==true )
        {
          aLandmassID(posX,posY)=currentID;
		  ++landMassSize;
          //aLandmassID(posX,posY)=0;
		  
		 // std::cout<<"Filling left: ["<<posX<<","<<posY<<"]\n";
		  
		  
		  
          /* check below */
          /* If we're not on the bottom row, and the below tile is land. */
          if(posY<aIsLand.nY-1 && aIsLand(posX,posY+1)==true )
          {

            /* If we are at the leftmost or rightmost column of the map. */
            if(posX==0 || posX==aIsLand.nX-1 )
            {
              /* push tile below us if it's valid. */
              if(aLandmassID(posX,posY+1)==-1)
              {
			//	   std::cout<<"1. Push: ["<<posX<<","<<posY+1<<"]\n";
				  vX.push(posX); vY.push(posY+1);
					//vX.push(posX+1); vY.push(posY+1);
				}
            }
            /* if we aren't at the leftmost or rightmost column of the map. */
            else
            {
              /* if the bottom-left tile is water, and the below tile has not been processed. */
				
			  
              if ( aIsLand(posX-1,posY+1)==false && aLandmassID(posX,posY+1)==-1 )
              { 
		 // std::cout<<"2. Push: ["<<posX<<","<<posY+1<<"]\n";
			vX.push(posX); vY.push(posY+1);
				//vX.push(posX-1); vY.push(posY+1);
				//vX.push(posX+1); vY.push(posY+1);
			//vX.push(posX+1); vY.push(posY+1);
			}
            }
          }
          /* check above */
          /* If we're not on the top row, and the above tile is water. */
          if(posY>0 && aIsLand(posX,posY-1)==true )
          {

            /* If we are at the leftmost or rightmost column of the map. */
            if(posX==0 || posX==aIsLand.nX-1 )
            {
              /* push tile above us if it's valid. */
              if(aLandmassID(posX,posY-1)==-1)
              {
				 // std::cout<<"3. Push: ["<<posX<<","<<posY-1<<"]\n";
				  vX.push(posX); vY.push(posY-1);
              }
            }
            /* if we aren't at the leftmost or rightmost column of the map. */
            else
            {
              /* if the top-left tile is land, and the above tile has not been processed. */
              if ( aIsLand(posX-1,posY-1)==false && aLandmassID(posX,posY-1)==-1 )
              {
				//  std::cout<<"4. Push: ["<<posX<<","<<posY-1<<"]\n";
				  vX.push(posX); vY.push(posY-1);
					//vX.push(posX+1); vY.push(posY-1);
					//vX.push(posX-1); vY.push(posY-1);
				  }
            }
          }
          --posX; /* move left 1 tile */
        }
        /* check the last X for this scan. */
		// NOTE: I THINK SWITCHING TO A DO WHILE MIGHT REMOVE THE NEED FOR THIS BIT.
		
        ++posX;
        /* check below */
        /* If we're not on the bottom row, and the below tile is water, and the below tile hasn't been processed yet. */
        if(posY<aIsLand.nY-1 && aIsLand(posX,posY+1)==true && aLandmassID(posX,posY+1)==-1 )
        {
          /* push coords onto vectors. */
		 // std::cout<<"5. Push: ["<<posX<<","<<posY+1<<"]\n";
          vX.push(posX); vY.push(posY+1);
          //vX.push(posX+1); vY.push(posY+1);
          //vX.push(posX-1); vY.push(posY+1);
        }

		
        /* check above */
        /* If we're not on the top row, and the above tile is water, and the above tile hasn't been processed yet. */
        if(posY>0 && aIsLand(posX,posY-1)==true && aLandmassID(posX,posY-1)==-1)
        {
          /* push coords onto vectors. */
		//  std::cout<<"6. Push: ["<<posX<<","<<posY-1<<"]\n";
          vX.push(posX); vY.push(posY-1);
        }
		
		
		// DIAGONALS: SAME BUT CHECK BELOW-LEFT
        if(posY<aIsLand.nY-1 && posX>0 && aIsLand(posX-1,posY+1)==true && aLandmassID(posX-1,posY+1)==-1 )
        {
          vX.push(posX-1); vY.push(posY+1);
        }
		
		// DIAGONALS: SAME BUT CHECK ABOVE-LEFT
        if(posY>0 && posX>0 && aIsLand(posX-1,posY-1)==true && aLandmassID(posX-1,posY-1)==-1 )
        {
          vX.push(posX-1); vY.push(posY-1);
        }
		
		// --posX; //KLUDGE?
		// // DIAGONALS: SAME BUT CHECK BELOW-LEFT
        // if(posY<aIsLand.nY-1 && posX>0 && aIsLand(posX-1,posY+1)==true && aLandmassID(posX-1,posY+1)==-1 )
        // {
          // vX.push(posX-1); vY.push(posY+1);
        // }
		
		// // DIAGONALS: SAME BUT CHECK ABOVE-LEFT
        // if(posY>0 && posX>0 && aIsLand(posX-1,posY-1)==true && aLandmassID(posX-1,posY-1)==-1 )
        // {
          // vX.push(posX-1); vY.push(posY-1);
        // }
		
			
		
		if ( posX == INITIAL_X )
		{
		//		std::cout<<"SPECIAL CASE: PosX == INITIAL_X\n";
		}
		
        posX=INITIAL_X+1;
        /* Go back to the initial x position, and now fill right */
        while ( posX < aIsLand.nX && aIsLand(posX,posY)==true && aLandmassID(posX,posY)==-1 )
        {
		//	std::cout<<"Filling right: ["<<posX<<","<<posY<<"]\n";
          aLandmassID(posX,posY)=currentID;
		  ++landMassSize;
          /* check below */
          /* If we're not on the bottom row, and the below tile is water. */
          if(posY<aIsLand.nY-1 && aIsLand(posX,posY+1)==true )
          {
            /* If we are at the leftmost or rightmost column of the map. */
            if(posX==0 || posX==aIsLand.nX-1 )
            {
              /* push tile below us if it's valid. */
              if(aLandmassID(posX,posY+1)==-1)
              { vX.push(posX); vY.push(posY+1); }
            }
            /* if we aren't at the leftmost or rightmost column of the map. */
            else
            {
              /* if the bottom-left tile is land, and the below tile has not been processed. */
              if ( aIsLand(posX-1,posY+1)==false && aLandmassID(posX,posY+1)==-1 )
              { vX.push(posX); vY.push(posY+1); }
            }
          }
          /* check above */
          /* If we're not on the top row, and the above tile is water. */
          if(posY>0 && aIsLand(posX,posY-1)==true )
          {
            /* If we are at the leftmost or rightmost column of the map. */
            if(posX==0 || posX==aIsLand.nX-1 )
            {
              /* push tile above us if it's valid. */
              if(aLandmassID(posX,posY-1)==-1)
              { vX.push(posX); vY.push(posY-1);
              }
            }
            /* if we aren't at the leftmost or rightmost column of the map. */
            else
            {
              /* if the top-left tile is land, and the above tile has not been processed. */
              if ( aIsLand(posX-1,posY-1)==false && aLandmassID(posX,posY-1)==-1 )
              { vX.push(posX); vY.push(posY-1); }
            }
          }
          /* move right 1 tile */
          ++posX;
        }
			// DIAGONALS CHECK
		// DIAGONALS: SAME BUT CHECK BELOW-RIGHT
        // if(posY<aIsLand.nY-1 && posX>0 && aIsLand(posX-1,posY+1)==true && aLandmassID(posX-1,posY+1)==-1 )
        // {
          // vX.push(posX-1); vY.push(posY+1);
        // }
		// // DIAGONALS: SAME BUT CHECK ABOVE-RIGHT
        // if(posY<aIsLand.nY-1 && posX>0 && aIsLand(posX-1,posY+1)==true && aLandmassID(posX-1,posY+1)==-1 )
        // {
          // vX.push(posX-1); vY.push(posY+1);
        // }
		
		//KLUDGE. DO WHILE MAYBE IS SMARTER. MAYBE.
--posX;
// DIAGONALS: CHECK BELOW-RIGHT
if(posY<aIsLand.nY-1 && posX<aIsLand.nX-1 && aIsLand(posX+1,posY+1)==true && aLandmassID(posX+1,posY+1)==-1 )
{
  vX.push(posX+1); vY.push(posY+1);
}

// DIAGONALS: CHECK ABOVE-RIGHT
if(posY>0 && posX<aIsLand.nX-1 && aIsLand(posX+1,posY-1)==true && aLandmassID(posX+1,posY-1)==-1 )
{
  vX.push(posX+1); vY.push(posY-1);
}
		
        /* entire row is complete. Go to next set of coords. */
        /* Look for some good coords on the vector. */
        ++currentV;
		//std::cout<<"Moving to next vector.\n";
        for (;currentV<vX.size();++currentV)
        {
          if ( aLandmassID(vX(currentV),vY(currentV)) == -1 )
          {
            posX=vX(currentV);
            posY=vY(currentV);
            break;
          }
        }
        //std::cout<<"\n";
        if(currentV==vX.size())
        { break; }
      }
      vX.clear();
      vY.clear();
	  vLandmassSize.push(landMassSize);
	  landMassSize=0;
      ++currentID;					
							
							
											
							
							
							
							
							
							
							
							
							
							//if ( wg.aTerrainType(_x,_y) != wg.OCEAN )
							//{
								// FLOOD FILL HERE.
								//Vector <HasXY> * vLandmass = wg.aTerrainType.floodFillVector ( _x, _y , true );

								// FILL WITH THE VECTOR COORDS.
								//for ( int i2=0 ; i2<vLandmass->size(); ++i2)
								//{
									//aLandmass((*vLandmass)(i2))=currentID;
								//}
								
								//delete vLandmass;
								//++currentID;
							//}
						}
						
					}
				}
				
				//std::cout<<"CurrentID is: "<<currentID<<".\n";
				
				
				
				int totalLand = 0;
				
					// A table has some sorting functions.
				Table tLandmass;
					// Landmass name.
				tLandmass.addStringColumn();
					// Landmass size.
				tLandmass.addIntColumn();
				
				//std::cout<<"The largest landmass is: x tiles.\n";
				for ( int i2=0;i2<vLandmassSize.size();++i2)
				{
					//std::cout<<"Landmass "<<i2<<" is "<<vLandmassSize(i2)<<" tiles big. ("<<vLandmassSize(i2)*25<<"km^2).\n";
					totalLand +=vLandmassSize(i2);
					tLandmass.pushData(0,"Name");
					tLandmass.pushData(1,vLandmassSize(i2));
				}
				std::cout<<"Total land area: "<<totalLand*25<<"km^2.\n";
				
				tLandmass.sortDescendingBy(1);
				tLandmass.cout();
				
				std::cout<<"There are "<<vLandmassSize.size()<<" landmasses.\n";
				std::cout<<"The average landmass has "<<totalLand/tLandmass.nRows()<<" tiles.\n";
				std::cout<<"The mean landmass has "<<tLandmass.get(1,tLandmass.nRows()/2)<<" tiles.\n";
				//STANDARD DEVIATION
				// CLUSTERING
				
				//int largestLandmass = DataTools::toInt(tLandmass.get(0,1));
				//int secondLargestLandmass = DataTools::toInt(tLandmass.get(1,1));
				std::cout<<"The largest landmass has "<<tLandmass.get(1,0)<<" tiles.\n";
				//std::cout<<"The largest landmass has "<<DataTools::toInt(tLandmass.get(1,0))<<" tiles.\n";
				int largestLandmass = DataTools::toInt(tLandmass.get(1,0));
				std::cout<<"The second largest landmass has "<<tLandmass.get(1,1)<<" tiles.\n";
				//std::cout<<"The second largest landmass has "<<DataTools::toInt(tLandmass.get(1,1))<<" tiles.\n";
				int secondLargestLandmass = DataTools::toInt(tLandmass.get(1,1));
				
				double secondPercent = (double)secondLargestLandmass/(double)largestLandmass * 100;
				
				// LARGE LANDMASS/MAINLAND CRITERIA: 80% OF LARGEST.
				// CONTINENT CRITERIA: 33% OF LARGEST.
				// LARGE ISLAND CRITERIA: 10% OF LARGEST.
				// MEDIUM ISLAND CRITERIA: 1% OF LARGEST.
				// SMALL ISLAND CRITERIA: ANYTHING ELSE.
				int nLargeLandmass = 0;
				int largeLandmassCriteria = largestLandmass*0.8;
				int criteriaContinent = largestLandmass*0.33;
				int crieteriaLargeIsland = largestLandmass*0.1;
				int crieteriaMediumIsland = largestLandmass*0.01;
				int nContinent=0;
				int nLargeIsland=0;
				int nMediumIsland=0;
				int nSmallIsland=0;
				
				// DYNAMIC LANDMASS CLUSTERING. WORKS QUITE WELL.
				// CATEGORY 1: ANYTHING 75% OF THE LARGEST.
				// ERASE ALL CATEGORY 1.
				// CATEGORY 2: ANYTHING 75% OF THE LARGEST.
				// REPEAT FOR CATEGORY 3 and 4.
				// CATEGORY 5 IS REMAINING LANDMASSES.
				
				Vector <int> * vLandmassSizeSorted = vLandmassSize.getIndexesDescending();
				int sizeCategory = 1;
				
				double tolerance = 0.5;
				
				while (vLandmassSizeSorted->size()>0 && sizeCategory < 5)
				{
					//std::cout<<"Tolerance: "<<tolerance<<".\n";
					int nCategory=1;
					//std::cout<<"Size category: "<<sizeCategory<<".\n";
					int currentLargest = vLandmassSize((*vLandmassSizeSorted)(0));
					vLandmassSizeSorted->eraseSlot(0);
					//std::cout<<"Currentlargest: "<<currentLargest<<".\n";
					//std::cout<<"Criteria size: "<<(double)currentLargest*tolerance<<".\n";
					
					Vector <int> vIndexesToDelete;
					
					for ( int i2=0;i2<vLandmassSizeSorted->size();++i2)
					{
						if ( vLandmassSize((*vLandmassSizeSorted)(i2)) >= (double)currentLargest*tolerance )
						{
								//std::cout<<"Adding: "<<vLandmassSize((*vLandmassSizeSorted)(i2))<<".\n";
							++nCategory;
							(*vLandmassSizeSorted)(i2)=0;
							//vLandmassSizeSorted->eraseSlot(i2);
							//--i2;
							//vIndexesToDelete.push(
						}
						else
						{
							//std::cout<<"Not adding: "<<
						}
					}
					std::cout<<"In category "<<sizeCategory<<": "<<nCategory<<".\n";
					
					vLandmassSizeSorted->removeNulls();
					//std::cout<<"Index size: "<<vLandmassSizeSorted->size()
					++sizeCategory;
					tolerance*=0.5;
					//if ( sizeCategory
					//Vector <int>* vIndex = vColumn(_column)->sortDescending();
				}
				//std::cout<<"In remainder: "<<vLandmassSizeSorted->size()<<".\n";	
				delete vLandmassSizeSorted;
				
				for ( int i2=0;i2<vLandmassSize.size();++i2)
				{
					int landmassSize = vLandmassSize(i2);
					//std::cout<<"lms: "<<landmassSize<<".\n";
					if ( landmassSize >= largeLandmassCriteria)
					{
						nLargeLandmass++;
					}
					else if ( landmassSize>= criteriaContinent)
					{
						nContinent++;
					}
					else if ( landmassSize>= crieteriaLargeIsland)
					{
						nLargeIsland++;
					}
					else if ( landmassSize >= crieteriaMediumIsland )
					{
						++nMediumIsland;
					}
					else
					{
						++nSmallIsland;
					}
				}
				
				std::cout<<"Percent: "<<secondPercent<<"%.\n";
				
				// if ( secondPercent > 80 )
				// {
					// std::cout<<"This land has more than one primary landmass.\n";
				// }
				// else if ( secondPercent > 50 )
				// {
					// std::cout<<"This world has one primary landmass.\n";
				// }
				// else if (secondPercent > 25 )
				// {
					// std::cout<<"This world has one main landmass and many small islands.\n";
				// }
				// else
				// {
					// std::cout<<"One landmass dominates the world.\n";
				// }
				
				std::cout<<"This world has "<<nLargeLandmass<<" mainlands.\n";
				std::cout<<"This world has "<<nContinent<<" continents.\n";
				std::cout<<"This world has "<<nLargeIsland<<" large islands.\n";
				std::cout<<"This world has "<<nMediumIsland<<" medium islands.\n";
				std::cout<<"This world has "<<nSmallIsland<<" small islands.\n";

				//for ( int i=0;i<
				//std::cout<<"\n";

				//std::cout<<"Exporting thumbnail.\n";
				//ArrayS3 <unsigned char> * aThumbnail = wg.aTopoMap.resizeTo(64,64,3);

				// EXPORT THE LANDMASS ID MAP.
				ArrayS3 <unsigned char> aTopoMap (wg.mapSize,wg.mapSize,3,0);
				
				Vector <unsigned char> vRed;
				Vector <unsigned char> vGreen;
				Vector <unsigned char> vBlue;
				
					// RANDOMISE COLOURS FOR LANDMASSES
				for ( int i2=0;i2<currentID;++i2)
				{
				// if ( i2<256 )
				// {
					// vRed.push(i2);
					// vGreen.push(i2);
					// vBlue.push(i2);
				// }
				// else
				// {
					// vRed.push(255);
					// vGreen.push(255);
					// vBlue.push(255);
				// }
					vRed.push(Random::randomInt(255));
					vGreen.push(Random::randomInt(255));
					vBlue.push(Random::randomInt(255));
				}
				
				for (unsigned int _y=0;_y<aTopoMap.nY;++_y)
				{
					for (unsigned int _x=0;_x<aTopoMap.nX;++_x)
					{
						if (aLandmassID(_x,_y)==-1)
						{
							aTopoMap(_x,_y,0)=0;
							aTopoMap(_x,_y,1)=0;
							aTopoMap(_x,_y,2)=0;
						}
						else
						{
							int ID = aLandmassID(_x,_y);
							aTopoMap(_x,_y,0)=vRed(ID);
							aTopoMap(_x,_y,1)=vGreen(ID);
							aTopoMap(_x,_y,2)=vBlue(ID);
						}
						//aTopoMap(_x,_y,0)=vRed(
					}
				}
				Png png;
				png.encodeS3("world-landID.png",&aTopoMap);
				//typedef std::tuple<int,int,int> i3tuple;
				//Vector <
				

				//Png png;
				//png.encodeS3("world-thumb.png",aThumbnail);
				
				//#endif
			}
			else
			{
					std::cout<<"Worldgen failed for some reason.\n";
			}
		}

	if (argReader.noArgs()==true)
	{
		printHelp();
	}

	return 0;
}