#define BAUD_RATE    B9600                // RS232C通信ボーレート

#define ALST_OK	0
#define ECRS_NO	0
#define ECRS_WAIT	2
#define ECRS_YES	3

//#define PULSE_360DEG	2097152
#define PULSE_360DEG	(2097152*4)
//#define PULSE_360DEG	8388608

void serial_init(int fd);

void buffered_write(int fd, char* buf, const unsigned int size);
int send_receive(int fd, char* buf, const unsigned int bufSize, char* buffer);
int getPreviousCommand(const int fd, char* buffer);
int send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize);
int getAlermState(int fd);
int getPulseActual(int fd);
int getVelocityActual(int fd);
int getVelocity(int fd);
int getDOState(int fd);
int getJogVelocity(int fd);
int getGain(int fd);
int getVelocityLoopConstant(int fd);
int getPositionLoopConstant(int fd);
int getAcceralationFilter(int fd);
void clearAlerm(int fd);
void setVelocity(int fd, int velocity);
void setJogVelocity(int fd, int velocity);
void setPulse(int fd, int velocity);
void setSERVOFF(int fd);
void stop(int fd);
void start(int fd);
