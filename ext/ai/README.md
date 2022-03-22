# Agents

Agents act within an environment with a set of actions using a model.  Agents are based on the DynaQ+ architecture.
Agents should have all the information they would need in each state in order to make a decision within the environment.
For situations where there is an infinite number of states within a range, such as a pixel array for images, FreeAgents should be used.
If FreeAgents are used, then sections 1.2 and 1.3 do not apply.

To define an Agent, do:

	pen::ai::Agent* agent = new pen::ai::Agent();
	agent->Init(AIState** userStates, long userStateNum, AIState* userInitialState, float userEpsilon = 0.0f, float userStepSize = 0.8f);

You can also create agents without intializing by loading them from .arlpen files by doing:
	
	pen::ai::Agent* agent = new pen::ai::Agent();
	agent->Load(const std::string& path, Action** userActions, long userNumActions);

For loading you have to have the Action** array already defined with actions passed in to do this.  You don not need to call Init when loading in anything.

You can save agents current learning by doing:

	agent->Save(const std::string& path);

----------------------------------------------------------------------------------------------------------------------------------------------------

# 1.1 - Creating Actions

All action functions passed in should be void functions.  In order to reuse actions, you can use the currentState from the agent that you are using to determine which state to go to next.
For instance in a grid, if one of your actions is going Right() and you are currently in cell (1,1), then you can query the agent->currentState to see that state id in a struct that you
define with the state id or pointer address and the grid cell location, then you can move the agent to (1,2).  How the state pointers and ids get mapped is application specific.

To create an Action, do:

	pen::ai::Action** actions = new pen::ai::Action*[numAllPossibleActionsForAgent];
	actions[0] = new pen::ai::Action(&YourAction, "YourAction");

The action name that gets passed in is case-sensitive.  Every action that is possible for an agent should be in the actions array.

----------------------------------------------------------------------------------------------------------------------------------------------------

# 1.2 - Creating AIStates

The ids for AIStates should be defined in a look up structure before creating these states since each application
is different.

When creating AIStates, StateActions need to be passed into the AIState from the given Actions:

	pen::ai::StateAction** p = new pen::ai::StateAction*[numActionsForCurrentState];
	long tempId = 0; //The tempId has its value incremented each time a new StateAction is created so it has a unique id.
	for (int i = 0; i < numActionsForCurrentState; i++) {
		p[i] = new pen::ai::StateAction(&tempId, pen::ai::Action* userActions[i], long userNextStateId,float nextStateReward, const std::string& userActions[i]->actionName, int numActions);
	}

The numActions should be the number of actions for the current state in which you are creating the StateAction list for.
The nextStateId and nextStateReward for each action are for the state in which the action causes the agent to transition to given the state you are making the StateAction list for.
The nextStateId and nextStateReward should be defined already for the environment, so that they can be passed in during initialization.

Once this array is created, you can now create a AIState:

	pen::ai::AIState* state = new pen::ai::AIState(char alreadyDefinedStateId, int numStates, int numActions, float userReward, pen::ai::StateAction** userActions, bool terminalStatus = false, float* userState = nullptr, int numStateParams = 0);

The userState and numStateParams fields are optional if you want to add data for each state for easier access later.

You can grab pointers to states by doing:

	pen::ai::AIState* state = FindState(pen::ai::Agent* agent, char stateId);

You can grab pointers to actions from a state by doing:

	pen::ai::Action* action = state->FindPolicy(long id);

----------------------------------------------------------------------------------------------------------------------------------------------------

# 1.3 - Running Agents

To run an agent, do:

	bool terminal = false;
    for (int i = 0; i < numSteps; i++) {
        if (i == numSteps - 1) terminal = true;
        agent->Step(terminal);
    }

----------------------------------------------------------------------------------------------------------------------------------------------------

# 1.4 - Running Free Agents

FreeAgents are Agents that perform based on a continuous state environment.

To use a FreeAgent first initialize its weights:

	Weight* weights = new Weight[numLayers];
	for(int i = 0; i < numLayers; i++){
		/*The number of layers is the hidden layers and the output layer*/
		if (i == 0) {
			weights[i] = new Weight(stateFeaturesNum, layers[i]->numNodes);
		}else{
			weights[i] = new Weight(layers[i - 1]->numNodes, layers[i]->numNodes);
		}
	}

There should be a Weight for each layer.

To run a FreeAgent first make one by doing:

	pen::ai::Agent* agent = new pen::ai::FreeAgent();
	agent->Init(pen::ai::Action** userActions, pen::ai::Weight* userWeights, pen::Mat* userInitialState, int userNumLayers, long userNumActions, int userNumEpisodes, float userEpsilon = 0.0001f, float userStepSize = 0.1f);

The userInitialState is a column vector for the state features.
Each state features column vector should be the same length and have the initial element state whether this is a terminal state or not.
The state features should be a pen::Mat column vector that contains everything within the environment needed to make decisions.
This vector should always be the same height.

	pen::Mat stateFeatures = pen::Mat(0.0f, 1, numStateParams);
	stateFeatures.matrix[0][0] = isTerminal ? 1.0f : 0.0f;

Then to run it do:

	bool terminal = false;
	while(!terminal){
		agent->Step(pen::Mat stateFeatures, float reward);
		terminal = SpecificImplementation();
	}

The reward is based on the specific set of state features passed in, most of these can be 0.0f with the terminal states being a huge positive number.
Most of these states can also be negative numbers with the terminal states being a huge positive number to encourage the agent to get to the terminal states faster.
It is up to the specific implementation as to when episodes should end.

Saving FreeAgents work the same way as regular Agents, but loading is different.
Once you load in a FreeAgent you have to pass in the initial state column vector of the environment since it could have changed since last time:

	agent->Load(const std::string& path, Action** userActions, long userNumActions);
	agent->Init(pen::Mat* stateFeatures);

----------------------------------------------------------------------------------------------------------------------------------------------------