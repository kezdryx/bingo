#include <iostream>
using namespace std;

int main() 
{

	int choice;
	int torq;
	int rpm;
	int x;
	int radianspm = 5252;
	float hp;
	int amps;
	int volt;
	float eff;
	int y;
	int z = 746;
	float ehp;
	int lbs;
	int stairs;
	float gravconst = 9.81;
	int timerun;
	float watts;
	float hhp;

	cout << "Horsepower Calculator by Wes W.\n";
	cout << "With this program, you can calculate the horsepower of a regular\n";
	cout << "gasoline engine, an electric motor, or even your own running\n";
	cout << "horsepower. The choice is yours!";
	cout << "\n";
	
	cout << "Choose number 1 for gasoline engine,\n";
	cout << "Number 2 for an electric motor,\n";
	cout << "Or, Number 3 for a human running :";
	cin >> choice;

		switch(choice)
		{

		case 1:
			cout << "Please enter the torque value of the engine: ";
			cin >> torq;
			cout << "\n";
			cout << "Please enter the revolutions per minute: ";
			cin >> rpm;
			cout << "Calculating horsepower...\n";

		/* calculation for gas engine */

			x = (torq * rpm);
			hp = (x / radianspm);

			cout << "Your vehicle has " << hp << " Horsepower!\n";
			cout << "\n";

			break;

		case 2:
			cout << "Please enter the amps of the motor: ";
			cin >> amps;
			cout << "\n";
			cout << "Please enter the voltage of the motor: ";
			cin >> volt;
			cout << "\n";
			cout << "Please enter the efficiency percent of the motor ";
			cout << "in decimal form: ";
			cin >> eff;
			cout << "Calculating horsepower...\n";

		/* calculation for electric engine */
		
			y = (volt * amps * eff); 
			ehp = (y / z);

			cout << "Your electric motor has " << ehp << " horsepower!\n";
			cout << "\n";

			break;

		case 3:
			cout << "Please note!! For this part of the program, you need a\n";
			cout << "staircase for this to work. Ok?\n";
			cout << "Please enter your weight in pounds: ";
			cin >> lbs;
			cout << "Now heres the hard part: go to a staircase, measure the\n";
			cout << "total height of the staircase. when you get the number\n";
			cout << "of feet the staircase is high, Please enter it here: ";
			cin >> stairs;
			cout << "Finally, using a stopwatch, get a running start, when\n";
			cout << "your foot lands on the first step, start the stopwatch,\n";
			cout << "stop it when both feet are at the top of the stairs.\n";
			cout << "Please enter the number of seconds it took to run up: ";
			cin >> timerun;
			cout << "Calculating horsepower...\n";

		/* calculation for human running */
			
			watts = (lbs * gravconst * stairs) / timerun;
			hhp = (watts / z);

			cout << "Your human legs have a horsepower of " << hhp << "\n";
			cout << "\n";

			break;

		default:
			cout << "call me.. little sunshine..\n";
			cout << "\n";

			break;

	}

	return 0;

}
