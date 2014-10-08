#include "Gene.h"

//CGenAlg::CGenAlg(int popsize, double mutrate, double crossrate, int numweights) :
//PopSize(popsize),
//MutationRate(mutrate),
//CrossoverRate(crossrate),
//ChromoLength(numweights),
//TotalFitness(0),
//generation(0),
//FittestGenome(0),
//BestFitness(0),
//WorstFitness(99999999),
//AverageFitness(0)
//{
//	for (int i = 0; i < PopSize; i++)
//	{
//		population.push_back(SGenome());
//		for (int j = 0; j < ChromoLength; j++)
//		{
//			population[i].weights.push_back(RandNormal());
//		}
//	}
//}
//
//void CGenAlg::Mutate(vector<double> &chromo)
//{
//	for (int i = 0; i < chromo.size(); i++)
//	{
//		if (RandFloat() < MutationRate)
//		{
//			chromo[i] += (RandNormal() * CParams::MaxPerturbation);
//		}
//	}
//}
//
//SGenome CGenAlg::GetChromoRoulette()
//{
//	double Slice = (double)(RandFloat() * TotalFitness);
//
//	SGenome chosen;
//
//	double accumulatedFitness = 0;
//
//	for (int i = 0; i < PopSize; i++)
//	{
//		accumulatedFitness += population[i].fitness;
//		if (accumulatedFitness > Slice)
//		{
//			chosen = population[i];
//			break;
//		}
//	}
//	return chosen;
//}
//
//void CGenAlg::Crossover(const vector<double> &mom,
//	const vector<double> &dad,
//	vector<double> &offspring1,
//	vector<double> &offspring2)
//{
//	if ((RandFloat() > CrossoverRate) || (mom == dad))
//	{
//		offspring1 = mom;
//		offspring2 = dad;
//		return;
//	}
//
//	int cp = RandInt(0, ChromoLength - 1);
//
//	for (int i = 0; i < cp; i++)
//	{
//		offspring1.push_back(mom[i]);
//		offspring2.push_back(dad[i]);
//	}
//	for (int i = cp; i < mom.size(); i++)
//	{
//		offspring1.push_back(mom[i]);
//		offspring2.push_back(dad[i]);
//	}
//	return;
//}
//
//vector<SGenome> CGenAlg::Epoch(vector<SGenome> &old_pop)
//{
//	//assign the given population to the classes population
//	population = old_pop;
//
//	//reset the appropriate variables
//	reset();
//
//	//sort the population (for scaling and elitism)
//	sort(population.begin(), population.end());
//
//	//calculate best, worst, average and total fitness
//	calBestWorstAvTot();
//
//	//create a temporary vector to store new chromosones
//	vector <SGenome> vecNewPop;
//
//	//Now to add a little elitism we shall add in some copies of the
//	//fittest genomes. Make sure we add an EVEN number or the roulette
//	//wheel sampling will crash
//	if (!(CParams::NumCopiesElite * CParams::NumElite % 2))
//	{
//		GrabBest(CParams::NumElite, CParams::NumCopiesElite, vecNewPop);
//	}
//
//
//	//now we enter the GA loop
//
//	//repeat until a new population is generated
//	while (vecNewPop.size() < PopSize)
//	{
//		//grab two chromosones
//		SGenome mom = GetChromoRoulette();
//		SGenome dad = GetChromoRoulette();
//
//		//create some offspring via crossover
//		vector<double>		baby1, baby2;
//
//		Crossover(mom.weights, dad.weights, baby1, baby2);
//
//		//now we mutate
//		Mutate(baby1);
//		Mutate(baby2);
//
//		//now copy into vecNewPop population
//		vecNewPop.push_back(SGenome(baby1, 0));
//		vecNewPop.push_back(SGenome(baby2, 0));
//	}
//
//	//finished so assign new pop back into m_vecPop
//	population = vecNewPop;
//
//	return population;
//}
//
//void CGenAlg::GrabBest(int best, const int numCopies, vector<SGenome> &pop)
//{
//	while (best--)
//	{
//		for (int i = 0; i < numCopies; i++)
//		{
//			pop.push_back(population[(PopSize - 1) - best]);
//		}
//	}
//}
//
//void CGenAlg::calBestWorstAvTot()
//{
//	TotalFitness = 0;
//
//	double HighestSoFar = 0;
//	double LowestSoFar = 9999999;
//
//	for (int i = 0; i<PopSize; ++i)
//	{
//		//update fittest if necessary
//		if (population[i].fitness > HighestSoFar)
//		{
//			HighestSoFar = population[i].fitness;
//
//			FittestGenome = i;
//
//			BestFitness = HighestSoFar;
//		}
//
//		//update worst if necessary
//		if (population[i].fitness < LowestSoFar)
//		{
//			LowestSoFar = population[i].fitness;
//
//			WorstFitness = LowestSoFar;
//		}
//
//		TotalFitness += population[i].fitness;
//
//
//	}//next chromo
//
//	AverageFitness = TotalFitness / PopSize;
//}
//
////-------------------------Reset()------------------------------
////
////	resets all the relevant variables ready for a new generation
////--------------------------------------------------------------
//void CGenAlg::reset()
//{
//	TotalFitness = 0;
//	BestFitness = 0;
//	WorstFitness = 9999999;
//	AverageFitness = 0;
//}

//void CGenAlg::GetOffSpring(CIndividual* dad, CIndividual* mom)
//{
//	if (dad == mom || !dad->CanMate() || !mom->CanMate())
//	{
//		return;
//	}
//
//	vector<SGenome> NewDNA(dad->GetDNA().size());
//
//	for (int i = 0; i < NewDNA.size(); i++)
//	{
//		if (RandFloat() <= CParams::CrossoverRate)
//		{
//			CrossOver(dad->GetDNA()[i], mom->GetDNA()[i], NewDNA[i]);
//		}
//
//		if (RandFloat() <= CParams::MutationRate)
//		{
//			Mutate(NewDNA[i]);
//		}
//	}
//
//	SVector2D NewPosition((dad->position.x - mom->position.x) / 2, (dad->position.y - mom->position.y) / 2);
//
//	CIndividual::Population.push_back(new CIndividual(NewPosition.x, NewPosition.y, NewDNA));
//}
//
//void CGenAlg::CrossOver(const vector<double> & dad, const vector<double> & mom, vector<double> & offspring)
//{
//	int cp = RandInt(0, dad.size());
//
//	for (int i = 0; i < cp; i++)
//	{
//		offspring.push_back(dad[i]);
//	}
//	for (int i = cp; i < mom.size(); i++)
//	{
//		offspring.push_back(mom[i]);
//	}
//}
//
//void CGenAlg::Mutate(vector<double> & chromosome)
//{
//	for (int i = 0; i < chromosome.size(); i++)
//	{
//		if (RandFloat() < CParams::MutationRate)
//		{
//			chromosome[i] += (RandNormal() * CParams::MaxPerturbation);
//		}
//	}
//}