/*This sketch is a simple version of the famous Simon Says game. You can use it and improved it adding
levels and everything you want to increase the diffuculty!

There are five buttons connected to A0, A1, A2, A3 and A4.
The buttons from A0 to A3 are used to insert the right sequence while A4 to start the game.

When a wrong sequence is inserted all the leds will blink for three time very fast otherwhise the
inserted sequence is correct.

Hardware needed:
5x pushbuttons
1x Blue led
1x Yellow led
1x Red led
1x Green Led
4x 1k resistors
4x 10k resisors
10x jumpers
*/

const int MAX_LEVEL = 100;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 1;

int velocity = 1000;

void setup(){
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);

	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);

	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
}

void loop(){
	if (level == 1)
		generate_sequence();//generate a sequence;

	if (digitalRead(A4) == LOW || level != 1){ //If start button is pressed or you're winning
		show_sequence();    //show the sequence
		get_sequence();     //wait for your sequence
	}
}

void show_sequence(){
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	for (int i = 0; i < level; i++){
		digitalWrite(sequence[i], HIGH);
		delay(velocity);
		digitalWrite(sequence[i], LOW);
		delay(200);
	}
}

void get_sequence(){
	int flag = 0; //this flag indicates if the sequence is correct
	for (int i = 0; i < level; i++){
		flag = 0;
		while(flag == 0){
			if (digitalRead(A0) == LOW){
				digitalWrite(5, HIGH);
				your_sequence[i] = 5;
				flag = 1;
				delay(200);
				if (your_sequence[i] != sequence[i]){
					wrong_sequence();
					return;
				}
				digitalWrite(5, LOW);
			}
			if (digitalRead(A1) == LOW){
				digitalWrite(4, HIGH);
				your_sequence[i] = 4;
				flag = 1;
				delay(200);
				if (your_sequence[i] != sequence[i]){
					wrong_sequence();
					return;
				}
				digitalWrite(4, LOW);
			}
			if (digitalRead(A2) == LOW){
				digitalWrite(3, HIGH);
				your_sequence[i] = 3;
				flag = 1;
				delay(200);
				if (your_sequence[i] != sequence[i]){
					wrong_sequence();
					return;
				}
				digitalWrite(3, LOW);
			}
			if (digitalRead(A3) == LOW){
				digitalWrite(2, HIGH);
				your_sequence[i] = 2;
				flag = 1;
				delay(200);
				if (your_sequence[i] != sequence[i]){
					wrong_sequence();
					return;
				}
				digitalWrite(2, LOW);
			}
		}
	}
	right_sequence();
}

void generate_sequence(){
	randomSeed(millis()); //in this way is really random!!!
	for (int i = 0; i < MAX_LEVEL; i++){
		sequence[i] = random(2,6);
	}
}

void wrong_sequence(){
	for (int i = 0; i < 3; i++){
		digitalWrite(2, HIGH);
		digitalWrite(3, HIGH);
		digitalWrite(4, HIGH);
		digitalWrite(5, HIGH);
		delay(250);
		digitalWrite(2, LOW);
		digitalWrite(3, LOW);
		digitalWrite(4, LOW);
		digitalWrite(5, LOW);
		delay(250);
	}
	level = 1;
	velocity = 1000;
}

void right_sequence(){
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	delay(250);
	digitalWrite(2, HIGH);
	digitalWrite(3, HIGH);
	digitalWrite(4, HIGH);
	digitalWrite(5, HIGH);
	delay(500);
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	delay(500);
	if (level < MAX_LEVEL);
		level++;
	velocity -= 50; //increase difficulty
}
