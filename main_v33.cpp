#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>

#define FIELD_X 16000
#define FIELD_Y 7500

#define GOAL_CENTER_RIGHTX 16000
#define GOAL_CENTER_Y 3750
#define GOAL_MIN_Y 1750
#define GOAL_MAX_Y 5750
#define GOAL_CENTER_LEFTX 0
#define GOAL_RADIUS 300
#define GOAL_LENGHT 4000

#define TOLERENCE 1000

#define SNAFFLE_RADIUS 150
#define BLUDGER_THRUST 1000
#define BLUDGER_RADIUS 200
#define WIZARD_RADIUS 400

#define FIELD_TIER_LEFT 5333
#define FIELD_TIER_RIGHT 10666

#define DANGER_ZONE_LEFT FIELD_X / 10
#define DANGER_ZONE_RIGHT FIELD_X / 10 * 9
#define TENTH_FIELD FIELD_X / 10
#define NINE_TENTH_FIELD FIELD_X / 10 * 9

#define POLE_RADIUS 300

// STRATEGY
#define MANA_STRATEGY 20
#define MANA_STRATEGY_DEF 25

#define MANNED_STRATEGY 1



using namespace std;

/**
 * Grab Snaffles and try to throw them through the opponent's goal!
 * Move towards a Snaffle to grab it and use your team id to determine towards where you need to throw it.
 * Use the Wingardium spell to move things around at your leisure, the more magic you put it, the further they'll move.
 **/
 
class Entity
{
    public:
    int _entityId; // entity identifier
    string _entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" or "BLUDGER"
    int _x; // position
    int _y; // position
    int _vx; // velocity
    int _vy; // velocity
    int _state;
    float _mass;
    float _friction;
};

class Wizard: public Entity
{
    public:
    int _role;
    int _cooldown;
    
    Wizard(){
        _entityId = -1;
        _entityType = "WIZARD";
        _mass = 1;
        _friction = 0.75;
        _role = 0;
        _cooldown = 0;}
    
    void wingardium();
};

class OppWizard: public Entity
{
    public:
    OppWizard(){
        _entityId = -1;
        _entityType = "OPPONENT_WIZARD";
        _mass = 1;
        _friction = 0.75;}
};

class Snaffle: public Entity
{
    public:
    int _manned;
    
    Snaffle(){
        _entityId = -1;
        _entityType = "SNAFFLE";
        _mass = 0.5;
        _manned = 0;
        _friction = 0.75;} // a changer
};

class Bludger: public Entity
{
    public:
    Bludger(){
        _entityId = -1;
        _entityType = "BLUDGER";
        _mass = 8;
        _friction = 0.9;} // a changer
    
};

class Action
{
    public:
    Action(int id)
    {
        _idPlayer = id;
        if (id == 0) // -> Right
        {
            _goalCenterx = GOAL_CENTER_RIGHTX;
        }
        else
            _goalCenterx = GOAL_CENTER_LEFTX;
        _goalCentery = GOAL_CENTER_Y;
        _goalMiny = _goalCentery - (GOAL_LENGHT - GOAL_RADIUS);
        _goalMaxy = _goalCentery + (GOAL_LENGHT - GOAL_RADIUS);
    }
    
    string _command;
    int _x;
    int _y;
    int _param;
    int _id;
    
    int _idPlayer;
    int _goalCenterx;
    int _goalCentery;
    int _goalMiny;
    int _goalMaxy;
    
    int _mana;
    int _turn;
};

void turnZero(vector<Entity*>& entities, vector<Snaffle>& snaffles,  int entityId, string entityType, int x, int y, int vx, int vy, int state);
void print_stuff(vector<Entity*>& entities, vector<Snaffle>& snaffles);
void normalTurn(vector<Entity*>& entities, vector<Snaffle>& snaffles, int entityId, string entityType, int x, int y, int vx, int vy, int state);

void takeAction(Wizard& wizard, vector<Entity*>& entities, vector<Snaffle>& snaffles, Action& action, Bludger* bludgers, OppWizard* opp);

double getDist(int ax, int ay, int bx, int by);
void setSpeed(Wizard& wizard, Action& a, double dist);
void goToClosest(Wizard& wizard, vector<Snaffle>& snaffles, Action& action);
void throwBall(Wizard& wizard, Action& action);

void wingardiumABludger(Wizard& wizard, Bludger& bludger, Action& action, OppWizard *opp);
int goingToGetHit(Wizard& wizard, Bludger* bludgers);

bool isInTier(Entity& snaffle, Action& action);
int aBallIsGoingIn(vector<Snaffle>& snaffles, Action& action);
void goToClosestDef(Wizard& wizard, vector<Snaffle>& snaffles, Action& action);
void defensiveWingardium(Wizard& wizard, Snaffle& snaffle, Action& action);

void offensiveWingardium(Wizard& wizard, vector<Snaffle> snaffles, Action& action);

int ballIsInDangerZone(vector<Snaffle>& snaffles, Action& action);
void saveTheBall(Wizard& wizard, Snaffle& snaffle, Action& action);

bool entityIsInDangerZone(Entity& entity, Action& action);
bool entityIsInMoneyTime(Entity& entity, Action& action);

void shootStraight(Wizard& wizard, Snaffle& snaffle, Action& action);

int main()
{
    srand(time(nullptr));
    int myTeamId; // if 0 you need to score on the right of the map, if 1 you need to score on the left
    int turn = 0;
    
    cin >> myTeamId; cin.ignore();
    
    Wizard wizards[2];
    OppWizard opp[2];
    Bludger bludgers[2];
    vector<Snaffle> snaffles;
    
    Action action(myTeamId);
    wizards[0]._role = 0;
    wizards[1]._role = 1;
    
    
    vector<Entity *> entities;
    entities.push_back(&wizards[0]);
    entities.push_back(&wizards[1]);
    entities.push_back(&opp[0]);
    entities.push_back(&opp[1]);
    entities.push_back(&bludgers[0]);
    entities.push_back(&bludgers[1]);
    
  //  cout << "merde\n";
    //gros_bug;

    // game loop
    while (1) {
        int myScore;
        int myMagic;
    
        cin >> myScore >> myMagic; cin.ignore();
        int opponentScore;
        int opponentMagic;
        cin >> opponentScore >> opponentMagic; cin.ignore();
        int nentities; // number of entities still in game
        cin >> nentities; cin.ignore();
        
        action._mana = myMagic;
        
        for (int i = 0; i < nentities; i++) {
            int entityId; // entity identifier
            string entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" or "BLUDGER"
            int x; // position
            int y; // position
            int vx; // velocity
            int vy; // velocity
            int state; // 1 if the wizard is holding a Snaffle, 0 otherwise. 1 if the Snaffle is being held, 0 otherwise. id of the last victim of the bludger.
            
            cin >> entityId >> entityType >> x >> y >> vx >> vy >> state; cin.ignore();
            
            if (turn == 0)
                turnZero(entities, snaffles, entityId, entityType, x, y, vx, vy, state);
            else
                normalTurn(entities, snaffles, entityId, entityType, x, y, vx, vy, state);
            
        }
    //    print_stuff(entities, snaffles);
        turn += 1;

        for (int i = 0; i < 2; i++) {

            // Write an action using cout. DON'T FORGET THE "<< endl"
            // To debug: cerr << "Debug messages..." << endl;
            takeAction(wizards[i], entities, snaffles, action, bludgers, opp);

            // Edit this line to indicate the action for each wizard (0 ≤ thrust ≤ 150, 0 ≤ power ≤ 500, 0 ≤ magic ≤ 1500)
            // i.e.: "MOVE x y thrust" or "THROW x y power" or "WINGARDIUM id x y magic"
            if (action._command != "WINGARDIUM")
                cout << action._command << " " << action._x << " " << action._y << " " 
                << action._param << endl;
            else
                cout << action._command << " " << action._id << " " 
                << action._x << " " << action._y << " " << action._param << endl;
        }
        
        //reset
        snaffles.clear();
    }
}

void takeAction(Wizard& wizard, vector<Entity*>& entities, vector<Snaffle>& snaffles, Action& action, Bludger* bludgers, OppWizard* opp)
{
    int id;
    wizard._cooldown -= 1;
    
  /*  if (action._mana > 0 && (id = goingToGetHit(wizard, bludgers) > -1))
    {
        action._id = bludgers[id]._entityId;
        wingardiumABludger(wizard, bludgers[id], action, opp);
    } // doesn't work that good
 /*   if (wizard._state == 0 && wizard._role == 0)
    {
        if ((id = aBallIsGoingIn(snaffles, action) > -1 && action._mana > 0)) // check if cannot grab ball for 3 turns
        {
            defensiveWingardium(wizard, snaffles[id], action);
            action._id = id;
        }
        else
            goToClosestDef(wizard, snaffles, action);
    }       */
    if ((id = ballIsInDangerZone(snaffles, action)) != -1 && action._mana >= MANA_STRATEGY_DEF && wizard._role == 0)
        saveTheBall(wizard, snaffles[id], action); 
    else if (wizard._state == 0 && wizard._cooldown > 0)
    {
        for (int i = 0; i < snaffles.size(); i++)
        {
            if ((action._mana > 0 && entityIsInMoneyTime(snaffles[i], action) &&
            snaffles[i]._y >= GOAL_MIN_Y + 400 && snaffles[i]._y <= GOAL_MAX_Y - 400))
            {
                shootStraight(wizard, snaffles[i], action);
                return ;
            }
        }
        if (action._mana > MANA_STRATEGY)
            offensiveWingardium(wizard, snaffles, action);
        else //if (!isInTier(wizard, action))
            goToClosest(wizard, snaffles, action);
        //else
         //   pokeTheBallRight(wizard, snaffles, action);
    }
    else if (wizard._state == 0 && wizard._cooldown <= 0)
    {
        goToClosest(wizard, snaffles, action);
    }
    else if (wizard._state == 1)
    {
        throwBall(wizard, action);
        wizard._cooldown = 3;
    }
}

/*
void pokeTheBallRight(Wizard& wizard, vector<Snaffle>& snaffles, Action& action)
{
  */  

// defense

int ballIsInDangerZone(vector<Snaffle>& snaffles, Action& action)
{
    for (int i = 0; i < snaffles.size(); i++)
    {
        if ((snaffles[i]._x <= DANGER_ZONE_LEFT && action._idPlayer == 0) ||
        (snaffles[i]._x >= DANGER_ZONE_RIGHT && action._idPlayer == 1))
            if (snaffles[i]._y <= GOAL_MAX_Y && snaffles[i]._y >= GOAL_MIN_Y)
                return (i);
    }
    return (-1);
}

void saveTheBall(Wizard& wizard, Snaffle& snaffle, Action& action)
{
    action._command = "WINGARDIUM";
    action._id = snaffle._entityId;
    if (snaffle._y <= GOAL_CENTER_Y)
        action._y = 0;
    else
        action._y = FIELD_Y;
    if (action._idPlayer == 0)
        action._x = TENTH_FIELD;
    else
        action._x = NINE_TENTH_FIELD;
    action._param = action._mana;
}

void shootStraight(Wizard& wizard, Snaffle& snaffle, Action& action)
{
    action._command = "WINGARDIUM";
    action._id = snaffle._entityId;
    action._x = action._goalCenterx;
    action._y = snaffle._y;
    action._param = getDist(snaffle._x, snaffle._y, action._x, action._y) / 15;
    if (action._param > action._mana)
        action._param = action._mana;
    if (action._param <= 0)
        action._param = 1;
    snaffle._manned = 1;
}

void offensiveWingardium(Wizard& wizard, vector<Snaffle> snaffles, Action& action)
{
    double dist;
    double dist_closest = 0;
    int id_far = 0;
    
    for (int i = 0; i < snaffles.size(); i++)
    {
        //dist = getDist(snaffles[i]._x + snaffles[i]._vx, snaffles[i]._y + snaffles[i]._vy, wizard._x, wizard._y);
        if (action._idPlayer == 1)
          dist = getDist(snaffles[i]._x, snaffles[i]._y, GOAL_CENTER_LEFTX, GOAL_CENTER_Y);
         else
            dist = getDist(snaffles[i]._x, snaffles[i]._y, GOAL_CENTER_RIGHTX, GOAL_CENTER_Y);
/*        if (isInTier(snaffles[i], action) && 
        snaffles[i]._y <= GOAL_MAX_Y - POLE_RADIUS && 
        snaffles[i]._y >= GOAL_MIN_Y + POLE_RADIUS
        && (snaffles[i]._manned == 0 || snaffles.size() <= MANNED_STRATEGY))
        {
            id_far = snaffles[i]._entityId;
            dist_farest = dist;
            break ;
        }   */
        if (dist < dist_closest && (snaffles[i]._manned == 0 || snaffles.size() <= MANNED_STRATEGY)
        && snaffles[i]._state == 0)
        {
            dist_closest = dist;
            id_far = i;
        }
    }
    action._command = "WINGARDIUM";
    action._id = snaffles[id_far]._entityId;
    
    action._x = action._goalCenterx;
 //   int random = rand() % ((action._goalMaxy - 100) - (action._goalMiny + 100)) + action._goalMiny + 100;
    action._y = GOAL_CENTER_Y;
    action._param = action._mana;
}

bool entityIsInDangerZone(Entity& entity, Action& action)
{
    if (action._idPlayer == 0 && entity._x <= DANGER_ZONE_LEFT)
            return true;
    if (action._idPlayer == 1 && entity._x >= DANGER_ZONE_RIGHT)
            return true;
    return false;
}

bool entityIsInMoneyTime(Entity& entity, Action& action)
{
    if (action._idPlayer == 1 && entity._x <= DANGER_ZONE_LEFT)
            return true;
    if (action._idPlayer == 0 && entity._x >= DANGER_ZONE_RIGHT)
            return true;
    return false;
}

int aBallIsGoingIn(vector<Snaffle>& snaffles, Action& action)
{
    for (int i = 0; i < snaffles.size(); i++)
    {
        if (snaffles[i]._vx >= 0 && action._idPlayer == 0)
            continue ;
        if (snaffles[i]._vx < 0 && action._idPlayer == 1)
            continue ;
        if (action._idPlayer == 0) // i am left
        {
            if (snaffles[i]._x + snaffles[i]._vx <= GOAL_CENTER_LEFTX + TOLERENCE
            && snaffles[i]._y + snaffles[i]._vy >= GOAL_MIN_Y &&
            snaffles[i]._y <= GOAL_MAX_Y)
                return (i);
        }
        else if (action._idPlayer == 1)
        {
            if (snaffles[i]._x + snaffles[i]._vx >= GOAL_CENTER_LEFTX - TOLERENCE
            && snaffles[i]._y + snaffles[i]._vy >= GOAL_MIN_Y &&
            snaffles[i]._y <= GOAL_MAX_Y)
                return (i);
        }
    }
    return (-1);
}

void defensiveWingardium(Wizard& wizard, Snaffle& snaffle, Action& action)
{   
    int x, y;
    
    action._command = "WINGARDIUM";
    if (action._idPlayer == 0)
        action._x = rand() % (FIELD_X - snaffle._x) + snaffle._x;
    else
        action._x = rand() % (snaffle._x);
    action._y = rand() % (FIELD_Y);
    
    action._param = rand() % action._mana;
}

void goToClosestDef(Wizard& wizard, vector<Snaffle>& snaffles, Action& action)
{
    action._command = "MOVE";
    double dist;
    double dist_closest = INT_MAX;
    int id_close = -1;
    
    for (int i = 0; i < snaffles.size(); i++)
    {
        dist = getDist(snaffles[i]._x + snaffles[i]._vx, snaffles[i]._y + snaffles[i]._vy, wizard._x, wizard._y);
        if (dist < dist_closest && snaffles[i]._manned == 0 && snaffles[i]._state == 0
        && isInTier(snaffles[i], action))
        {
            dist_closest = dist;
            id_close = i;
        }
    }
    if (id_close > 1)
    {
        action._x = snaffles[id_close]._x + snaffles[id_close]._vx;
        action._y = snaffles[id_close]._y + snaffles[id_close]._vy;
        snaffles[id_close]._manned = 1;
    }
    else
    {
        if (action._idPlayer == 0)
        {
            dist_closest = getDist(wizard._x, wizard._y, GOAL_CENTER_LEFTX, GOAL_CENTER_Y);
            action._x = GOAL_CENTER_LEFTX;
        }
        else
        {
            dist_closest = getDist(wizard._x, wizard._y, GOAL_CENTER_RIGHTX, GOAL_CENTER_Y);
            action._x = GOAL_CENTER_LEFTX;
        }
        action._y = GOAL_CENTER_Y;
    }
    
    setSpeed(wizard, action, dist_closest);
}

bool isInTier(Entity& snaffle, Action& action)
{
    if (action._idPlayer == 0 && snaffle._x <= FIELD_TIER_LEFT)
            return true;
    if (action._idPlayer == 1 && snaffle._x >= FIELD_TIER_RIGHT)
            return true;
    return false;
}

////// ALGOS UTILS
void goToClosest(Wizard& wizard, vector<Snaffle>& snaffles, Action& action)
{
    action._command = "MOVE";
    double dist;
    double dist_closest = INT_MAX;
    int id_close = 0;
    
    for (int i = 0; i < snaffles.size(); i++)
    {
        dist = getDist(snaffles[i]._x + snaffles[i]._vx, snaffles[i]._y + snaffles[i]._vy, wizard._x, wizard._y);
        if (dist < dist_closest && snaffles[i]._state == 0
        && (snaffles[i]._manned == 0 || snaffles.size() <= MANNED_STRATEGY))
        {
            dist_closest = dist;
            id_close = i;
        }
    }
    action._x = snaffles[id_close]._x + snaffles[id_close]._vx;
    action._y = snaffles[id_close]._y + snaffles[id_close]._vy;
    snaffles[id_close]._manned = 1;
    
    setSpeed(wizard, action, dist_closest);
}

void setSpeed(Wizard& wizard, Action& a, double dist)
{
    // already got predicted pos
    int dirx, diry;
    double current_speed = getDist(wizard._vx, wizard._vy, wizard._x, wizard._y);
    
    
    (a._x >= wizard._x) ? (dirx = 1) : (dirx = -1);
    (a._y >= wizard._y) ? (diry = 1) : (diry = -1);
    
/*    if (current_speed > dist)
    {
        
    }
    else if (current_speed < dist)
    {*/
        //a._param = (dist - current_speed) * wizard._mass;
        a._param = dist * wizard._mass;
        cerr << a._param << endl;
        cerr << "dist " << dist << " current speed " << current_speed << endl;
        if (a._param > 150)
            a._param = 150;
        else if (a._param < 0)
            a._param = 0;
}


void throwBall(Wizard& wizard, Action& action)
{
    int x, y;
    double dist;
    double closest;
    
    action._command = "THROW";
    x = action._goalCenterx;
    // get dir
/*    if (wizard._y <= action._goalMaxy - SNAFFLE_RADIUS && wizard._y >= action._goalMiny + SNAFFLE_RADIUS)
        y = wizard._y;s
    else if (wizard._y < action._goalMiny + SNAFFLE_RADIUS)
        y = action._goalMiny + (SNAFFLE_RADIUS + 100);
    else
        y = action._goalMaxy - (SNAFFLE_RADIUS + 100);*/
    
    if (entityIsInDangerZone(wizard, action)) // defensive throw
    {
        if (action._idPlayer == 0)
            x = DANGER_ZONE_LEFT * 2;
        else
            x = DANGER_ZONE_RIGHT - DANGER_ZONE_LEFT;
        if (wizard._y <= GOAL_CENTER_Y)
            action._y = 0;
        else
            action._y = FIELD_Y;
    }
    else if (getDist(wizard._x, wizard._y, x, GOAL_CENTER_Y) <= FIELD_TIER_LEFT) // money time
    {
        cerr << wizard._entityId << " is on money time\n";
      //  action._y = rand() % ((GOAL_MAX_Y - POLE_RADIUS) + (GOAL_MIN_Y + POLE_RADIUS)) + GOAL_MIN_Y + POLE_RADIUS;
        action._y = GOAL_CENTER_Y;
    }
    else
    {
        if (wizard._y >= GOAL_CENTER_Y)
            action._y = rand() % (GOAL_CENTER_Y + GOAL_MIN_Y + POLE_RADIUS) + GOAL_MIN_Y + POLE_RADIUS;
        else
            action._y = rand() % (GOAL_MAX_Y - POLE_RADIUS + GOAL_CENTER_Y) + GOAL_CENTER_Y;
    }    
    action._x = x;
    action._param = 500;
}

double getDist(int ax, int ay, int bx, int by)
{
    return (sqrt(pow(ax - bx, 2) + pow(ay - by, 2)));
}

int goingToGetHit(Wizard& wizard, Bludger* bludgers)
{
    for (int i = 0; i < 2; i++)
    {
        if (getDist(bludgers[i]._x + bludgers[i]._vx, bludgers[i]._y + bludgers[i]._vy, wizard._x, wizard._y) <= WIZARD_RADIUS)
            return (i);
    }
    return (-1);
}

void wingardiumABludger(Wizard& wizard, Bludger& bludger, Action& action, OppWizard *opp)
{
    action._command = "WINGARDIUM";
    double closer = INT_MAX;
    int id_closer;
    double dist;
    
    for (int i = 0; i < 2; i++)
    {
        if (opp[i]._entityId != bludger._state)
        {
            dist = getDist(bludger._x, bludger._y, opp[i]._x + opp[i]._vx, opp[i]._y + opp[i]._vy);
            if (dist < closer)
            {
                closer = dist;
                id_closer = i;
                action._x = opp[i]._x + opp[i]._vx;
                action._y = opp[i]._y + opp[i]._vy;
            }
        }
        action._param = closer / 15;
        if (action._param > action._mana)
            action._param = action._mana;
        if (action._param <= 0)
            action._param = 1;
    }
}





///////////// PZRSING
void normalTurn(vector<Entity*>& entities, vector<Snaffle>& snaffles, int entityId, string entityType, int x, int y, int vx, int vy, int state)
{
 //   cerr << entityId << entityType << endl;
    if (entityType != "SNAFFLE")
    {
        for (int i = 0; i < entities.size(); i++)
        {
            if (entities[i]->_entityId == entityId)
            {
                entities[i]->_x = x;
                entities[i]->_y = y; 
                entities[i]->_vx = vx; 
                entities[i]->_vy = vy;
                entities[i]->_state = state;
                break ;
            }
        }
    }
    else
    {
        Snaffle snaffle;
        snaffle._entityId = entityId;
        snaffle._x = x;
        snaffle._y = y;
        snaffle._vx = vx;
        snaffle._vy = vy;
        snaffle._state = state;
        snaffles.push_back(snaffle);
    //    entities.push_back(&snaffles[snaffles.size() - 1]);
    }
}

void turnZero(vector<Entity*>& entities, vector<Snaffle>& snaffles, int entityId, string entityType, int x, int y, int vx, int vy, int state)
{
   // cerr << entityId << entityType << endl;
    if (entityType != "SNAFFLE")
    {
        for (int i = 0; i < entities.size(); i++)
        {
            if (entities[i]->_entityId == -1 && entityType == entities[i]->_entityType)
            {
                entities[i]->_entityId = entityId;
                entities[i]->_x = x;
                entities[i]->_y = y; 
                entities[i]->_vx = vx; 
                entities[i]->_vy = vy;
                entities[i]->_state = state;
                break ;
            }
        }
    }
    else
    {
        Snaffle snaffle;
        snaffle._entityId = entityId;
        snaffle._x = x;
        snaffle._y = y;
        snaffle._vx = vx;
        snaffle._vy = vy;
        snaffle._state = state;
        snaffles.push_back(snaffle);
    //    entities.push_back(&snaffles[snaffles.size() - 1]);
    }
}

void print_stuff(vector<Entity*>& entities, vector<Snaffle>& snaffles)
{
    for (int i = 0; i < entities.size(); i++)
    {
       cerr << i << " " << entities[i]->_entityId << " " << entities[i]->_entityType << " x,y " << entities[i]->_x << " " << entities[i]->_y << endl;
    }
    for (int i = 0; i < snaffles.size(); i++)
        cerr << i << " " << snaffles[i]._entityId << " " << snaffles[i]._entityType << " x,y " << snaffles[i]._x << " " << snaffles[i]._y << endl;
}

/* NOTES
**  Maybe not two on the same ball
**  Wingardium if is gonna get hit 
**  Defense system isn't good
**  shot directly one if close to goal
**  grab to closest if cooldown ok else shoot or go near
**  if cannot grab::
**  --> if  moneyTime -> align
**
**  Matbe defensive wingardium on the closet ball to the opponent
*/
