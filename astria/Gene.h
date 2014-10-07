#ifndef _GENE_H_
#define _GENE_H_

#include <vector>
#include <algorithm>
#include "Params.h"
#include "utils.h"

using namespace std;

//struct SGenome
//{
//	vector<double> weights;
//	SGenome(vector<double> w) :weights(w){}
//};

enum GENE_INDEX
{
	GENE_INDEX_MOVEMENT = 0,
	GENE_INDEX_SURROUNDING,
	GENE_INDEX_ACTION,
	GENE_COUNT
};

typedef vector<double> SGenome;

class CIndividual;

class CGenAlg
{
public:
	static void GetOffSpring(CIndividual* dad, CIndividual* mom);
private:
	static void CrossOver(const vector<double> & dad, const vector<double> & mom, vector<double> & offspring);
	static void Mutate(vector <double> & chromosome);
};


//class CGenAlg
//{
//private:
//	vector<SGenome> population;
//	int PopSize;
//	int ChromoLength;
//	double TotalFitness;
//	double BestFitness;
//	double AverageFitness;
//	double WorstFitness;
//	double FittestGenome;
//	double MutationRate;
//	double CrossoverRate;
//	int generation;
//	void Crossover(const vector<double> &mom,
//		const vector<double> &dad,
//		vector<double> &offspring1,
//		vector<double> &offspring2);
//	void Mutate(vector<double> &chromo);
//	SGenome GetChromoRoulette();
//	void GrabBest(int best, const int numCopies, vector<SGenome> &pop);
//	void calBestWorstAvTot();
//	void reset();
//
//public:
//	CGenAlg(int popsize, double mutrate, double crossrate, int numweights);
//
//	vector<SGenome> Epoch(vector<SGenome> &old_pop);
//
//	vector<SGenome> GetChromo() const {
//		return population;
//	}
//
//	double avgFitness() const { return TotalFitness / PopSize; }
//
//	double bestFitness() const { return BestFitness; }
//};

#endif