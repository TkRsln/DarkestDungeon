// TERM_PRO_V2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include<time.h>
#include<vector>
#include<string>
#include <algorithm>
#include<thread>
#include <windows.h>

using namespace std;
class Unit;

int  rd = 0;

void printUnit();
vector<Unit*> heros(0);

vector<Unit*> monsters(0);

COORD CursorPosition;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);




vector<string> box;

void setColor(int k) {
    SetConsoleTextAttribute(console, k);
}
void decideColor(char a) {
    int id = a - 'A';
    setColor(id);
}
void printStringColor(string s, int coolDown, bool endL) {

    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '&') {
            i++;
            decideColor(s[i]);
        }
        else {
            cout << s[i];
            if (coolDown != 0)this_thread::sleep_for(chrono::milliseconds(coolDown));
        }
    }
    if (endL)cout << endl;
}

void printTempString(string s, bool endL) {
    printStringColor(s, 1, endL);
}

void addString(string s) {
    system("cls");
    printUnit();
    //      #1      #2      #3
    if (box.size() < 15)box.push_back(s);
    else {
        box.erase(box.begin());
        box.push_back(s);
    }
    for (int i = 0; i < box.size(); i++) {
        if (i == box.size() - 1) {
            printStringColor(box[i], 10, true);
        }
        else
            printStringColor(box[i], 0, true);
    }
    this_thread::sleep_for(chrono::milliseconds(150));
}
void printAttack(string attacker, string skillName, string enemy, int damage, bool critic, bool isDead) {
    string s = "&I[&DAttack&I] &H" + attacker + " &Iused &H" + skillName + "&I skill on &H" + enemy + "&I with &M-" + to_string(damage) + "&I damage point";
    if (isDead)s += ", &H" + enemy + "&I is now &MDEAD&I!";
    if (critic)s += " (&MCritical Hit&I)";
    addString(s);
}
void printMiss(string attacker, string skillName, string enemy) {
    addString("&I[&DMISS&I] &H" + attacker + " &Nmiss &H" + skillName + "&I skill on &H" + enemy);
}
void printStun(string enemy) {
    addString("&I[&DStun&I] &H" + enemy + "&I is &GStunned&I!");
}
void printUtility(string unit, string sentence) {
    addString("&I[&DUtility&I] &H" + unit + sentence);
}
void printTeamTurn(bool isHero) {
    if (isHero)addString("&I[&DInfo&I] &HHero's turn");
    else addString("&I[&DInfo&I] &HMonsters&I' turn");
}
void printMoveSkill(string unitName, bool isForward) {
    if (isForward)addString("&I[&DMove&I]  &H" + unitName + "&I is moved &HFORWARD");
    else addString("&I[&DMove&I]  &H" + unitName + "&I is moved &HBACKWARD");
}
void printEffectRemove(string effectName, string unitName) {
    addString("&I[&DEffect&I] &H " + effectName + "&I on &H" + unitName + "&I is over!");
}
void printStunRemove(string unitName) {
    addString("&I[&DEffect&I]  due to StunEffect, &H" + unitName + "&I's turn has passed !");
}



void gotoXY(int x, int y)
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

void consoleSize(int x, int y) {
    HWND console = GetConsoleWindow();
    RECT ConsoleRect;
    GetWindowRect(console, &ConsoleRect);
    MoveWindow(console, ConsoleRect.left, ConsoleRect.top, x, y, TRUE);
}






class effect {
protected:
    int prot = 0;
    bool stun = 0;
    int round = 0;

public:
    effect(int prot, bool stun, int round) :prot(prot), stun(stun), round(round) {

    }

    int getProt() {
        return prot;
    }

    bool hasStun() {
        return stun;
    }

    int getRound() {
        return round;
    }
    void setRound() {
        if (round != -1)round--;
    }
    string getName() {
        string s = "";
        if (stun)s += "Stun Effect";
        if (prot != 0)s += "Protection Effect";
        return s;
    }
};

class StunSkill : public effect {
public:
    StunSkill() :effect(0, true, -1) {
    }
    bool hasChance(int stunBase, int stunResist, bool isCritic) {
        int r = rand() % 100;
        int c = stunBase - stunResist;
        if (isCritic)c += 20;
        return r <= c;
    }


};
class protectionEffect : public effect {
public:
    protectionEffect() :effect(20, false, 3) {

    }
};

class Skill {
protected:
    bool* position;
    bool* target;

public:

    Skill() {

    }
    Skill(bool* position, bool* target) {
        this->position = position;
        this->target = target;
    }
    void setPosition(bool* position) {
        this->position = position;
    }
    void setTarget(bool* target) {
        this->target = target;
    }
    bool getPosition(int position) {
        return this->position[position];
    }
    bool getTarget(int target) {
        return this->target[target];
    }
    bool* getTarget() {
        return this->target;
    }
    bool* getPosition() {
        return this->position;
    }

    virtual effect* getEffect(int stunResist, bool critic, int position) {
        return nullptr;
    }
    virtual string getName() {
        return "";
    }
};
class MoveSkill :public Skill {
public:

    static void swapHero(int c, int b) {

        iter_swap(heros.begin() + c, heros.begin() + b);
    }

    static void swapMonster(int c, int b) {

        iter_swap(monsters.begin() + c, monsters.begin() + b);
    }



};


class AttackSkill :public Skill {
protected:
    int baseAcc; //Each attack skill has a base accuracy
    double critMod;
    int dmgMod; //Each attack skill has a damage modifier
    bool stunEffect; //Each attack skill has stun effect (yes/no)
    int stunBase;
public:
    AttackSkill(bool* position, bool* target, int baseAcc, int dmgMod, int stunBase, double critMod, bool stunEffect) :Skill(position, target), stunBase(stunBase), baseAcc(baseAcc), critMod(critMod), stunEffect(stunEffect) {

    }
    void setBaseAc(int val) {
        baseAcc = val;
    }
    void setCriticMod(double val) {
        critMod = val;
    }
    void setDmgMod(int val) {
        dmgMod = val;
    }
    void setStunEffect(bool val) {
        stunEffect = val;
    }
    void setStunBase(int val) {
        stunBase = val;
    }
    int getBaseAc() { return baseAcc; }
    double getCriticMod() { return critMod; }
    int getDmgMod() { return dmgMod; }
    bool getStunEffect() { return stunEffect; }
    int getStunBase() { return stunBase; }


};
class Smite :public AttackSkill {
public:


    Smite() :AttackSkill(new bool[4]{ true,true,false,false }, new bool[4]{ true,true,false,false }, 85, 0, 0, 0, false) {

    }
    string getName() {
        return "Smite";
    }

};
class StunningBlow :public AttackSkill {
protected:

public:
    StunningBlow() :AttackSkill(new bool[4]{ true,true,false,false }, new bool[4]{ true,true,false,false }, 90, -50, 100, 0, true) {

    }
    effect* getEffect(int stunResist, bool critic, int position) {

        StunSkill* ss = new StunSkill();
        if (ss->hasChance(stunBase, stunResist, critic)) {
            return ss;
        }
        return nullptr;
    }
    string getName() {
        return "Stunning_Blow";
    }

};

class HolyLance :public AttackSkill {
protected:

    int move = -1;
public:
    HolyLance() :AttackSkill(new bool[4]{ false,false,true,true }, new bool[4]{ false,true,true,true }, 85, 0, 0, 6.5, false) {

    }
    effect* getEffect(int stunResist, bool critic, int position) {
        MoveSkill::swapHero(position, position + move);
        return nullptr;
    }
    string getName() {
        return "Holy_Lance";
    }

};

class MaceBash :public AttackSkill {
public:
    MaceBash() :AttackSkill(new bool[4]{ true,true,false,false }, new bool[4]{ true,true,false,false }, 85, 0, 0, 0, false) {

    }
    string getName() {
        return "Mace_Bash";
    }

};
class DazzlingLight :public AttackSkill {
public:
    DazzlingLight() :AttackSkill(new bool[4]{ false,false,true,true }, new bool[4]{ true,true,true,false }, 90, -75, 100, 5, true) {

    }

    effect* getEffect(int stunResist, bool critic, int position) {
        StunSkill* ss = new StunSkill();
        if (ss->hasChance(stunBase, stunResist, critic)) {
            return ss;
        }
        return nullptr;
    }
    string getName() {
        return "Dazzling_Light";
    }
};
class GraveYardSlash :public AttackSkill {
public:
    GraveYardSlash() :AttackSkill(new bool[4]{ true,true,true,false }, new bool[4]{ true,true,true,false }, 85, 0, 0, 6, false) {

    }
    string getName() {
        return "Grave_Yard_Slash";
    }
};
class GraveYardStumble :public AttackSkill { //Similar to HolyLance

public:
    int move = -1;
    GraveYardStumble() :AttackSkill(new bool[4]{ false,false,false,true }, new bool[4]{ true,true,false,false }, 45, -50, 0, 0, false) {

    }
    string getName() {
        return "Grave_Yard_Stumble";
    }
    effect* getEffect(int stunResist, bool critic, int position) {
        MoveSkill::swapMonster(position, position + move);
        return nullptr;
    }
};
class Axeblade :public AttackSkill {

public:
    Axeblade() :AttackSkill(new bool[4]{ true,true,false,false }, new bool[4]{ true,true,false,false }, 72, 0, 0, 6, false) {

    }
    string getName() {
        return "Axe_Blade";
    }
};
class DeadWeight :public AttackSkill {

public:
    DeadWeight() :AttackSkill(new bool[4]{ true,true,true,false }, new bool[4]{ true,true,false,false }, 82, -25, 100, 6, true) {

    }
    effect* getEffect(int stunResist, bool critic, int position) {
        StunSkill* ss = new StunSkill();
        if (ss->hasChance(stunBase, stunResist, critic)) {
            return ss;
        }
        return nullptr;
    }

    string getName() {
        return "Dead_Weight";
    }
};





class UtilitySkilll :public Skill {
protected:
    int effect_min;
    int effect_max;
    // targets = 0-> Self / 1-> Any / 2-> All;
public:

    UtilitySkilll(bool* position, int target, int effect_min, int effect_max) :Skill(position, new bool[4]{ 0 == target,1 == target,2 == target,3 == target }), effect_max(effect_max), effect_min(effect_min) {
        this->target[target] = true;
    }
    int getEffectMin() {
        return effect_min;
    }
    int getEffectMax() {
        return effect_max;
    }

};

class Divine_Grace :public UtilitySkilll {
public:
    /*
    Herhangi bir kişiye can verir
    */
    Divine_Grace() :UtilitySkilll(new bool[4]{ false,false,true,true }, 1, 4, 5) {

    }
    string getName() {
        return "Divine_Grace";
    }
};
class Divine_Comfort :public UtilitySkilll {
public:
    /*
    Herkese can verir
    */
    Divine_Comfort() :UtilitySkilll(new bool[4]{ false,true,true,true }, 2, 1, 3) {

    }
    string getName() {
        return "Divine_Comfort";
    }

};
class BulwarkOfFaith :public UtilitySkilll {
public:

    /*
    3 Raound 20 protec verir;
    */
    BulwarkOfFaith() :UtilitySkilll(new bool[4]{ true,true,false,false }, 0, 0, 0) {

    }
    effect* getEffect(int stunResist, bool critic, int position) {
        protectionEffect* s = new protectionEffect();
        return s;
    }
    string getName() {
        return "Bulwark_of_Faith";
    }
};
class KnittingBones :public UtilitySkilll {
public:
    /*
        Herhangi bir kiþiye can verir
    */
    KnittingBones() :UtilitySkilll(new bool[4]{ false,false,true,true }, 1, 2, 3) {

    }
    string getName() {
        return "Knitting_Bones";
    }
};



class Unit {
protected:
    int speedRating; //All units have a speed rating
    int maxHp;
    int accMod;  //Each unit has an accuracy modifier 
    int dodge;
    int baseCrit; //Each unit has a base critical chance 
    int minDmg; //Each unit hase a base minimum damage
    int maxDmg;  //Each unit has a base maximum damage
    int prot; //Defenders have a prtection rating
    int stunResist; //Each unit has a stun resist
    vector< AttackSkill* > attack_list;
    vector< UtilitySkilll* > utility_list;
    vector<effect*> unit_effect;
    bool played = false;
    int pos;
    bool isAlive = true;
    string id = "";
    int hp;  // Algorithm kütüphanesinin erişebilmesi için protected olmadı!


public:


    Unit(int speedRating, int accMod, int dodge, int baseCrit, int minDmg, int maxDmg, int prot, int hp, int stunResist, string id)
        :speedRating(speedRating), id(id), accMod(accMod), maxHp(hp), dodge(dodge), baseCrit(baseCrit), minDmg(minDmg), maxDmg(maxDmg), prot(prot), hp(hp), stunResist(stunResist)
    {
        this->speedRating += (rand() % 8);
    }

    int findDamage() {
        return hp - maxHp;
    }

    int getSpeedRaitng() {
        return speedRating;
    }
    void addToSpeedRating(int toAdd) {
        speedRating += toAdd;
    }
    void setSpeedRating(int speedRate) {
        speedRating = speedRate;
    }
    bool isAllive() {
        return isAlive;
    }
    bool hasStunEffect() {
        for (auto& e : unit_effect) {
            if (e->hasStun())return true;
        }
        return false;
    }
    void removeStunEffect() {
        for (auto& e : unit_effect) {
            if (e->hasStun()) {
                vector<effect*>::iterator position = find(unit_effect.begin(), unit_effect.end(), e);
                if (position != unit_effect.end()) { // == myVector.end() means the element was not found
                    printStunRemove(getName());
                    unit_effect.erase(position);
                    delete(e);
                }
                return;
            }
        }
    }
    void addEffect(effect* ef) {
        unit_effect.push_back(ef);
    }
    int getProtection() {
        int ekstra = 0;
        for (auto& a : unit_effect)
            try {
            ekstra += a->getProt();
        }
        catch (exception e) {}
        return prot + ekstra;
    }
    int getStunResist() {
        return stunResist;
    }
    void addHp(int a) {
        hp += a;
        if (hp > maxHp) {
            hp = maxHp;
        }
    }
    void setHp(int a) {
        hp = a;
        if (hp > maxHp) {
            hp = maxHp;
        }
    }
    int getHp() {
        return hp;
    }

    virtual void printInfo(int pos) {

    }
    virtual int useSkill(string skill, int pos) {
        return 0;
    }
    void endOfRound() {
        for (int i = 0; i < unit_effect.size(); i++) {
            effect* e = unit_effect[i];
            e->setRound();
            if (e->getRound() == 0) {
                vector<effect*>::iterator position = find(unit_effect.begin(), unit_effect.end(), e);
                if (position != unit_effect.end()) { // == myVector.end() means the element was not found
                    printEffectRemove(e->getName(), getName());
                    unit_effect.erase(position);
                    delete(e);
                }
            }
        }
    }
    bool isPlayed() {
        return played;
    }
    void setPlayed(bool play) {
        played = play;
    }
    void setPosition(int a) {
        pos = a;
    }
    int getPosition() {
        return pos;
    }

    bool Attack(Unit* target, int skillNo, int pos) {
        AttackSkill* sk = properAttackSkill(pos)[skillNo];
        int hitChance = sk->getBaseAc() + accMod - target->dodge;
        int r = rand() % 100;
        if (r <= hitChance) { //hit oldu
            int CriticalChance = baseCrit + sk->getCriticMod();
            double damage = 0.0;
            bool isCriticHit = rand() % 100 < CriticalChance;

            if (!isCriticHit) { //normal hit
                damage = minDmg + (rand() % (maxDmg - minDmg)); //kritik değilken
                damage *= (100.0 + sk->getDmgMod()) / 100.0;
            }
            else { //critic hit
                damage = 1.5 * maxDmg;
            }
            double rawDmg = damage * (100 + sk->getDmgMod()) / 100.0;
            double actualDmg = rawDmg - rawDmg * (target->getProtection() / 100.0);
            target->takeDamage(actualDmg);

            printAttack(getName(), sk->getName(), target->getName(), actualDmg, isCriticHit, !target->isAllive());
            effect* s = sk->getEffect(target->getStunResist(), isCriticHit, pos);
            if (s != nullptr) {
                target->addEffect(s);
                printStun(target->getName());
            }
            return true;

        }
        else {
            printMiss(getName(), sk->getName(), target->getName());
            return false;
        }
    }
    virtual void Utility(Unit* target, UtilitySkilll* u) {
        //Monster ve Herolar için ayrı ayrı yazıldı

    }
    virtual void takeDamage(double lost) {
    }
    virtual string getName() {
        return "";
    }
    void dead() {
        isAlive = false;
        removeAllEffects();
    }

    void removeAllEffects() {
        unit_effect.clear();
    }
    vector<AttackSkill*> properAttackSkill(int pos) { //pozisyonlara göre kullanılabilir atak skilleri

        vector<AttackSkill*> aa;
        for (auto& skill : attack_list) {
            if (skill->getPosition(pos)) {
                aa.push_back(skill);
            }
        }
        return aa;
    }
    vector<UtilitySkilll*> properUtilitySkill(int pos) { //neden boolean değil?

        vector<UtilitySkilll*> aa;
        for (auto& Skill : utility_list) {
            if (Skill->getPosition(pos)) {
                aa.push_back(Skill);
            }
        }
        return aa;
    }
};






class Hero :public Unit {
protected:
    int deathBlowResist; //Each hero has death blow resist

public:
    Hero(int speedRating, int accMod, int dodge, int baseCrit, int minDmg, int maxDmg, int prot, int hp, int stunResist, int deathBlowResist, string id)
        :Unit(speedRating, accMod, dodge, baseCrit, minDmg, maxDmg, prot, hp, stunResist, id), deathBlowResist(deathBlowResist)
    {
    }
    void takeDamage(double lost) { //enemy takes damage
        hp -= lost;

        if (hp < 0) hp = 0;
        if (hp == 0) {
            if (isAlive) {
                int r = rand() % 100;
                if (r > deathBlowResist) {
                    dead();
                }
            }

        }
    }
    void Utility(Unit* target, UtilitySkilll* u) {

        effect* s = u->getEffect(target->getStunResist(), false, 0);
        if (s != nullptr) {
            addEffect(s);
            if (s->getProt() != 0)
                printUtility(getName(), "&I received &H" + s->getName() + "&I for &H3 rounds&I! ");
        }

        if (u->getEffectMax() == 0)return;
        int r = u->getEffectMin() + (rand() % (u->getEffectMax() - u->getEffectMin()));
        if (u->getTarget(2)) {
            for (auto& m : heros) {
                m->addHp(r);
            }

            printUtility(getName(), "&I healed &Hwhole team&I members with &K+" + to_string(r) + "&I hp by &H" + u->getName() + "&I skill!");
        }
        else if (u->getTarget(1)) {
            target->addHp(r);
            printUtility(getName(), "&I healed &H" + target->getName() + "&I with &K+" + to_string(r) + "&I hp by &H" + u->getName() + "&I skill!");
        }
        else {
            addHp(r);

            printUtility(getName(), "&I healed &Hitself&I with &K+" + to_string(r) + "&I hp by &H" + u->getName() + "&I skill!");
        }

    }

};

class Crusader :public Hero {

public:
    Crusader(string id) :Hero(1, 0, 5, 3, 6, 12, 0, /*HP*/33, 40, 67, id) {

        attack_list.push_back(new Smite);
        attack_list.push_back(new StunningBlow);
        attack_list.push_back(new HolyLance);
        utility_list.push_back(new BulwarkOfFaith);
    }
    void printInfo(int pos) {
        addString("");
        printTempString("&I[&DTurn&I]   Now &H" + getName() + "&I's turn.", true);
        printTempString("&I[&DSkills&I]  You can use these Attack skills:     &H", false);
        for (auto& as : properAttackSkill(pos)) {
            cout << as->getName() + "  ";
        }
        printTempString("   ", true);
        printTempString("&I[&DSkills&I]  You can use these Utility skills:    &H", false);
        for (auto& as : properUtilitySkill(pos)) {
            cout << as->getName() + "  ";
        }

        printTempString("   ", true);
        printTempString("&I[&DSkills&I]  Or you can move:                     &H", false);
        if (pos != 0)printTempString("Forward     ", false);//cout << "Forward     ";
        if (pos != 3)printTempString("Backward    ", false); //cout << "Backward    ";
        //cout << "          " << endl;
        printTempString("     ", true);
    }
    string getName() {
        return "Crusader" + id;
    }

};

class Vestal :public Hero {
public:
    Vestal(string id) :Hero(4, 0, 0, 1, 4, 8, 0, /*HP*/24, 30, 77, id) {

        attack_list.push_back(new MaceBash);
        attack_list.push_back(new DazzlingLight);
        utility_list.push_back(new Divine_Grace);
        utility_list.push_back(new Divine_Comfort);

    }


    int useSkill(string a) {
        cout << speedRating << " " << "Vestal" << endl;
        return 0;
    }

    void printInfo(int pos) {
        addString("");

        printTempString("&I[&DTurn&I]   Now &H" + getName() + "&I's turn.", true);
        printTempString("&I[&DSkills&I]  You can use these Attack skills:     &H", false);
        for (auto& as : properAttackSkill(pos)) {
            cout << as->getName() + "  ";
        }

        printTempString("   ", true);
        printTempString("&I[&DSkills&I]  You can use these Utility skills:    &H", false);
        for (auto& as : properUtilitySkill(pos)) {
            cout << as->getName() + "  ";
        }

        printTempString("   ", true);
        printTempString("&I[&DSkills&I]  Or you can move:                     &H", false);
        if (pos != 0)printTempString("Forward     ", false);//cout << "Forward     ";
        if (pos != 3)printTempString("Backward    ", false); //cout << "Backward    ";
        printTempString("   ", true);

    }
    string getName() {
        return "Vestal" + id;
    }
};

class Monster : public Unit {
public:
    Monster(int speedRating, int accMod, int dodge, int baseCrit, int minDmg, int maxDmg, int prot, int hp, int stunResist, string id)
        :Unit(speedRating, accMod, dodge, baseCrit, minDmg, maxDmg, prot, hp, stunResist, id)
    {}
    void Utility(Unit* target, UtilitySkilll* u) {

        effect* s = u->getEffect(target->getStunResist(), false, 0);
        if (u->getEffectMax() == 0)return;
        int r = u->getEffectMin() + (rand() % (u->getEffectMax() - u->getEffectMin()));
        if (u->getTarget(2)) {
            for (int i = 0; i < 4; i++) {
                monsters[i]->addHp(r);
            }
            printUtility(getName(), "&I healed &Hwhole team&I members with &K+" + to_string(r) + " &IHp by &I" + u->getName() + " skill!");
        }
        else if (u->getTarget(1)) {
            Unit* min = nullptr;
            int minimum = 100;
            for (int i = 0; i < 4; i++) {
                if (monsters[i]->isAllive() && monsters[i]->findDamage() < minimum) {
                    min = monsters[i];
                    minimum = min->findDamage();
                }
            }
            min->addHp(r);
            printUtility(getName(), "&I healed &H" + min->getName() + "&I with &K+" + to_string(r) + "&I hp by &H" + u->getName() + "&I skill!");
        }
        else {
            addHp(r);
            printUtility(getName(), "&I healed &Hitself&I with &K+" + to_string(r) + " hp by &H" + u->getName() + "&I skill!");
        }

    }
    int useSkill(string s, int pos) {
        int mr = rand() % 100;
        if (mr < 10) {//move kullanacak

            mr = rand() % 100;
            if ((mr > 50 && pos != 0) || pos == 3) { //FORWARD

                MoveSkill::swapMonster(pos, pos - 1);
                printMoveSkill(getName(), true);
                return -1;
            }
            else if (pos != 3) {             //BACKWARD
                MoveSkill::swapMonster(pos, pos + 1);
                printMoveSkill(getName(), false);
                return 1;
            }
        }
        else { //random skill kullanacak.
            mr = rand() % 100;


            if ((mr < 20 && properUtilitySkill(pos).size() != 0 && canUseUtility(pos)) || (properUtilitySkill(pos).size() != 0 && properAttackSkill(pos).size() == 0)) {//20 den küçükse utility kullanıyor
                int a = properUtilitySkill(pos).size();
                mr = rand() % a;
                for (int next = 0; true; next++) {
                    if (canUseThisUtility(pos, (mr + next) % a)) {
                        mr = (mr + next) % a;
                        break;
                    }
                    if (next > 10)break;
                }
                int randomMonster = rand() % 4;
                Utility(monsters[randomMonster], properUtilitySkill(pos)[mr]);

            }
            else if (properAttackSkill(pos).size() != 0) {  //20 den büyükse attack kullanıyor
                int a = properAttackSkill(pos).size(); //hangi attack skilli kullanacağımız

                mr = rand() % a;
                bool* target = properAttackSkill(pos)[mr]->getTarget();
                int dene = 0;
                bool found = false;
                while (!found) {
                    for (int i = 0; i < 4; i++) {
                        if (target[i] && heros[i]->isAllive()) {
                            found = true;
                            break;
                        }
                    }
                    mr = rand() % a;
                    target = properAttackSkill(pos)[(mr + dene) % properAttackSkill(pos).size()]->getTarget();
                    dene++;
                    if (dene > 4) {

                        addString("&I[&MMonster&I]  &H" + getName() + "&I couldn't found alive hero to use &M" + properAttackSkill(pos)[mr]->getName() + "&I skill!");
                        return false;
                    }
                }
                found = false;
                a = rand() % 4;
                int targetNo = 0;
                for (int i = 0; i < 12; i++) {

                    if (i >= a && target[i % 4] && heros[i % 4]->isAllive()) {
                        targetNo = i % 4;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    addString("&I[&MMonster&I]  &H" + getName() + "&I couldn't found alive hero!");
                    return false;
                }
                bool at = Attack(heros[targetNo], mr, pos);
                if (at && heros[targetNo]->getHp() == 0 && heros[targetNo]->isAllive())addString("&I[&DDeadDoor&I] &H" + heros[targetNo]->getName() + "&I is in &MDeadDoor&I, still Alive!");
            }
        }
        return 0;
    }

    bool canUseThisUtility(int pos, int utilityNo) {
        for (int i = 0; i < 4; i++) {
            if (properUtilitySkill(pos)[utilityNo]->getTarget(i))
                if (monsters[i]->isAllive())return true;
        }
        return false;
    }

    bool canUseUtility(int pos) {
        for (int s = 0; s < properUtilitySkill(pos).size(); s++) {
            for (int i = 0; i < 4; i++) {
                if (properUtilitySkill(pos)[s]->getTarget(i))
                    if (monsters[i]->isAllive())return true;
            }
        }
        return false;
    }

    void takeDamage(double lost) { //enemy takes damage
        hp -= lost;

        if (hp <= 0) {
            hp = 0;
            dead();
        }
    }
};

class Bone_Soldier :public Monster {
public:
    Bone_Soldier(string id) :Monster(2, 0, 10, 0, 3, 8, 15, 10, 25, id) {

        attack_list.push_back(new GraveYardSlash);
        attack_list.push_back(new GraveYardStumble);

    }
    string getName() {
        return "Bone_Soldier" + id;
    }
};

class Bone_Defender :public Monster {
public:
    Bone_Defender(string  id) :Monster(1, 0, 8, 0, 2, 4, 45, 22, 45, id) {

        attack_list.push_back(new Axeblade);
        attack_list.push_back(new DeadWeight);
        utility_list.push_back(new KnittingBones);

    }
    string getName() {
        return "Bone_Defender" + id;
    }

};




#pragma region Main Fonksiyon




void resetUnitsPlay() {
    for (auto& hero : heros) {
        hero->setPlayed(false);
        hero->endOfRound();
    }
    for (auto& monsters : monsters) {
        monsters->setPlayed(false);
        monsters->endOfRound();
    }
}

void setRandomRating() {

    for (auto& hero : heros) {
        hero->setSpeedRating(rand() % 8);
        if (dynamic_cast<Vestal*>(hero) == nullptr)
        {
            hero->addToSpeedRating(4);
        }
        else {
            hero->addToSpeedRating(1);
        }
    }
    for (auto& monster : monsters) {
        monster->setSpeedRating(rand() % 8);;
        if (dynamic_cast<Bone_Defender*>(monster) == nullptr)
        {
            monster->addToSpeedRating(1);
        }
        else {
            monster->addToSpeedRating(2);
        }
    }
}

int getMaxHero() {
    int max_Sıra = -1;
    int maxValue = 0;
    for (int i = 0; i < 4; i++) {
        auto& hero = heros[i];
        if (!hero->isAllive())hero->setPlayed(true);
        if (!hero->isPlayed()) {
            if (hero->getSpeedRaitng() > maxValue) {
                max_Sıra = i;
                maxValue = hero->getSpeedRaitng();

            }
        }

    }

    return max_Sıra;

}

int getMaxMonster() {
    int max_Sıra = -1;
    int maxValue = 0;
    for (int i = 0; i < 4; i++) {
        auto& monster = monsters[i];
        if (!monster->isAllive())monster->setPlayed(true);
        if (!monster->isPlayed() && monster->isAllive()) {
            if (monster->getSpeedRaitng() > maxValue) {
                max_Sıra = i;
                maxValue = monster->getSpeedRaitng();

            }
        }
    }

    return max_Sıra;

}

int winnerTeam() {
    int count = 0;
    for (auto& a : heros)if (!a->isAllive())count++;
    if (count == 4)return 2;//monster kazandıysa
    count = 0;
    for (auto& a : monsters)if (!a->isAllive())count++;
    if (count == 4)return 1;//Hero kazandıysa
    return 0; // kazanan takım yoksa
}



bool iequals(const string& a, const string& b)
{
    return std::equal(a.begin(), a.end(),
        b.begin(), b.end(),
        [](char a, char b) {
            return tolower(a) == tolower(b);
        });
}

int getMonsterForAttack(AttackSkill* as) {

    printTempString("&I[&OInput&I]  Please type name of the monster that you want to attack with &H" + as->getName() + "&I skill!", true);
    printTempString("&I[&OInput&I]  You can attack to: &H", false);
    bool found = false;
    for (int i = 0; i < 4; i++) {
        if (as->getTarget(i) && monsters[i]->isAllive()) {
            cout << monsters[i]->getName() << "  ";
            found = true;
        }
    }
    if (!found) {
        addString("&I[&MError&I] Nobody was found to use &H" + as->getName() + "&I skill!");
        return -1;
    }
    cout << endl;
    while (true) {
        string name;
        cin >> name;
        bool msg = false;
        for (int i = 0; i < 4; i++) {
            if (iequals(name, monsters[i]->getName()))
            {
                if (monsters[i]->isAllive()) {
                    if (as->getTarget(i))return i;
                    else {
                        printTempString("&I[&OInput&I] Hero can't use attack skill for the &H" + monsters[i]->getName() + "&I. Monster is not in the range!&H", true);
                        msg = true;
                    }
                }
                else {
                    printTempString("&I[&OInput&I]  The &H" + monsters[i]->getName() + "&I you selected has already &Mdead&I!&H", true);
                    msg = true;
                }
            }
            if (i == 3 && !msg)
                printTempString("&I[&MError&I]  There is no monster with this name, type again!&H", true);
        }
    }
}
int getHeroForUtility(UtilitySkilll* u) {

    printTempString("&I[&OInput&I]  Please type name of the hero that you want to use &H" + u->getName() + "&I skill on!&H", true);
    printTempString("&I[&OInput&I]  You can use utility on: &H", false);
    bool found = false;
    for (int i = 0; i < 4; i++) {
        if (u->getTarget(i) && heros[i]->isAllive()) {
            cout << heros[i]->getName() << "  ";
            found = true;
        }
    }
    cout << endl;
    if (!found) {
        addString("&I[&MError&I]  Nobody was found to use &H" + u->getName() + "&I skill!&H");
        return -1;
    }

    while (true) {
        string name;
        cin >> name;
        bool msg = false;
        for (int i = 0; i < 4; i++) {
            if (iequals(name, heros[i]->getName()))
            {
                if (heros[i]->isAllive()) {
                    if (u->getTarget(i))return i;
                    else {
                        printTempString("&I[&OInput&I] Hero can't use utility skill for this hero. Hero is not in the range!&H", false);
                        msg = true;
                    }
                }
                else {
                    printTempString("&I[&OInput&I]  The hero you selected has already dead!&H", true);
                    msg = true;
                }
            }
            if (i == 3 && !msg)
                printTempString("&I[&MError&I]  There is no hero with this name, type again!&H", true);
        }

    }
}

int getSkillNumberFromUserForHero(int pos) {
    printTempString("&I[&OInput&I]  Please type the &Hskill name&I that you want to use!&H", true);
    while (true) {
        bool msg = false;
        string name;
        cin >> name;
        for (int i = 0; i < heros[pos]->properAttackSkill(pos).size(); i++) {
            if (iequals(name, heros[pos]->properAttackSkill(pos)[i]->getName()))
            {
                return i;
            }
        }
        for (int i = 0; i < heros[pos]->properUtilitySkill(pos).size(); i++) {
            if (iequals(name, heros[pos]->properUtilitySkill(pos)[i]->getName()))
            {
                return 20 + i;
            }
        }
        if (iequals(name, "backward")) {
            if (pos != 3)return 101;
            else {
                printTempString("&I[&MError&I] &H" + heros[pos]->getName() + "&I can not move &HBackward&I!&H", true);
                msg = true;
            }
        }
        if (iequals(name, "forward")) {

            if (pos != 0)return 100;
            else {

                printTempString("&I[&MError&I] &H" + heros[pos]->getName() + "&I can not move &HForward&I!&H", true);
                msg = true;
            }
        }
        if (!msg)
            printTempString("&I[&MError&I]  Please type the name correctly!&H", true);
    }

}



int getSkillNumberFromuserForMonster(int pos) {

    printTempString("&I[&OInput&I]  Please type the skill name that you want to use!&H", true);
    string name;
    cin >> name;
    while (true) {
        for (int i = 0; i < monsters[pos]->properAttackSkill(pos).size(); i++) {
            if (iequals(name, monsters[pos]->properAttackSkill(pos)[i]->getName()))
            {
                return i;
            }
        }
        for (int i = 0; i < monsters[pos]->properUtilitySkill(pos).size(); i++) {
            if (iequals(name, monsters[pos]->properUtilitySkill(pos)[i]->getName()))
            {
                return i;
            }
        }
        printTempString("&I[&MError&I]  Please type the name correctly!&H", true);
    }

}

void printUnit() {
    gotoXY(51, 3);
    printStringColor(" &LROUND " + to_string(rd) + "&H", 0, true);
    for (int i = 0; i < 4; i++) {
        gotoXY(3 + (12 * i), 7);
        cout << "#" << 4 - i;
        if (!heros[3 - i]->isAllive()) {
            continue;

        }
        gotoXY(3 + (12 * i), 5);
        cout << heros[3 - i]->getName();
        gotoXY(3 + (i * 12), 6);
        printStringColor("&IHP: &H" + to_string(heros[3 - i]->getHp()) + "", 0, true);
    }
    for (int i = 0; i < 4; i++) {
        gotoXY(58 + (15 * i), 7);
        cout << "#" << i + 1;
        if (!monsters[i]->isAllive()) {
            continue;
        }
        gotoXY(58 + (15 * i), 5);
        cout << monsters[i]->getName();
        gotoXY(58 + (i * 15), 6);
        printStringColor("&IHP: &H" + to_string(monsters[i]->getHp()) + "", 0, true);
    }
    gotoXY(0, 10);
}

#pragma endregion








int main()
{
    

    consoleSize(1200, 600);
    srand(time(0));


    unique_ptr<Crusader> c1(new Crusader("1"));
    unique_ptr<Crusader> c2(new Crusader("2"));
    unique_ptr<Vestal> v1(new Vestal("1"));
    unique_ptr<Vestal> v2(new Vestal("2"));

    heros.push_back(c1.get());
    heros.push_back(c2.get());
    heros.push_back(v1.get());
    heros.push_back(v2.get());

    unique_ptr<Bone_Soldier> bs1(new Bone_Soldier("1"));
    unique_ptr<Bone_Defender> bd1(new Bone_Defender("1"));
    unique_ptr<Bone_Soldier> bs2(new Bone_Soldier("2"));
    unique_ptr<Bone_Defender> bd2(new Bone_Defender("2"));

    monsters.push_back(bs1.get());
    monsters.push_back(bd1.get());
    monsters.push_back(bs2.get());
    monsters.push_back(bd2.get());

    int winner = 0;
    bool first = false;
    printUnit();
    while (winner == 0) {
        rd++;
        for (int i = 0; i < 8; i++) {

            if (first)this_thread::sleep_for(chrono::milliseconds(500));
            first = true;
            int sıra = 0;
            bool isHero = false;
            string s = "a";

            int hero_max = getMaxHero();
            int monster_max = getMaxMonster();
            if ((monster_max == -1 && hero_max != -1) || (hero_max != -1 && heros[hero_max]->getSpeedRaitng() > monsters[monster_max]->getSpeedRaitng())) {
                int used = false;
                int moved = 0;
                if (heros[hero_max]->hasStunEffect()) {
                    heros[hero_max]->removeStunEffect();
                    heros[hero_max]->setPlayed(true);
                    continue;
                }
                while (!used) {

                    heros[hero_max]->printInfo(hero_max);

                    int skill_no = getSkillNumberFromUserForHero(hero_max);
                    if (skill_no == 100) {//FORWARD
                        MoveSkill::swapHero(hero_max, hero_max - 1);
                        moved = -1;
                        printMoveSkill(heros[hero_max + moved]->getName(), true);
                        used = true;
                    }
                    else if (skill_no == 101) {//BACKWARD
                        moved = 1;
                        MoveSkill::swapHero(hero_max, hero_max + 1);
                        printMoveSkill(heros[hero_max + moved]->getName(), false);
                        used = true;
                    }
                    else if (skill_no >= 20 && skill_no < 30) { // Utilitidir demektir eğer 20'den büyükse
                        skill_no -= 20;
                        if (heros[hero_max]->properUtilitySkill(hero_max)[skill_no]->getTarget(2) || heros[hero_max]->properUtilitySkill(hero_max)[skill_no]->getTarget(0)) {
                            heros[hero_max]->Utility(heros[hero_max], heros[hero_max]->properUtilitySkill(hero_max)[skill_no]);
                            used = true;
                        }
                        else {
                            int teamMate = getHeroForUtility(heros[hero_max]->properUtilitySkill(hero_max)[skill_no]);
                            if (teamMate == -1)continue;
                            heros[hero_max]->Utility(heros[teamMate], heros[hero_max]->properUtilitySkill(hero_max)[skill_no]);
                            used = true;
                        }

                    }
                    else {
                        int enemy = getMonsterForAttack(heros[hero_max]->properAttackSkill(hero_max)[skill_no]);
                        if (enemy == -1)continue;

                        heros[hero_max]->Attack(monsters[enemy], skill_no, hero_max);
                        used = true;
                    }
                }
                heros[hero_max + moved]->setPlayed(true);
                winner = winnerTeam();
                if (winner != 0)break;
            }
            else if (monster_max != -1) {
                if (monsters[monster_max]->hasStunEffect()) {
                    monsters[monster_max]->removeStunEffect();
                    monsters[monster_max]->setPlayed(true);
                    continue;
                }
                printTeamTurn(false);
                int a = monsters[monster_max]->useSkill(s, monster_max);
                monsters[monster_max + a]->setPlayed(true);
                winner = winnerTeam();
                if (winner != 0)break;

            }
            else {}
        }

        resetUnitsPlay();
        addString("");
        this_thread::sleep_for(chrono::milliseconds(500));

    }
    addString("");
    addString("");
    addString("");
    if (winner == 2) {
        addString("&H[ &EBONE SOLDIERS WON THE MATCH &H]");
    }
    else {
        addString("     &H[ &DHEROES WON THE MATCH &H]");
    }
    cout << endl;
    cout << endl;
    cout << endl;
    //*/
    
}
