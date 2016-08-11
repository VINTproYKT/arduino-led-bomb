/*
	LED Bomb (Arduino sketch)

	Example visualizes ticking and exploding "states". You can arm the bomb (with defuse code) and then disarm without delays.
	Use Serial Monitor to control the bomb.

	    Note: you may need to connect external LED to ground and `pin_explosive` to check out "explosion power".

	Author: VINTproYKT
*/

const short pin_LED = LED_BUILTIN;// Device visualizing ticking. Change it to your own digital output if you like.
const short pin_explosive = LED_BUILTIN;// Device visualizing explosion. Change it to other digital output if built-in LED starts working wrong (stays dimming, for example).
int LED_voltage = HIGH;// Voltage needed for ticking device
int explosive_voltage = HIGH;// Voltage needed for "explosive" device. Set to `LOW` to prevent explosion effects.

boolean bomb_armed;
String bomb_code;
unsigned short bomb_disarm_attempts;
unsigned short bomb_disarm_maxAttempts = 1;// How many times you can try code per bomb
long bomb_timeout = 30 * 1000;// Default timeout to explode the bomb
long bomb_tick_minDelay = 50;// Minimum tick delay
long bomb_tick_maxDelay = 1000;// Maximum tick delay
long bomb_tick_fadeTimeout = 15 * 1000;// Default timeout for delay decreasing
float bomb_tick_fadeAcceleration = 0.06;// Quantifier for delay decreasing
boolean bomb_tick_done;

long explosion_timeout = 5000;// Explosion duration
long explosion_wave;
long explosion_waves_count = 30;// Overall count of "wave" messages
long explosion_waves_maxDelay = 200;// Maximum wave delay

long time;
long bomb_lastDrawTime;
long bomb_lastTickTime;
long bomb_activationTimeout;
long explosion_lastDrawTime;
long explosion_lastWaveTime;

String message_drawBomb = "Bomb has been planted";
String message_disarmedBomb = "Successfully disarmed";
String message_drawExplosion = "BOOM!";
String message_drawExplosionWave = ".";// "Wave"-visualization message for bomb's explosion

void setup() {
	Serial.begin(9600);
	pinMode(pin_LED, OUTPUT);
	digitalWrite(pin_LED, LOW);
	if (pin_explosive != pin_LED) pinMode(pin_explosive, OUTPUT);
	digitalWrite(pin_explosive, LOW);
	Serial.println("LED Bomb. Use with care.");
}

void loop() {
	time = millis();
	if (bomb_armed) {
		bomb_activationTimeout = bomb_timeout - time + bomb_lastDrawTime;
		if (bomb_activationTimeout <= 0) explodeBomb();
		else {
			// Bomb ticks
			int bomb_tickInterval = time - bomb_lastTickTime;
			int bomb_tick_delay = constrain(bomb_tick_maxDelay - bomb_tick_fadeAcceleration * (bomb_tick_fadeTimeout - bomb_activationTimeout), bomb_tick_minDelay, bomb_tick_maxDelay);
			if (bomb_tickInterval >= bomb_tick_delay && bomb_tick_done) {
				bomb_tick_done = false;
				bomb_lastTickTime = time - bomb_tickInterval + bomb_tick_delay;
				digitalWrite(pin_LED, LED_voltage);
			}
			else if ((float) bomb_tickInterval / (float) bomb_tick_delay > 0.5 && !bomb_tick_done) {// Switch LED off on middle
				bomb_tick_done = true;
				digitalWrite(pin_LED, LOW);
			}
		}
	}
	// Do waves
	if (explosive_voltage > LOW && explosion_waves_count > 0 && explosion_wave > 0) {
		float explosion_waves_delay = (float) explosion_wave / (float) explosion_waves_count * (float) explosion_waves_maxDelay;
		int explosion_waveInterval = time - explosion_lastWaveTime;
		if (explosion_wave <= explosion_waves_count) {
			if (explosion_waveInterval >= (int) explosion_waves_delay) {
				explosion_lastWaveTime = time;
				explosion_wave = explosion_wave + 1;
				Serial.print(message_drawExplosionWave);
			}
		}
		else {
			explosion_wave = 0;
			Serial.println();
		}
	}
	// Prevent voltage on explosive on explosion timeout
	int explosion_drawInterval = time - explosion_lastDrawTime;
	if (explosion_drawInterval >= explosion_timeout) {
		if (pin_explosive != pin_LED) digitalWrite(pin_explosive, LOW);
	}
	
	if (Serial.available()) {
		String input = String(Serial.readStringUntil('\n'));
		input.trim();
		String input_param = String("");
		int input_space = input.indexOf(' ');
		if (input_space > 0) {
			input_param = input.substring(input_space + 1);
			input_param.trim();
		}
		if (input.startsWith("arm")) {// Send serial command `arm` with code. Example: arm 1284
			if (armBomb(input_param)) Serial.println(message_drawBomb);
			else Serial.println("Already armed");
		}
		else if (input.startsWith("disarm")) {// Try to disarm the bomb. Example: disarm 1234
			if (disarmBomb(input_param)) Serial.println(message_disarmedBomb);
		}
		else if (input.startsWith("status")) {// Get current status
			if (bomb_armed) Serial.println(String((double) bomb_activationTimeout / 1000) +" sec left before explosion!");
			else Serial.println("Clear");
		}
		else if (input.startsWith("timeout")) {// Get or set timeout to explode the bomb on the fly (code required to increase, the bomb will explode with wrong code)
			long delta;
			if (input_param.length()) {
				String input_param1 = input_param;
				String input_param2 = String("");
				int input_space = input_param.indexOf(' ');
				if (input_space > 0) {
					input_param1 = input_param.substring(0, input_space);
					input_param1.trim();
					input_param2 = input_param.substring(input_space + 1);
					input_param2.trim();
				}
				delta = (long) (input_param1.toFloat() * 1000) - bomb_timeout;
				if (bomb_armed && delta > 0 && input_param2 != bomb_code) {
					explodeBomb();
					delta = 0;
				}
				if (delta) {
					bomb_timeout += delta;
					if (delta > 0) Serial.print("In");
					else Serial.print("De");
					Serial.println("creased bomb timeout by "+ String(abs((double) delta) / 1000) +" sec");
				}
			}
			if (!input_param.length() || delta) Serial.println("Current timeout: "+ String((double) bomb_timeout / 1000) + " sec");
		}
		else if (input.startsWith("wave")) {// Play "wave" animation
			explosion_wave = 1;
		}
		else if (input.startsWith("suicide")) {
			bomb_timeout /= 2;
		}
	}
}

boolean armBomb(String code) {
	if (!bomb_armed) {
		bomb_armed = true;
		bomb_code = code;
		bomb_disarm_attempts = 0;
		bomb_lastDrawTime = time;
		return true;
	}
	return false;
}

boolean disarmBomb(String code) {
	if (bomb_armed) {
		bomb_disarm_attempts = bomb_disarm_attempts + 1;
		if (code == bomb_code) {
			bomb_armed = false;
			digitalWrite(pin_LED, LOW);
			return true;
		}
		else if (bomb_disarm_attempts >= bomb_disarm_maxAttempts) explodeBomb();
	}
	return false;
}

boolean explodeBomb() {
	if (bomb_armed) {// Bomb exists?
		bomb_armed = false;
		explosion_lastDrawTime = time;
		digitalWrite(pin_LED, LOW);
		digitalWrite(pin_explosive, explosive_voltage);
		explosion_wave = 1;
		Serial.println(message_drawExplosion);
		return true;
	}
	return false;
}
