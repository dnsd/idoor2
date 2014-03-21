#include <vector>
#include <deque>
#include "decision.h"

using namespace std;

int open::judge_open_mode(int vote1)
{
    // 将来的には、voteの数に動的に対応できるようにする

    return vote1;
}

int open::judge_open_mode(int vote1, int vote2)
{
    // 将来的には、voteの数に動的に対応できるようにする
    if (vote1 >= vote2)
    {
        return vote1;
    }else{
        return vote2;
    }
}

int open::judge_open_mode(int vote1, int vote2, int vote3)
{
	// 将来には、voteの数に動的に対応できるようにする
	vector<int> votes;
	votes.push_back(vote1);
	votes.push_back(vote2);
	votes.push_back(vote3);

	sort(votes.begin(), votes.end());
	return votes[0];
}

int open::judge_open_mode(int vote1, int vote2, int vote3, int vote4)
{
	// 将来には、voteの数に動的に対応できるようにする
	vector<int> votes;
	votes.push_back(vote1);
	votes.push_back(vote2);
	votes.push_back(vote3);
	votes.push_back(vote4);

	sort(votes.begin(), votes.end());
	return votes[0];
}