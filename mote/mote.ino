#include <assert.h>
#include <EEPROM.h>

#define BUTTON_PIN 5
#define GREEN_LED 6
#define RED_LED 7
#define DEBOUNCE_PERIOD_MS 100

static int16_t deviceId;

static void loadId();
static bool validateCommand(String name);

void setup()
{
	Serial.begin(115200);
	Serial.setTimeout(100);

	loadId();

	pinMode(BUTTON_PIN, INPUT_PULLUP);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);
}

void loop()
{
	static bool pressRegistered = false;
	static long debounceEpoch = 0;
	// YOVO - You Only Validate Once per button press.
	static bool triggerValidation = false;

	int buttonLevel = digitalRead(BUTTON_PIN);

	if (buttonLevel == HIGH)
	{
		pressRegistered = false;
	}

	if (buttonLevel == LOW && !pressRegistered)
	{
		pressRegistered = true;
		debounceEpoch = millis();
		triggerValidation = true;
	}

	if (pressRegistered && (millis() - debounceEpoch) > DEBOUNCE_PERIOD_MS && triggerValidation)
	{
		if (validateCommand("BTN1"))
		{
			digitalWrite(RED_LED, 0);
			digitalWrite(GREEN_LED, 1);
		}
		else
		{
			digitalWrite(GREEN_LED, 0);
			digitalWrite(RED_LED, 1);	
		}

		triggerValidation = false;
	}
}

static void createId()
{
	long randomId = random(RANDOM_MAX);
	EEPROM.write(1, (byte)(randomId & 0x000000FF));
	EEPROM.write(2, (byte)((randomId & 0x0000FF00) >> 8));
}

static void loadId()
{
	// Not reading from the beginning of the EEPROM, because
	// experiments showed that by default there is "magic" 0x69 
	// in the first slot.
	deviceId = ((int16_t)EEPROM.read(1) << 8);
	deviceId |= EEPROM.read(2);

	if (deviceId != 0xFFFF)
		return;

	createId();
	loadId();
}

static bool validateCommand(String name)
{
	assert(name.length() == 4);

	Serial.write((byte*)&deviceId, 2);
	Serial.write(name.c_str(), 4);

	byte response;
	// Lifehacks baby...
	while (Serial.readBytes(&response, 1) == 0);

	// Response 0x01 means OK, everything else is no confirmation.
	return (response == 'Y');
}
