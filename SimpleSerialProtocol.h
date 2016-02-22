#include "Arduino.h"

//CONTROL VALUES
#define START 0
#define END   1

//RECEPTION STATES
#define WAITING_TYPE 0
#define WAITING_DATA 1
#define WAITING_CRC  2
#define NO_MESSAGE   3

class SimpleSerialProtocol
{
public:
    SimpleSerialProtocol();
    bool refresh();
    uint8_t get_type();
    bool has_data();
    uint32_t get_data();

private:
    uint8_t state;
    uint8_t type;
    uint32_t data32;
    uint8_t data_counter;
    uint8_t parity;
    bool valid;

    void process_control_byte(uint8_t byte);
    void process_content_byte(uint8_t byte);
    void reset_data();
    void append_data(uint8_t byte);

    void reset_partity(uint8_t byte);
    void add_partity(uint8_t byte);
    bool check_parity(uint8_t byte);
};
