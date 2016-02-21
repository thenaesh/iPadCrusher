#include <cstdio>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;


enum OBJECTIVESTATE { INPROGRESS=0, LOSE=1, WIN=2 };
#define CRITICAL_STRENGTH_UNDEFINED -1

class Game
{
private:
	int N, L; // #iPads, #levels
	int** wc; // worst case number of crushes required per n, l
	int range[2]; // critical range in which the breaking strength is stored
	int& lb, ub; // references to the range bounds, to avoid array notation
	int cost; // current cost incurred, with assumption that cost per test is 1
	int rn; // iPads left

public:
	Game(int N_, int L_);
	~Game();

	bool smash(int l); // smash an ipad at strength l	
	OBJECTIVESTATE checkProgress();
	int getCriticalStrength();
	int getiPadsLeft();

	void printTable();

private:
	void calcWorstCases();
};


int main()
{
	int num_ipads, num_levels;

	printf("#iPads #levels ? ");
	scanf("%d %d", &num_ipads, &num_levels);
	printf("\n");

	Game game(num_ipads, num_levels);

	OBJECTIVESTATE state = INPROGRESS;
	while ((state = game.checkProgress()) == INPROGRESS)
	{
		int strength;
		printf("Enter Strength: "); scanf("%d", &strength); printf("\n");	
		bool smashresult = game.smash(strength);
		printf("iPad %s, %d iPads left\n", smashresult ? "smashed" : "not smashed", game.getiPadsLeft());
	}

	if (state == WIN) printf("YOU WIN! The critical strength was %d\n", game.getCriticalStrength());
	else printf("LOSER :P\n");
}


Game::Game(int N_, int L_): N(N_), L(L_), cost(0), rn(N_), lb(range[0]), ub(range[1])
{
	// construction of the worst case array
	this->wc = new int*[N+1];
	this->wc[0] = reinterpret_cast<int*>(0); // DANGER: EXPLOSIVES
	for (int i=1; i<=N; i++) this->wc[i] = new int[L+1];
	
	// initialisation of the critical range
	lb = 1; ub = L;

	this->calcWorstCases();

	//this->printTable();
}

Game::~Game()
{
	// destruction of the worst case array
	for (int i=1; i<=N; i++) delete this->wc[i];
	delete this->wc;
}

bool Game::smash(int l)
{
	if (rn > 1 && lb <= l && l <= ub)
	{
		// need to modify the critical range
		int wc_break = wc[rn-1][l-1];
		int wc_nobreak = wc[rn][L-l];

		bool break_decision;

		if (wc_break > wc_nobreak)
		{
			ub = l;
			break_decision = true;
			rn--;
		}
		else
		{
			lb = l + 1;
			break_decision = false;
		}

		return break_decision;
	}
	else if (rn == 1 && lb <= l && l && ub)
	{
		if (lb != ub)
		{
			lb = lb + 1;
			rn--;
			return false;
		}
		else
		{
			rn--;
			return true;
		}
	}
	else return !(l < lb); 
}

OBJECTIVESTATE Game::checkProgress()
{
	if (rn == 0)
	{
		if (lb == ub) return WIN;
		else return LOSE;
	}
	else
	{
		if (lb == ub) return WIN;
		else return INPROGRESS;
	}
}

int Game::getCriticalStrength()
{
	// make sure the range has converged to a bound
	if (lb != ub) return CRITICAL_STRENGTH_UNDEFINED;

	return lb;
}

int Game::getiPadsLeft()
{
	return rn;
}

void Game::printTable()
{
	printf("Lower Bound: %d\nUpper Bound: %d\n", lb, ub);
	for (int n=1; n<=N; n++)
	{
		for (int l=1; l<=L; l++)
			printf("(%d, %d): %d\n", n, l, wc[n][l]);
		printf("\n");
	}
}

void Game::calcWorstCases()
{
	// if this is our last iPad, there is no choice but to linear search upwards
	for (int l=1; l<=L; l++) wc[1][l] = l;

	// if there are no floors, there's nothing to test
	for (int n=1; n<=N; n++) wc[n][0] = 0;

	// compute the DP table bottom up
	for (int n=2; n<=N; n++)
		for (int l=2; l<=L; l++)
		{
			wc[n][l] = INT_MAX;
			for (int x=1; x<=l; x++)
			{
				int tmp = 1 + max(wc[n-1][x-1], wc[n][l-x]);
				if (tmp < wc[n][l]) wc[n][l] = tmp;
			}
		}
}
