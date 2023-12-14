#define NONE 0
#define INDIVIDUAL 1
#define GLOBAL 2

#define POLYNOMIAL 0
#define PIECEWISE_LINEAR 1
#define PIECEWISE_POLYNOMIAL 2

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <time.h>
#include <sstream> 
#include <iomanip> 
#include <algorithm>
#include <cmath>

#ifdef __linux__ 
#include <sys/stat.h>
#else
#include <direct.h>
#endif

#include <pugixml.hpp>

using namespace std;

const char* getCmdOption(const char ** begin, const char ** end, const std::string & option)
{
	const char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(const char** begin, const char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

int gcd(int a, int b)
{
	// Everything divides 0  
	if (a == 0)
		return b;
	if (b == 0)
		return a;

	// base case 
	if (a == b)
		return a;

	// a is greater 
	if (a > b)
		return gcd(a - b, b);
	return gcd(a, b - a);
}

vector<int> hardSubsetEvenOdd(int n, int E, int & W)
{
	W = 2 * floor(n*E / 8.0) + 1;
	vector<int> w(0);
	for (int j = 1; j <= n; j++)
	{
		w.push_back(2*(1 + rand() % (E/2)));
	}

	return w;
}

vector<int> hardSubsetF2(int n, int E, int v, int phi, int & W)
{
	int a0, a1;

	do
	{
		a0 = 2 + rand() % (int)(pow(10, E)-1);
		a1 = 2 + rand() % (int)(pow(10, E) - 1);
		W = (a0 - 1)*(a1 - 1) - 1;
	} while ((gcd(a0, a1) != 1) || (W < pow(10, E)*(phi-v)) || (W > pow(10, E)*(phi)));

	vector<int> w(n,0);
	for (int i = 1; i <= n/2; i++)
	{
		w[2 * i - 2] = ( 1 + (i - 1) % ( (int)floor( ((double)W) / ((double)a0)) ) )*a0;
		w[2 * i - 1] = (1 + (i - 1) % ((int)floor(((double)W) / ((double)a1))))*a1;
	}

	return w;
}

int main(int argc, const char* argv[])
{
	stringstream aux;

	int seed = (int)time(0);

	int n = 0;
	int m = 0;
	int numInstances = 1;
	string instancePrefix = "";

	int fixedCosts = 0;
	int priceLimit = NONE;
	int weightFunction = 0;
	int funcDegree = 1;
	int numPieces = 0;
	string datasetName = "";

	if (cmdOptionExists(argv, argv + argc, "-help"))
	{
		cout << "Usage:" << endl;
		cout << "-n <size of instance>" << endl;
		cout << "-m <number of bins>" << endl;
		cout << "-seed <random seed>" << endl;
		cout << "-numinstances <how many random instance should be generated>" << endl;
		cout << "-fixedcosts <fixed cost type>" << endl;
		cout << "\t 0: None" << endl;
		cout << "\t 1: Only costs" << endl;
		cout << "\t 2: Costs and profits" << endl;
		cout << "-pricelimit <price limit behaviour>" << endl;
			cout << "\t 0: None" << endl;
			cout << "\t 1: Individual price limit" << endl;
			cout << "\t 2: Single price limit" << endl;
		cout << "-weight <>" << endl;
			cout << "\t " << POLYNOMIAL << ": Polynomial (please specify degree)" << endl;
			cout << "\t " << PIECEWISE_LINEAR << ": Piecewise Linear (please specify number of pieces)" << endl;
			cout << "\t " << PIECEWISE_POLYNOMIAL << ": Piecewise Polynomial (please specify degree and number of pieces)" << endl;
		cout << "-degree <weight function degree for the polynomial and piecewise polynomial case>" << endl;
		cout << "-numpieces <weight function number of pieces for the piecewise and piecewise polynomial case>" << endl;
		cout << "-dataset <desired dataset name>" << endl;

		return 0;
	}

	const char * arg = getCmdOption(argv, argv + argc, "-fixedcosts");
	if (arg)
		fixedCosts = atoi(arg);

	arg = getCmdOption(argv, argv + argc, "-n");
	if (arg)
		n = atoi(arg);

	arg = getCmdOption(argv, argv + argc, "-m");
	if (arg)
		m = atoi(arg);

	arg = getCmdOption(argv, argv + argc, "-numinstances");
	if (arg)
		numInstances = atoi(arg);

	arg = getCmdOption(argv, argv + argc, "-pricelimit");
	if (arg)
		priceLimit = atoi(arg);

	arg = getCmdOption(argv, argv + argc, "-weight");
	if (arg)
		weightFunction = atoi(arg);

	arg = getCmdOption(argv, argv + argc, "-degree");
	if (arg)
		funcDegree = atoi(arg);

	arg = getCmdOption(argv, argv + argc, "-dataset");
	if (arg)
		datasetName = arg;
	
	arg = getCmdOption(argv, argv + argc, "-numpieces");
	if (arg)
		numPieces = atoi(arg);
	
	if (n == 0)
	{
		cout << "error: please provide the instance size with the argument -n\nUse -help for more information\n";
		return(-1);
	}
	if (m == 0)
	{
		cout << "error: please provide the number of bins with the argument -m\nUse -help for more information\n";
		return(-1);
	}
	if ((weightFunction>0) && (numPieces == 0))
	{
		cout << "error: please provide the number of pieces with the argument -numpieces\nUse -help for more information\n" << endl;
		return -1;
	}


	cout << "Random Seed: " << seed << endl;
	srand(seed);

	double precisionFactor = 100.0;

	string folderName = datasetName + "_n" + to_string(n) + "_m" + to_string(m);


#ifdef __linux__
	const int dir_err = mkdir(folderName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (-1 == dir_err)
	{
		printf("Error creating directory!n");
		exit(1);
	}
#else
	_mkdir(folderName.c_str());
#endif


	//linear decreasing case
	if ( (weightFunction == POLYNOMIAL) && (funcDegree == 1) )
	{
		//random instances
		for (int k = 0; k < numInstances; k++)
		{
			string instanceName = to_string(k + 1);
			pugi::xml_document instanceFile;
			pugi::xml_node instanceNode = instanceFile.append_child("instance");
			pugi::xml_node infoNode = instanceNode.append_child("info");
			infoNode.append_child("dataset").append_child(pugi::node_pcdata).set_value(datasetName.c_str());
			infoNode.append_child("name").append_child(pugi::node_pcdata).set_value(instanceName.c_str());
			infoNode.append_child("n").append_child(pugi::node_pcdata).set_value(to_string(n).c_str());
			infoNode.append_child("m").append_child(pugi::node_pcdata).set_value(to_string(m).c_str());
			pugi::xml_node itemsNode = instanceNode.append_child("items");

			//pMax in [100, 1000]
			//double globalpMax = 100 + rand() % ((int)precisionFactor * 1000) / precisionFactor;
			double globalpMax = 100;
			
			//we can scale the rest
			int W = 100;
			
			//vector<int> w = hardSubsetF2(n, 2, 5, 10, W);
			//vector<int> w = hardSubsetEvenOdd(n, 1e2, W);

			for (int i = 0; i < n; i++)
			{
				double a, b, pi = 0, pMax;

				//avoid creating undesired items, such as items with no price of positive profit in the fixed costs case
				bool undesired = true;
				while (undesired)
				{
					undesired = false;
					
					//a factor from 0.01 to 20
					a = (.01 + rand() % (20 * (int)precisionFactor)) / precisionFactor;
					//a = (0.01 + 0.1 * (rand() % (5 * (int)precisionFactor)) / precisionFactor);

					//1 to W 										
					b = 1 + rand() % (5 * W);
					pMax = b / a;

					double wMax = 0;
					switch (priceLimit)
					{
					case INDIVIDUAL:
						pMax = pMax * ((51 + rand() % 49) / 100.0); //random percentage from 51% to 99%
						break;
					case GLOBAL:
						pMax = globalpMax;
						//weight of item at pMax will be between 20 to 96% of the capacity
						//this helps avoid filler items
						wMax = W * (20.0 + rand() % 77)*0.01; // 20.0 + rand() % 77 \in [20, 20+76 = 96]
						b = wMax + a * pMax;
						break;
					default:
						break;
					}	

					if (fixedCosts == 1)
					{
						//we want a fixed cost lower than the maximum profit, otherwise the item might not be profitable at any price point
						//int maxProfit = b * b / (4 * a);
						//pi = 1 + rand() % ((int)(maxProfit/2));
												
						pi = 1 + rand() % 100; //in [1,100]
					}
					else if (fixedCosts == 2)
					{
						pi = 1 + rand() % 100; //in [1,100]
						if (rand() % 2 == 0)
							pi = -pi;
					}

					//check if individual optimal for item has positive profit
					if ((b*b) / (2 * a*a) - pi <= 0)
						undesired = true;	

					//check if item fits the knapsack at some price
					if (b - a* pMax > W)
						undesired = true;
				}
								
				pugi::xml_node item = itemsNode.append_child("item");
				item.append_attribute("id") = i + 1;
				item.append_attribute("type") = "POLYNOMIAL";

				aux.str("");
				aux << fixed << setprecision(2) << pi;
				item.append_attribute("fixed_cost") = aux.str().c_str();

				aux.str("");
				aux << fixed << setprecision(2) << pMax;
				item.append_attribute("price_limit") = aux.str().c_str();
				
				pugi::xml_node coefficients = item.append_child("coefficients");

				pugi::xml_node coef = coefficients.append_child("coefficient");
				coef.append_attribute("degree") = 0;
				aux.str("");
				aux << fixed << setprecision(2) << b;
				coef.append_child(pugi::node_pcdata).set_value(aux.str().c_str());

				coef = coefficients.append_child("coefficient");
				coef.append_attribute("degree") = 1;
				aux.str("");
				aux << fixed << setprecision(2) << -a;
				coef.append_child(pugi::node_pcdata).set_value(aux.str().c_str());
			}
			
			infoNode.append_child("capacity").append_child(pugi::node_pcdata).set_value(to_string(W).c_str());

			instanceFile.save_file((folderName + "/" + instanceName + ".xml").c_str());
		}
	}

	//piecewise linear
	if (weightFunction == PIECEWISE_LINEAR)
	{
		//probability that any piece of the function will be continuous
		double constantPieceProbability = 20;
		for (int k = 0; k < numInstances; k++)
		{
			string instanceName = to_string(k + 1);
			pugi::xml_document instanceFile;
			pugi::xml_node instanceNode = instanceFile.append_child("instance");
			pugi::xml_node infoNode = instanceNode.append_child("info");
			infoNode.append_child("dataset").append_child(pugi::node_pcdata).set_value(datasetName.c_str());
			infoNode.append_child("name").append_child(pugi::node_pcdata).set_value(instanceName.c_str());
			infoNode.append_child("type").append_child(pugi::node_pcdata).set_value("PIECEWISE_LINEAR");
			infoNode.append_child("degree").append_child(pugi::node_pcdata).set_value("1");
			infoNode.append_child("n").append_child(pugi::node_pcdata).set_value(to_string(n).c_str());
			infoNode.append_child("m").append_child(pugi::node_pcdata).set_value(to_string(m).c_str());
			
			pugi::xml_node itemsNode = instanceNode.append_child("items");

			double precisionFactor = 100.0;
			//pMax in [0.001, 10]
			//double globalpMax = 1 / precisionFactor + rand() % ((int)precisionFactor * 10) / precisionFactor;

			double globalpMax = 100;
			int W = 100;

			for (int i = 0; i < n; i++)
			{				
				int pi = 0;
				if (fixedCosts == 1)
				{
					pi = 1 + rand() % 100; //in [1,100]
				}
				else if (fixedCosts == 2)
				{
					pi = 1 + rand() % 100; //in [1,100]
					if (rand() % 2 == 0)
						pi = -pi;
				}

				//price for the LIMIT_NONE and PRICE_LIMIT_INDIVIDUAL case
				double pMax = globalpMax * ((51.0 + rand() % 49) / 100.0); //random percentage from 51% to 99%

				if (priceLimit == GLOBAL)
					pMax = globalpMax;

			
				//random weight at p=0 in [1, 2*W]
				double currentWeight = 1 + rand() % (2*W);
				double startingPrice = 0;

				double priceStep = (pMax - startingPrice) / numPieces;

				pugi::xml_node item = itemsNode.append_child("item");
				item.append_attribute("id") = i + 1;
				item.append_attribute("type") = "PIECEWISE_LINEAR";	

				aux.str("");
				aux << fixed << setprecision(2) << pi;
				item.append_attribute("fixed_cost") = aux.str().c_str();

				aux.str("");
				aux << fixed << setprecision(2) << pMax;
				item.append_attribute("price_limit") = aux.str().c_str();

				pugi::xml_node points = item.append_child("points");
				pugi::xml_node point = points.append_child("point");

				point.append_child("p").append_child(pugi::node_pcdata).set_value("0");
				aux.str("");
				aux << fixed << setprecision(2) << currentWeight;
				point.append_child("w").append_child(pugi::node_pcdata).set_value(aux.str().c_str());
				
				int constantPieceCount = 0;
				for (int j = 0; j < numPieces - 1; j++)
				{
					if ((1 + rand() % 100 > constantPieceProbability) || (constantPieceCount > 0) )
					{
						//updates current weight
						currentWeight = ((60 + rand() % 40)/100.0) * currentWeight;
						constantPieceCount = 0;
					}
					else
					{
						constantPieceCount++;
					}

					point = points.append_child("point");

					aux.str("");
					aux << fixed << setprecision(2) << startingPrice + priceStep * (j + 1);
					point.append_child("p").append_child(pugi::node_pcdata).set_value(aux.str().c_str());
					aux.str("");
					aux << fixed << setprecision(2) << currentWeight;
					point.append_child("w").append_child(pugi::node_pcdata).set_value(aux.str().c_str());					
				}
				aux.str("");
				aux << fixed << setprecision(2) << pMax;
				//this case allow all items to have a price such that w = 0
				if (priceLimit == NONE)
				{
					point = points.append_child("point");
					point.append_child("p").append_child(pugi::node_pcdata).set_value(aux.str().c_str());
					point.append_child("w").append_child(pugi::node_pcdata).set_value("0");
				}
				else
				{
					currentWeight = (rand() % (int)(currentWeight * 1000) + 1.0) / 1000.0;
					point = points.append_child("point");

					point.append_child("p").append_child(pugi::node_pcdata).set_value(aux.str().c_str());
					aux.str("");
					aux << fixed << setprecision(2) << currentWeight;
					point.append_child("w").append_child(pugi::node_pcdata).set_value(aux.str().c_str());
				}
			}
			
			infoNode.append_attribute("capacity") = W;
			infoNode.append_child("capacity").append_child(pugi::node_pcdata).set_value(to_string(W).c_str());

			instanceFile.save_file((folderName + "/" + instanceName + ".xml").c_str());
		}
	}

	cout << "Instance" << (numInstances > 1 ? "s " : " ") << "succesfully generated." << endl;
}
