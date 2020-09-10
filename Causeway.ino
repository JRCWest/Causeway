/*******************************
*
* "Causeway" - a game by jwest.
*
*******************************/

int gameState = 0;

Color playerColors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
byte playerColorFlags[6] = {'C_RED', 'C_ORANGE', 'C_YELLOW', 'C_GREEN', 
							'C_BLUE', 'C_MAGENTA'};
byte currentColor;
int colorArrayLen = 6;
int colorCycleCounter = 0;
int fullCycleCounter = 0;

Timer lockTimer;
bool timerRunning;
int lockTimerInterval = 3000; // 3 Seconds

enum Flags
{
	LOCK,
	RESET,
};

void setup()
{
	randomize();
	shuffleColors();
}

void loop()
{

	switch (gameState)
	{
		case 0:
		// Idle/blank state, awaiting button presses.
			setColor(WHITE);
			if (buttonSingleClicked())
			{
				gameState = 1;
			}
		break;

		case 1:
		// Cycle through colors on button press.
		// Set timer for lock check.
			currentColor = playerColorFlags[colorCycleCounter];
			setValueSentOnAllFaces(currentColor);
			setColor(playerColors[colorCycleCounter]);
			
			FOREACH_FACE(f)
			{
				if ((getLastValueReceivedOnFace(f) == currentColor))
				{
					//timerRunning = true;
					//lockTimer.set(lockTimerInterval);
					gameState = 2;
				}
			}

			if (buttonSingleClicked())
			{
				timerRunning = false;
				if (colorCycleCounter == (colorArrayLen - 1))
				{
					colorCycleCounter = 0;
					fullCycleCounter += 1;
					lockTimerInterval -= 500;
				}
				else
				{
					colorCycleCounter += 1;
				}
			}
			if (timerRunning && lockTimer.isExpired())
			{
				//timerRunning = false;
				gameState = 2;
			}
		break;

		case 2:
		// Lock state
			setColorOnFace(WHITE, 0);
			setColorOnFace(WHITE, 2);
			setColorOnFace(WHITE, 4);
			setValueSentOnAllFaces(LOCK);
			FOREACH_FACE(f)
			{
				if (getLastValueReceivedOnFace(f) == RESET)
				{
					setValueSentOnAllFaces(RESET);
					gameState = 0;
				}
			}
			if (buttonLongPressed())
			{
				setValueSentOnAllFaces(RESET);
				gameState = 0;
			}
		break;
	}

}

void shuffleColors()
{
	for (int i=0; i < colorArrayLen-1; i++)
	{
		int n = random(5);  // Integer from 0 to questionCount-1
   		Color temp_color = playerColors[n];
		byte temp_flag = playerColorFlags[n];
   		
		playerColors[n] =  playerColors[i];
		playerColorFlags[n] = playerColorFlags[i];
   		
		playerColors[i] = temp_color;
		playerColorFlags[i] = temp_flag;
	}
}
