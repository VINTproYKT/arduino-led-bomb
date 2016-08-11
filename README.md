# LED Bomb (Arduino sketch)
Example visualizes ticking and exploding "states". You can arm the bomb (with defuse code) and then disarm without delays.

Use Serial Monitor to control the bomb.

    Note: you may need to connect external LED to ground and `pin_explosive` to check out "explosion power".

## Outputs
- `pin_LED` // Device visualizing ticking. Change it to your own digital output if you like. Default: `LED_BUILTIN`.
- `pin_explosive` // Device visualizing explosion. Change it to other digital output if built-in LED starts working wrong (stays dimming, for example). Default: `LED_BUILTIN` (**not recommended**).
- serial transmission (TX) LED used to visualize waves right after explosion.

## Connecting all up
1. Connect external LED to *GND* near pin 13.
2. Connect its second electrode to *pin 12*.
3. Set `pin_explosive` to `12`.
4. Set needed voltage for "explosive": `explosive_voltage = HIGH`.
5. Save and upload to board.

## Controlling the bomb
1. Open **Serial Monitor**.
2. Configure its rate on `9600 baud`.
3. Use [commands](#commands).

## Commands
- `arm [code]` // Arm the bomb. Code is optional. Identical code must be used then in `disarm` and `timeout` to defuse the bomb or increase its timeout.
- `disarm [code]` // Disarm the bomb using code. After `bomb_disarm_maxAttempts` mistake(s) bomb explodes.
- `status` // If bomb is armed this will print time remaining before explosion.
- `timeout [duration [code]]` // Get current timeout value or change timeout to `duration` sec. Code required to increase timeout, otherwise bomb will explode.
- `wave` // Play "wave" animation that appears after explosion
- `suicide` // Halve current timeout

## What is supposed to happen?
### On arm
1. LED blinks 1/2 delay.
2. When remaining time goes under `bomb_tick_fadeTimeout`, LED blinks faster.
3. Bomb explodes after `bomb_timeout` msec from arm time.

### On disarm attempt
1. Attempt #1 that can come last if `bomb_disarm_maxAttempts` has default value.
2. If you type wrong code on last attempt, bomb will explode.
3. If you type right code, bomb will be disarmed.

### On timeout change
1. If you set timeout decreased, bomb can accidentally explode in case when timeout from arm time has expired.
2. If you try to set timeout increased with wrong code, bomb will explode.
3. If you set timeout increased with right code, you'll have more time before bomb exploded.

### On explode
1. Bomb is disarmed.
2. Triggering output `pin_explosive` for `explosion_timeout` msec.
3. Playing "wave" animation.

## Number variables
- `bomb_disarm_maxAttempts` // How many times you can try code per bomb. Default: `1`.
- `bomb_timeout` // Default timeout to explode the bomb. Default: `30000` (30 sec).
- `bomb_tick_minDelay` // Minimum tick delay. Default: `50`.
- `bomb_tick_maxDelay` // Maximum tick delay. Default: `1000`.
- `bomb_tick_fadeTimeout` // Default timeout for delay decreasing. Default: `15000` (15 sec).
- `bomb_tick_fadeAcceleration` // Quantifier for delay decreasing. Default: `0.06`.
- `explosion_timeout` // Explosion duration. Default: `5000`.
- `explosion_waves_count` // Overall count of "wave" messages. Default: `30`.
- `explosion_waves_maxDelay` // Maximum wave delay. Default: `200`.

## Contribution
Of course, commit your enhancements to make this a complete *beginner's guide*. I have not ever used any other devices with Arduino board, cause before this I need to order them but can't.

Help me to test this thing on some cool device, so we can include this option then.

You can rewrite this code as you want.
