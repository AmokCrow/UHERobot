
#ifndef COMMAND_LIST__H
#define COMMAND_LIST__H

enum eControlChar
{
  CLEAR_BUFFER = 'b',
  END_OF_MESSAGE = 'e'
};

// Maximum message length in unencoded bytes, excluding the header byte(s).
#define UHEROBOT_BOARD_MSG_MAX_LEN_BYTES 20
#define UHEROBOT_BOARD_MSG_HEADER_MAX_LEN 1

enum eRobotCommands
{
    SET_MOTORS = 0,
    SET_SERVOS = 1,
    CONTROLLER_HEARTBEAT = 2,
    ERROR = 3,
    GET_ANALOG_READING = 4,
    SET_DIGITAL_PIN = 5,
    HALT = 6
};

enum eRobotResponses
{
    ROBOT_HEARTBEAT = 0,
    ANALOG_READING = 1
};

#endif // COMMAND_LIST__H

