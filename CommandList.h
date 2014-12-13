
#ifndef COMMAND_LIST__H
#define COMMAND_LIST__H

enum eControlChar
{
  CLEAR_BUFFER = 'b',
  END_OF_MESSAGE = 'e'
};

// Maximum message length in unencoded bytes, excluding the header byte(s).
#define UHEROBOT_BOARD_MSG_MAX_LEN_BYTES 30
#define UHEROBOT_BOARD_MSG_HEADER_MAX_LEN 1

enum eRobotCommandTag
{
    SET_MOTORS_TAG = 0,
    SET_SERVOS_TAG = 1,
    CONTROLLER_HEARTBEAT_TAG = 2,
    ERROR_TAG = 3,
    SET_DIGITAL_PIN_TAG = 4,
    HALT_TAG = 5
};

// Length represents the length of the data part.
enum eRobotCommandLength
{
    SET_MOTORS_LENGTH = 4, // Right motor, left motor. Both are sint16.
    SET_SERVOS_LENGTH = 3, // 1st byte is servo number. Followed by two bytes representing uint16 position.
    CONTROLLER_HEARTBEAT_LENGTH = 0,
    ERROR_LENGTH = 2, // uint16 error bitmask. 0 for no errors. Error bits defined separately.
    SET_DIGITAL_PIN_LENGTH = 2, // uint8 pin number. uint8 state (1 = high, 0 = low)
    HALT_LENGTH = 0
};

enum eRobotResponseTag
{
    ROBOT_HEARTBEAT_TAG = 0,
    ANALOG_READING_TAG = 1
};

enum eRobotResponseLength
{
    ROBOT_HEARTBEAT_LENGHT = 0,
    ANALOG_READING_LENGTH = 3 // uint8 pin, uint16 result.
};

#endif // COMMAND_LIST__H

