#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

int main() {

int hhpoints = 250;
int mhpoints = 500;
int shield = 125;
int hasShield = 1;
int hasPotion = 1;
int defense = 0;
char hname[50] = " ";
int ap,kp,sp,mx,ru,heal,poop,attack;

intro:

cout << "Dungeon Fight (Level 3) by Wes. \n";
cout << "What is your name?\n";
cin >> hname;

cout << "Welcome " << hname << " to Dungeon Fight!\n";
cout << "Your final opponent is Ribsplitter the Manticore\n";
cout << "This fight is extremely unfair, so this time we have\n";
cout << "given you a sword, shield and healing potion.\n";
cout << "BEWARE! you can only use the healing potion ONCE and the shield can take only so much damage.";
cout << "Ribsplitter weilds a mighty two-handed, double sided battle axe.\n";

check:

if (hhpoints <= 0) {
  goto hslain;
  }

if (mhpoints <= 0) {
  goto mslain;
  }

stats:

cout << hname << "\tHp:\t" << hhpoints << "\n";
cout << "Ribsplitter\tHp:\t" << mhpoints << "\n\n";

hero:

srand(time(NULL));

cout << "Choose your attack or defense:\n";
cout << "1=Punch 2=Kick 3=Sword 4=Shield(if applicable) 5=Heal Potion(if applicable)\n";
cin >> attack;

if (attack == 1) {
  ap = rand() % 50 + 1;
  cout << hname << " Punches Ribsplitter for " << ap << " damage!\n"; 
  mhpoints = mhpoints - ap;
  goto master;
  } 

if (attack == 2) {
  kp = rand() % 75 + 1;
  cout << hname << " kicks Ribsplitter for " << kp << " damage!\n";
  mhpoints = mhpoints - kp;
  goto master;
  }

if (attack == 3) {
  sp = rand() % 100 + 1;
  cout << hname << " slashes Ribsplitter for " << sp << " damage!\n"; 
  mhpoints = mhpoints - sp;
  goto master;
  } 

if ((attack == 4) && (hasShield == 1)) {
  cout << hname << " raises their shield, ready to defend\n";
  defense = 1;
  goto master;
  }

if ((attack == 4) && (hasShield == 0)) {
  cout << "sorry, your shield is broken\n";
  goto master;
  }

if ((attack == 5) && (hasPotion == 1)) {
  cout << hname << " drinks their healing potion\n";
  heal = rand() % 250 + 1;
  hhpoints = hhpoints + heal;
  hasPotion = 0;
  goto master;
  }

if ((attack == 5) && (hasPotion == 0)) {
  cout << "sorry, you already drank the healing potion.\n";
  goto master;
  }

master:

poop = rand() % 2 + 1;

if ((poop == 1) && (defense == 1)) {
  mx = rand() % 80 + 1;
  cout << "Ribsplitter smashes " << hname << " with his battle axe for " << mx << " damage!\n"; 
    cout << "But " << hname << "has their shield up to block the attack!";
    shield = shield - mx;
    defense = 0; 
    goto check;
      }

if ((poop == 1) && (defense == 0)) {
  mx = rand() % 80 + 1;
   cout << "Ribsplitter smashes " << hname << " with his battle axe for " << mx << " damage!\n";
   hhpoints = hhpoints - mx;
   goto check;
 }

if ((poop == 2) && (hasShield == 1)) {
  ru = rand() % 100 + 1;
  cout << "Ribsplitter rushes and CRASHES into " << hname << " for " << ru << " damage!\n";
    cout << "Ribsplitter DESTROYS " << hname << "'s shield!!\n";
    defense = 0;
    hasShield = 0;
    goto check;
   }

if ((poop == 2) && (hasShield == 0)) {
  ru = rand() % 100 + 1;
  cout << "Ribsplitter rushes and CRASHES into " << hname << " for " << ru << " damage!\n";
  hhpoints = hhpoints - ru;
    goto check;
   }

mslain:

cout << hname << " has won the battle! you are a true champion, " << hname << "!\n";
goto endgame;

hslain: 

cout << hname << " has died, Ribsplitter eats " << hname << "'s corpse!\n";
goto endgame;

endgame:

return 0;
}