
#ifndef COMMAND_LIST__H
#define COMMAND_LIST__H

enum eControlChar
{
  CLEAR_BUFFER = 'b',
  END_OF_MESSAGE = 'e'
};

#define UHEROBOT_BOARD_MSG_MAX_LEN_BYTES 20

enum eRobotCommands
{
    SET_MOTORS = 0,
    SET_SERVOS = 1,
    HEARTBEAT = 2,
    ERROR = 3,
    GET_ANALOG_READING = 4,
    SET_DIGITAL_PIN = 5,
    HALT = 6
};

enum eRobotResponses
{
    HEARTBEAT = 0,
    ANALOG_READING = 1
};

#endif // COMMAND_LIST__H

