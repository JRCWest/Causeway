/*******************************
*
* "Causeway" - a game by jwest.
*
*******************************/

int gameState = 0;

Color playerColors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
int colorArrayLen = 6;
int colorCycleCounter = 0;
int fullCycleCounter = 0;

Timer lockTimer;
Timer lockAnimTimer;
int lockAnimCurrentFace = 0;
bool timerRunning;
int lockTimerAnimInterval = 250; // For stepping animation
int lockTimerInterval = 3000; // 3 Seconds
int lockingFace;

enum Flags
{
	EMPTY,	//Clear value sent
	C_RED,
	C_ORANGE,
	C_YELLOW,
	C_GREEN,
	C_BLUE,
	C_MAGENTA,
	LOCK,
	RESET
};

byte playerColorFlags[6] = {C_RED, C_ORANGE, C_YELLOW, C_GREEN, 
							C_BLUE, C_MAGENTA};
//byte neighborColorFlags[6] = {};
byte currentColor;
byte matchingFaces[6] = {0,0,0,0,0,0};

void setup()
{
	randomize();
	shuffleColors();
	gameState = 0;
}

void loop()
{

	switch (gameState)
	{
		case 0:
		// Idle/blank state, awaiting button presses.
			setValueSentOnAllFaces(EMPTY);
			setColor(WHITE);
			if (buttonSingleClicked())
			{
				gameState = 1;
			}
		break;

		case 1:
		// Cycle through colors on button press.
		// Set timer for lock check.
		
			//setColor(playerColors[colorCycleCounter]);
			currentColor = playerColorFlags[colorCycleCounter];
			setValueSentOnAllFaces(currentColor);
			
			// Update matching colors.
			FOREACH_FACE(f)
			{
				// Compare to other matching faces, skipping empty.
				if (getLastValueReceivedOnFace(f) == 0 && matchingFaces[f] == 0)
				{
					continue;
				}
				// If face matches but expired, or new flag detected, reset timer.
				if ((isValueReceivedOnFaceExpired(f)) || 
					(getLastValueReceivedOnFace(f) != matchingFaces[f]))
				{
					matchingFaces[f] = 0;
				}

				// Detect matching faces, begin lock timer.
				if ((getLastValueReceivedOnFace(f) == currentColor) && 
					!isValueReceivedOnFaceExpired(f))
				{
					matchingFaces[f] = getLastValueReceivedOnFace(f);
				}
			}
			
			// Run timer if matching faces exist, else stop timer.
			if (matchingFacesCounter() == 0)
			{
				timerRunning = false;
				setColor(playerColors[colorCycleCounter]);
			}
			else if (matchingFacesCounter() != 0 && !timerRunning) 
			{
				timerRunning = true;
				lockTimer.set(lockTimerInterval);
			}
		
			// Change color on button press.
			if (buttonSingleClicked())
			{
				timerRunning = false;
				if (colorCycleCounter == (colorArrayLen - 1))
				{
					// Track how many times through the cycle we've gone
					colorCycleCounter = 0;
					fullCycleCounter += 1;
					// Increase speed of locking timer
					if (lockTimerInterval > 1000)
					{
						lockTimerInterval -= 500;
						lockTimerAnimInterval -= 100;
					}
				}
				else
				{
					colorCycleCounter += 1;
				}
			}
			
			// Display lockign animation and wait for timer to expire & lock.
			if (timerRunning)
			{
				lockAnimLoop(playerColors[colorCycleCounter], lockTimerAnimInterval);
				if (lockTimer.isExpired())
				{
					timerRunning = false;
					gameState = 2;
				}
			}
			else
			{
				setColor(playerColors[colorCycleCounter]);
			}
		break;

		case 2:
		// Lock state
			FOREACH_FACE(f)
			{
				// Set "Locked" pattern"
				if (f % 2 == 0)
				{
					setColorOnFace(playerColors[colorCycleCounter], f);
				}
				else
				{
					setColorOnFace(OFF,f);
				}
				// Listen for a RESET flag
				if (getLastValueReceivedOnFace(f) == RESET)
				{
					setValueSentOnAllFaces(RESET);
					gameState = 0;
				}
			}
			// Send out RESET flag if long pressed in this state.
			if (buttonLongPressed())
			{
				resetVariables();
				setValueSentOnAllFaces(RESET);
				setup();
			}
		break;
	}

}

int matchingFacesCounter()
{
	int matchingFacesCount = 0;
	for (int i = 0; i < 6; i++)
	{
		if (matchingFaces[i] == 0) { continue; }
		else { matchingFacesCount++; }
	}
	return matchingFacesCount;
}

void resetVariables()
{
	colorCycleCounter = 0;
	fullCycleCounter = 0;
	lockAnimCurrentFace = 0;
	lockTimerAnimInterval = 250;
	lockTimerInterval = 3000; 
}

void shuffleColors()
{
	// Randomize both the order of the color and byte flag arrays.
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

void lockAnimLoop(Color currentColor, int interval)
{
	if (lockAnimTimer.isExpired())
	{
		setColor(OFF);
		lockAnimCurrentFace += 1;
		if (lockAnimCurrentFace == 5) { lockAnimCurrentFace = 0;}
		setColorOnFace(currentColor, lockAnimCurrentFace);
		lockAnimTimer.set(interval);
	}
}

