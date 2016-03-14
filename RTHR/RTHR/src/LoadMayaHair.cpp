#include "Hair.h"
#include "SimpleMath.h"
#include <fstream>
#include <sstream>

using namespace DirectX;

namespace RTHR
{
	typedef vector<Vector3> HairStrand;

	bool Hair::loadHairVertex(string dir, HairStyle style)
	{
		char filename[MAX_PATH];

		TCHAR HairStrandsTextFile[MAX_PATH];
		sprintf_s(filename, 100, "%s\\%s", dir, "hair_vertices.txt");
		
		//read the files

		//Read each hair strand and push it into strand list
		vector<HairStrand> hairStrands;
		vector<Vector2> hairTexture;

		// The coordinates for global coordinate system
		double x, y, z;

		// The texture coordinates
		double u, v;

		ifstream infile;
		infile.open(filename);

		if (!infile)
			return false;

		char c[MAX_PATH];
		char tempChar;
		int totalCVs = 0;

		while (infile.eof())
		{
			// Hair strands are defined as beginning with "//"
			infile.getline(c, MAX_PATH);
			string s(c);
			string::size_type loc = s.find('/', 0);
			if (loc != string::npos)
			{
				istringstream vertexString(s);

				//The number of vertices, the first number in the text
				int vertexCount;
				vertexString >> vertexCount;
				
				//Get texture coordinates
				infile.getline(c, MAX_PATH);
				string s2(c);
				istringstream texString(s2);
				texString >> tempChar >> u >> v;
				hairTexture.push_back(Vector2(u, v));

				//Create hairstrand
				HairStrand strand;
				//Iterate through getting each vertex
				for (uint16 i = 0; i < vertexCount; i++)
				{
					infile.getline(c, MAX_PATH);
					string h(c);
					istringstream vertexString(h);
					vertexString >> x >> y >> z;
					//Maya needs to have z axis flipped
					Vector3 hairVertex(x, y, -z);

					if (i < 13)
					{
						switch (style)
						{
						case HairStyle::LONG:
							strand.push_back(hairVertex);
							totalCVs++;
							break;
						case HairStyle::MEDIUM:
							break;
						case HairStyle::SHORT:
							break;
						case HairStyle::NONE:
							break;
						default:
							break;
						}
					}
					hairStrands.push_back(strand);
				}
			}
		}
	}
}